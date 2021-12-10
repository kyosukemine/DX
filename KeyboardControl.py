from __future__ import print_function

import serial
import sys
import select
import termios
import time


class KeyboardReader():
    def __init__(self):
        # Setup to check keyboard input without blocking
        self.fd = sys.stdin.fileno()
        self.attr_old = termios.tcgetattr(self.fd)
        attr_new = termios.tcgetattr(self.fd)
        attr_new[3] = attr_new[3] & ~termios.ICANON & ~termios.ECHO
        termios.tcsetattr(self.fd, termios.TCSAFLUSH, attr_new)

    def cleanup(self):
        # Reset terminal setting
        termios.tcsetattr(self.fd, termios.TCSAFLUSH, self.attr_old)

    def kbhit(self):
        # Check keyboard input
        dr, dw, de = select.select([sys.stdin], [], [], 0)
        return dr != []

    def getch(self):
        # Read keyboard input
        return sys.stdin.read(1)


class MotorController():
    def __init__(self, baserpm=10):
        # Set serial baudrate
        self.ser = serial.Serial('/dev/ttyUSB0', 19200)
        print("opened " + self.ser.name)

        self.v1 = 0
        self.v2 = 0
        self.v3 = 0

    def cleanup(self):
        # Close the serial port
        self.set_rpms(0, 0, 0)
        self.write_bytes()
        self.ser.close()
        print('closed serial port')

    def set_rpms(self, vx, vy, vtheta):
        """ set each wheel rpm from x, y ans circumferential velocity [rpm]

        Args:
            vx: right positive, left negative
            vy: advance positive, backoff negative
            vtheta: counterclockwise positive, clockwise positive
        """
        self.v1 = -vx - vtheta
        self.v2 = 0.5 * vx - 3**(0.5) * 0.5 * vy - vtheta
        self.v3 = 0.5 * vx + 3**(0.5) * 0.5 * vy - vtheta

    def encode_rpms(self, v1, v2, v3):
        maxv = max(abs(v1), abs(v2), abs(v3))
        if maxv != 0:
            b_v1 = int(127*v1/maxv)
            b_v2 = int(127*v2/maxv)
            b_v3 = int(127*v3/maxv)
        else:
            b_v1 = 0
            b_v2 = 0
            b_v3 = 0
        return int(maxv), b_v1, b_v2, b_v3

    def write_bytes(self):
        maxv, b_v1, b_v2, b_v3 = self.encode_rpms(self.v1, self.v2, self.v3)
        self.ser.write((maxv).to_bytes(1, byteorder='little', signed=True))
        self.ser.write((b_v1).to_bytes(1, byteorder='little', signed=True))
        self.ser.write((b_v3).to_bytes(1, byteorder='little', signed=True))
        self.ser.write((b_v2).to_bytes(1, byteorder='little', signed=True))


def main():
    baserpm = 10
    time_thresh = 0.5   # [s]
    time_new = time.time()
    time_old = time_new
    key = '0'
    key_old = key

    try:
        kr = KeyboardReader()
        ms = MotorController()
        while True:
            time_new = time.time()
            if kr.kbhit():
                key = kr.getch()
                print(f'key: {key}')
                if (key != key_old):
                    if key == 27:   # ESC
                        break
                    elif key == 'w':    # advance
                        ms.set_rpms(0, baserpm, 0)
                    elif key == 'a':    # left
                        ms.set_rpms(-baserpm, 0, 0)
                    elif key == 's':    # backoff
                        ms.set_rpms(0, -baserpm, 0)
                    elif key == 'd':    # right
                        ms.set_rpms(baserpm, 0, 0)
                    elif key == 'q':    # turn left
                        ms.set_rpms(-baserpm, -baserpm, -baserpm)
                    elif key == 'e':    # turn right
                        ms.set_rpms(baserpm, baserpm, baserpm)
                    ms.write_bytes()

            if time_new - time_old > time_thresh:
                ms.set_rpms(0, 0, 0)
                ms.write_bytes()

            key_old = key
            time_old = time_new
    finally:
        ms.set_rpms(0, 0, 0)
        ms.write_bytes()
        kr.cleanup()
        ms.cleanup()


if __name__ == '__main__':
    main()
