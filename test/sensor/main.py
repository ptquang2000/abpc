import os
import time
import argparse
os.system("cls")
parser = argparse.ArgumentParser()
parser.add_argument("-t", "--type")
parser.add_argument("-b", "--baudrate")
parser.add_argument("-c", "--com")
args = parser.parse_args()

count = 0
name = ''
current_time = round(time.time()*1000)
last_time = current_time

def time_log():
    global last_time, current_time
    current_time = round(time.time()*1000)
    print(f"\rTiming: {current_time - last_time: <4}ms")
    last_time = current_time

def count_log():
    global count
    print(f"Node: {name} Quantity: {count: <2}")
    print("\r\033[F\033[F\033[F\033[F")

def uart():
    global count, name
    import serial.tools.list_ports
    print("Received UART")
    baudrate = 115200 if args.baudrate is None else args.baudrate
    com = 'COM8' if args.com is None else args.com

    ser = serial.Serial (com, baudrate)
    while True:
        data = str(ser.readline())
        time_log()
        while data[0] != '!':
            data = data[1:]
        temp = ''
        key = ''
        pointer = data[1:]
        while pointer[0] != '#':
            temp += pointer[0]
            pointer = pointer[1:]
            if pointer[0] == ':':
                key = temp
                temp = ''
                pointer = pointer[1:]
        if key == 'QUANTITY':
            count = int(temp.strip())
            count_log()
        elif key == 'NODE':
            name = temp.strip()

def wifi():
    from flask import Flask, request
    app = Flask(__name__)
    
    @app.route('/')
    def index():
        global count, name
        name = request.args.get('NODE', type = str)
        count = request.args.get('QUANTITY', type = int)
        time_log()
        count_log()
        return 'Received'

    app.run(host="0.0.0.0", port=80)


if __name__ == "__main__":
    try:
        globals()[args.type]()
    except KeyError:
        print(f"Connection {args.type} type not support")

    