import os
import time
import argparse
os.system("cls")
import paho.mqtt.client as mqtt
import time
import json
import concurrent.futures
import argparse

def on_connect(client, userdata, flags, rc):
    print(f'{PUBLISHER} connected')

def time_log():
    global last_time, current_time
    current_time = round(time.time()*1000)
    print(f"\rTiming: {current_time - last_time: <4}ms")
    last_time = current_time

def count_log():
    global payload
    print(f"Sequence: {payload['sequence']}, Node: {payload['node']}, Quantity: {payload['quantity']: <2}.")
    print("\r\033[F\033[F\033[F\033[F")

def wifi():
    from flask import Flask, request
    app = Flask(__name__)
    
    @app.route('/', methods=['POST'])
    def index():
        global payload, publisher
        payload['sequence'] = request.values.get('SEQUENCE', type = int)
        payload['node'] = request.values.get('NODE', type = str)
        payload['quantity'] = request.values.get('QUANTITY', type = int)
        publisher.publish(topic=payload['node'], payload=json.dumps(payload), qos=0)
        time_log()
        count_log()
        return f"NODE:{payload['node']};SEQUENCE:{payload['sequence'] ^ 1}"

    publisher.connect(host=HOST, port=PORT)
    publisher.loop_start()
    while not publisher.is_connected():
        pass
    app.run(host="0.0.0.0", port=80)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-u", "--url")
    parser.add_argument("-p", "--port")
    args = parser.parse_args()

    HOST = "localhost" if args.url is None else args.url
    PORT = 1883  if args.port is None else args.port
    TOPIC = "test"
    PUBLISHER = "gateway"

    publisher = mqtt.Client(client_id=PUBLISHER, protocol=mqtt.MQTTv311)
    publisher.on_connect = on_connect

    payload = dict()
    current_time = round(time.time()*1000)
    last_time = current_time
    wifi()
    