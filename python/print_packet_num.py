import serial
import numpy as np
import matplotlib.pyplot as plt
import argparse

MAX_LINE = 300

def segmentTo16bit(segments):    
    img16 = np.zeros((120, 160), np.uint16)
    img16[0:60, 0:80]     = (segments[0][:,4::2]) + (segments[0][:,5::2] * 256)
    img16[0:60, 80:160]   = (segments[1][:,4::2]) + (segments[1][:,5::2] * 256)
    img16[60:120, 0:80]   = (segments[2][:,4::2]) + (segments[2][:,5::2] * 256)
    img16[60:120, 80:160] = (segments[3][:,4::2]) + (segments[3][:,5::2] * 256)
    return img16

def extractSegment(data):
    segments = []
    segments.append(data[0:60,:])
    segments.append(data[60:120,:])
    segments.append(data[120:180,:])
    segments.append(data[120:180,:])
    return segments
    
def readData(ser, lines):
    print("Ready")
    x = ser.read(164 * lines)
    if len(x) != (164 * lines):
        print("Timeout received {} bytes of data".format(len(x)))
    x = np.array(bytearray(x))
    y = x[0:(len(x)//164)*164]
    z = y.reshape((len(x)//164, 164))
    return x, z

def printPackets(data):    
    packet_num = data[:,0]
    packet_num = packet_num.astype(np.int)
    packet_num *= 256
    packet_num += data[:,1]
    np.set_printoptions(formatter={'int':hex})
    print(packet_num)


#ser = serial.Serial("/dev/ttyACM0", 115200, timeout=5)
ser = serial.Serial("COM32", 115200, timeout=5)
ser.reset_input_buffer()
raw, data = readData(ser, 300)
printPackets(data)
ser.close()