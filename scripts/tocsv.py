#!/bin/python

import matplotlib.pyplot as plt
import numpy as np
import sys
import struct
import os

filename = "../"+sys.argv[1]

with open(filename, mode='rb') as file:
    # Read header data 
    fileversion = struct.unpack('b', file.read(1));
    sizex = struct.unpack('i', file.read(4))[0];
    sizey = struct.unpack('i', file.read(4))[0];
    
    # Get data size 
    file_size = os.path.getsize(filename)
    data_size = (int)(np.round((file_size - 5)/(sizex*sizey),0))
    
    # Parse data
    f = 'b';
    if data_size == 4:
        f = 'f';
    ar = np.reshape(struct.unpack(f*sizey*sizex, file.read(data_size*sizey*sizex)),[sizex,sizey])

    # Print data
    for i in range(0, sizex):
        for j in range(0, sizey):
            e = ','
            if j == (sizey-1):
                e = '\n\r'
            print(ar[i][j], end = e);