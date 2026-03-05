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

    cookie_list = cookies["params"]["cookies"]
    for cookie in cookie_list:
        params = {
            "name": cookie.get("name"),
            "value": cookie.get("value"),
            "domain": cookie.get("domain"),
            "path": cookie.get("path", "/"),
            "secure": cookie.get("secure", False),
            "httpOnly": cookie.get("httpOnly", False),
            "sameSite": cookie.get("sameSite", "None"),
            "expires": cookie.get("expires", 0)
        }

        message = {
            "id": request_id,
            "method": "Network.setCookie",
            "params": params
        }

        ws.send(json.dumps(message))
        response = ws.recv()
        resp_data = json.loads(response)

        if "error" in resp_data:
            print(f"[ERROR] Cookie '{params['name']}' rejected: {resp_data['error']['message']}")
            error_counter += 1
        else:
            print(f"[OK] Cookie '{params['name']}' set successfully.")
            ok_counter += 1

        request_id += 1

    ws.close()
    print(f"Ok: {ok_counter}\nError: {error_counter}")

def main():
    with open("temp.json", "r") as f:
        cookies = json.load(f)

    tabs = requests.get(CHROME_URL).json()
    if not tabs:
        print("No Chrome tabs open with remote debugging enabled.")
        return

    ws_url = tabs[0]["webSocketDebuggerUrl"]
    set_cookies(cookies, ws_url)

if __name__ == "__main__":
    main()