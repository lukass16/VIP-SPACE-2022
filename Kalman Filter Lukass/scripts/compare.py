# -*- coding: utf-8 -*-
"""
Created on Tue Aug  3 09:41:40 2021

Small script demonstrating importing of datasets, slicing lists, filtering
of noise and labeling/legend on plots

@author: gaute
"""
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import csv

#----------------------------------------------------------------------------
#    Reading the .txt, putting it in a pandas dataframe
#----------------------------------------------------------------------------

#

fdata = pd.read_csv("altitudeEx.TXT", sep = ',')
# Extracting the time and altitude parameters by their indexes
p_data = fdata["P"].to_list()
t = fdata["T"].to_list()

data = pd.read_csv("filtered.TXT", sep = ',')
p = data["P"].to_list()
v = data["V"].to_list()
a = data["A"].to_list()

print("There are "+str(len(p))+" data points of positon")
print("There are "+str(len(t))+" data points of time")


plt.figure(figsize=(8,6))

plt.plot(t, p_data, color="b", label = "Raw Altitude [m]")
plt.plot(t, p, color="r", label = "Filtered Altitude [m]")
plt.legend()
#plt.show()
plt.savefig("pos2.png", dpi=1000)

plt.figure(figsize=(8,6))
plt.plot(t, v, color="c", label = "Kalman velocity [m/s]")
plt.legend()
#plt.show()
plt.savefig("vel2.png", dpi=1000)

plt.figure(figsize=(8,6))
plt.plot(t, a, color="m", label = "Kalman acceleration [m/s^2]")
plt.legend()
#plt.show()
plt.savefig("acc2.png", dpi=1000)
#plt.plot(t, p)

#Finding the length of the lists
print("There are "+str(len(p))+" data points of positon")
print("There are "+str(len(t))+" data points of time")

'''
#Finding the length of the lists
print("There are "+str(len(p))+" data points of positon")
print("There are "+str(len(t))+" data points of time")


plt.figure(1)

plt.plot(t, p, color="r", label = "Filtered Altitude [m]")
#plt.plot(t, p_data, color="b", label = "Raw Altitude [m]")
plt.legend()
plt.show()
'''

'''
#----------------------------------------------------------------------------
#    Cropping the data by index
#----------------------------------------------------------------------------
start_index = 6580
end_index = 6855
cropped_altitude = altitude[start_index:end_index]
cropped_time = time[start_index:end_index]

cropped_time = [x-cropped_time[0] for x in cropped_time]

with open("altitude.csv", "w", newline='') as out:
    csv_writer = csv.writer(out, delimiter = ",")
    for i in range(len(cropped_time)):
         line = [cropped_altitude[i], cropped_time[i]]
         csv_writer.writerow(line)
'''
