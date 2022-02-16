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

fin = open("mon.txt", 'r')
fout = open("moncrop2.txt", 'w')

for i,line in enumerate(fin.readlines()):
    if i % 20 == 0:
        fout.write(line)

fdata = pd.read_csv("moncrop2.TXT", sep = ';')
# Extracting the time and altitude parameters by their indexes
p = fdata["P"].to_list()
t = fdata["T"].to_list()


df = pd.DataFrame(data=[p,t])

df.to_csv("data.csv")



# Extracting the time and altitude parameters by their indexes
p = fdata["P"].to_list()
t = fdata["T"].to_list()

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
