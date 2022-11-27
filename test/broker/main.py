import paho.mqtt.client as mqtt
import time
import json
import concurrent.futures
import argparse
parser = argparse.ArgumentParser()
parser.add_argument("-u", "--url")
parser.add_argument("-p", "--port")
args = parser.parse_args()

HOST = "localhost" if args.url is None else args.url
PORT = 1883  if args.port is None else args.port
TOPIC = "test"
SUBSCRIBER = "subscriber"
PUBLISHER = "publiser"

def on_connect(client, userdata, flags, rc):
    if client._client_id.decode('utf-8') == SUBSCRIBER and rc == 0:
        print(f'{SUBSCRIBER} connected')
    elif client._client_id.decode('utf-8') == PUBLISHER and rc == 0:
        print(f'{PUBLISHER} connected')

def on_message(client, userdata, message):
    data = json.loads(message.payload.decode('utf-8'))
    try:
        print(f"received {data['quantity']}")
        if data["quantity"] == 5:
            client.loop_stop()
            client.disconnect()
    except KeyError:
        print('key error')

subscriber = mqtt.Client(client_id=SUBSCRIBER, protocol=mqtt.MQTTv311)
publisher = mqtt.Client(client_id=PUBLISHER, protocol=mqtt.MQTTv311)

subscriber.on_message = on_message
subscriber.on_connect = on_connect
publisher.on_connect = on_connect

def connect_client(client):
    client.connect(host=HOST, port=PORT)
    client.loop_start()
    while not client.is_connected():
        pass

    if client._client_id.decode('utf-8') == SUBSCRIBER:
        client.subscribe(topic=TOPIC, qos=0)
    elif client._client_id.decode('utf-8') == PUBLISHER:
        payload = {"quantity" : 0}
        while payload["quantity"] <= 5:
            print(f"sending {payload['quantity']}")
            client.publish(topic=TOPIC, payload=json.dumps(payload), qos=0)
            payload["quantity"] += 1
            time.sleep(1)
        client.loop_stop()
        client.disconnect()
    return client._client_id.decode('utf-8')

with concurrent.futures.ThreadPoolExecutor(max_workers = 5) as executor:
   results = [executor.submit(connect_client, client) for client in [subscriber, publisher]]
   for f in concurrent.futures.as_completed(results):
        print(f"{f.result()} is done")