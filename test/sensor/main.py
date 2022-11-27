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
seq = -1
current_time = round(time.time()*1000)
last_time = current_time

def time_log():
    global last_time, current_time
    current_time = round(time.time()*1000)
    print(f"\rTiming: {current_time - last_time: <4}ms")
    last_time = current_time

def count_log():
    global count, name, seq
    print(f"Sequence: {seq}, Node: {name}, Quantity: {count: <2}.")
    print("\r\033[F\033[F\033[F\033[F")

def uart():
    global count, name, seq
    import serial.tools.list_ports
    print("Received UART")
    baudrate = 115200 if args.baudrate is None else args.baudrate
    com = 'COM8' if args.com is None else args.com

    ser = serial.Serial (com, baudrate)
    while True:
        time_log()
        i_data = ser.readline()[:-1].decode('utf-8')
        data = {val.split(':')[0] : val.split(':')[1] for val in i_data.split(';')}
        count = int(data['QUANTITY'].strip())
        name = data['NODE'].strip()
        seq = int(data['SEQUENCE'].strip())
        count_log()
        ser.write(f'NODE:{name:<4};SEQUENCE:{seq ^ 1}'.encode())

def wifi():
    from flask import Flask, request
    app = Flask(__name__)
    
    @app.route('/', methods=['POST'])
    def index():
        global count, name, seq
        seq = request.values.get('SEQUENCE', type = int)
        name = request.values.get('NODE', type = str)
        count = request.values.get('QUANTITY', type = int)
        time_log()
        count_log()
        return f'NODE:{name};SEQUENCE:{seq ^ 1}'

    app.run(host="0.0.0.0", port=80)


if __name__ == "__main__":
    try:
        globals()[args.type]()
    except KeyError:
        print(f"Connection {args.type} type not support")

    