import serial
import numpy as np
import argparse

MAX_LINE = 300

ser = serial.Serial("/dev/ttyACM0", 115200, timeout=5)

print("Ready")
x = ser.read(164*MAX_LINE)
x = np.array(bytearray(x))
y = x[0:(len(x)//164)*164]
z = y.reshape((len(x)//164, 164))
packet_num = z[:,0]
packet_num = packet_num.astype(np.int)
packet_num *= 256
packet_num += z[:,1]
np.set_printoptions(formatter={'int':hex})
print(packet_num)