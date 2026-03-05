import json
import requests
import websocket

REMOTE_DEBUGGING_PORT = 5000
CHROME_URL = f"http://localhost:{REMOTE_DEBUGGING_PORT}/json"

def set_cookies(cookies, ws_url):
    ok_counter = 0
    error_counter = 0
    ws = websocket.create_connection(ws_url)
    request_id = 1

    for cookie in cookies:
        ws.send(json.dumps(cookie))
        response = ws.recv()
        resp_data = json.loads(response)

        if "error" in resp_data:
            print(f"[ERROR] Cookie '{cookie['params']['name']}' rejected: {resp_data['error']['message']}")
            error_counter += 1
        else:
            print(f"[OK] Cookie '{cookie['params']['name']}' set successfully.")
            ok_counter += 1

        request_id += 1

    ws.close()
    print(f"Ok: {ok_counter}\nError: {error_counter}")

def main():
    with open("cookies.json", "r") as f:
        cookies = json.load(f)

    tabs = requests.get(CHROME_URL).json()
    if not tabs:
        print("No Chrome tabs open with remote debugging enabled.")
        return

    ws_url = tabs[0]["webSocketDebuggerUrl"]
    set_cookies(cookies, ws_url)

if __name__ == "__main__":
    main()