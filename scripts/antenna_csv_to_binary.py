#!/bin/python
import csv
import sys
import struct

filename_in = sys.argv[1]
filename_out = sys.argv[2]

with open(filename_in) as csv_file:
    csv_reader = csv.reader(csv_file, delimiter = ',');
    line_count = 0
    fileout = open(filename_out, "wb")
    fileout.write(struct.pack('b', 0))
    fileout.write(struct.pack('i', 0))
    for row in csv_reader:
        fileout.write(struct.pack('f', float(row[0])))
        fileout.write(struct.pack('f', float(row[1])))
        line_count = line_count+1
    fileout.seek(1)
    fileout.write(struct.pack('i', line_count))