#!/usr/bin/env python
'''
wait for serial port to be ready
we 1st need to loose the port and then wait to get it again
'''

import os
from serial.tools import list_ports
import argparse

parser = argparse.ArgumentParser(description="simpler serial port listener", formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument("-u", "--upload", action="store_true", help="if set, it waits to go down and up again")
args = parser.parse_args()

print("waiting for serial terminal...")
waiting_for_port = True
lost_port_once = False
while waiting_for_port:
    port_is_available = [p.description for p in list_ports.comports() if "Nano" in p.description]
    if args.upload:
        if not port_is_available:
            lost_port_once = True

        if lost_port_once and port_is_available:
            waiting_for_port = False
    else:
        waiting_for_port = not port_is_available

for port in list_ports.comports():
    if "Nano" in port.description:
        os.system('./serialterminal.py --port {port}'.format(port=port.device))
        # os.system('pio device monitor --port {port}'.format(port=port.device))
