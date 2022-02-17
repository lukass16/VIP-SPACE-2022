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

data = pd.read_csv("filtered2.TXT",sep = ',')

# Extracting the time and altitude parameters by their indexes
p = data["P"].to_list()
v = data["V"].to_list()
a = data["A"].to_list()

#Finding the length of the lists
print("There are "+str(len(p))+" data points of positon")

plt.plot(p)


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