# -*- coding: utf-8 -*-
"""
Created on Mon Oct 24 18:48:17 2022

@author: Lukass
"""

#note if code does not work - install pyserial library via "pip3 install pyserial" command in terminal

import serial
import csv  

header = ["Time [ms]","Lat","Lng","GPS Alt [m]","Sats","Pressure [hPa]","Raw Baro Alt [m]","Filter Baro Alt [m]","Baro Vert Vel [m/s]","Baro Vert Acc [m/s^2]","Baro Temp [°C]","Bat 1 Voltage [V]","IMU Acc X [m/s^2]","IMU Acc Y [m/s^2]","IMU Acc Z [m/s^2]","IMU Gyr X [deg/s]","IMU Gyr Y [deg/s]","IMU Gyr Z [deg/s]","State"]
ser = serial.Serial('COM9', baudrate=115200)


with open('data.csv', 'w', encoding='UTF8', newline='') as f:
    writer = csv.writer(f)
    
    # write the header
    writer.writerow(header)
    
    line = "test"
    
    while True:
        b_line = ser.readline()   # read a '\n' terminated line
        line = b_line.decode("utf-8") 
        arrline = line.strip().split(",")
        writer.writerow(arrline)
