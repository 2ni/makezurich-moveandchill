#!/usr/bin/env python

'''
howto send test string:
import serial
ser = serial.Serial('/dev/cu.wchusbserial1410', 19200, timeout=.1)
ser.write("Hello".encode())
'''

import serial
import argparse
from datetime import datetime as dt
import sys

parser = argparse.ArgumentParser(description='simpler serial port listener', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-p', '--port', type=str, default='', required=True, help='give port to listen from, eg "/dev/cu.wchusbserial1410"')
parser.add_argument('-b', '--baudrate', type=int, default=19200, help='give port speed in baud, eg 19200')
parser.add_argument('-d', '--datestamp', action='store_true', help='show datestamp on output')

args = parser.parse_args()

print("-- Miniterm on {port} {baud},8,N,1 --".format(port=args.port, baud=args.baudrate))

printTimestamp = True
if not args.datestamp:
    printTimestamp = False

# 8bits, parity none, stop bit
client = serial.Serial(args.port, args.baudrate, timeout=1)
try:
    while True:
        data = client.read()
        if data:
            if printTimestamp:
                now = dt.now()
                seconds = int(now.strftime('%S'))
                micros = int(now.strftime('%f'))
                precision = round(micros / 100000)
                if precision == 10:
                    seconds += 1
                    precision = 0

                print('{now}:{seconds:02d}.{precision}: '.format(
                    now=now.strftime("%H:%M"),
                    seconds=seconds,
                    precision=precision), end='')
                # print('{}: '.format(now.strftime("%H:%M:%S.%f")[:-5]), end='')
                printTimestamp = False

            try:
                print(data.decode('utf-8'), end='', flush=True)
            except UnicodeDecodeError:
                pass

            if args.datestamp and ord(data) == 10:
                printTimestamp = True

except KeyboardInterrupt:
    pass
except OSError:
    print("no port found")
    sys.exit(0)
