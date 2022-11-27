# TEST MODULE 

## Sensor
Testing data transmission between gateway and sensor node.

| Connection | Decsiption |
|-|-|
| uart | UART |
| wifi | WIFI |

| Options | Decsiption |
|-|-|
| -t | function name of connection interface |
| -b | baudrate if use function uart |
| -p | serial port if use function uart |

```bash
# cd to test/sensor
python main.py -t {type}
```

## Broker
A publisher will public quantity message in 5 seconds, while a subscriber will receive it. 
```bash
# cd to test/broker
docker-compose -p test up --abort-on-container-exit
```