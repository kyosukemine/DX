from __future__ import print_function

import serial
import sys
import select
import termios
import time
import atexit


def kbhit():
    # Check keyboard input
    dr, dw, de = select.select([sys.stdin], [], [], 0)
    return dr != []


def getch():
    # Read keyboard input
    return sys.stdin.read(1)


class RobotController():
    def __init__(self):
        port = '/dev/ttyUSB0'
        baudrate = 19200

        key_dict = {}
        key_dict['stop'] = '0'
        key_dict['advance'] = 'w'
        key_dict['back'] = 's'
        key_dict['right'] = 'd'
        key_dict['left'] = 'a'

        atexit.register(self.cleanup)

        # Setup to check keyboard input without blocking
        self.fd = sys.stdin.fileno()
        self.attr_old = termios.tcgetattr(self.fd)
        attr_new = termios.tcgetattr(self.fd)
        attr_new[3] = attr_new[3] & ~termios.ICANON & ~termios.ECHO
        termios.tcsetattr(self.fd, termios.TCSAFLUSH, attr_new)
        
        # Set serial baudrate
        self.ser = serial.Serial(port, baudrate)
        print("Opened " + self.ser.name)

        # Read starting message
        msg = self.ser.readline()
        print(str(msg))

    def cleanup(self):
        # Close the serial port
        self.ser.close()
        print('Closed serial port')

        # Reset terminal setting
        termios.tcsetattr(self.fd, termios.TCSAFLUSH, self.attr_old)

    def send_key(self):


    def run(self, key):
        stop_key = '0'
        advance_key = 'w'
        back_key = 's'
        right_key = 'd'
        left_key = 'a'
        self.ser.write(stop_key)


    







