# -*- coding: utf-8 -*-
"""
Created on Wed Jun  1 19:59:26 2022

@author: lukas
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import csv

# Baro_P [m],Kalman_P [m],Kalman_V [m/s],Kalman_A [m/s^2],Kalman_Gain_P,Kalman_Uncertainty_P
#----------------------------------------------------------------------------
#    Reading the .txt, putting it in a pandas dataframe
#----------------------------------------------------------------------------

ID = 27

data = pd.read_csv("KALMAN{}.TXT".format(ID), sep=',')

# Extracting the time and altitude parameters by their indexes
t = data["Time [s]"].to_list()
bp = data["Baro_P [m]"].to_list()
p = data["Kalman_P [m]"].to_list()
v = data["Kalman_V [m/s]"].to_list()
a = data["Kalman_A [m/s^2]"].to_list()
g = data["Kalman_Gain_P"].to_list()
u = data["Kalman_Uncertainty_P"].to_list()

print(data.dtypes)

print("There are "+str(len(p))+" data points of positon")
print("There are "+str(len(t))+" data points of time")



#plotting
plt.figure(figsize=(8,6))

plt.plot(t, bp, color="b", label = "Raw Altitude [m]")
plt.plot(t, p, color="r", label = "Filtered Altitude [m]")
plt.legend()
plt.show()
plt.savefig("pos{}.png".format(ID), dpi=1000)

plt.figure(figsize=(8,6))
plt.plot(t, v, color="c", label = "Kalman velocity [m/s]")
plt.legend()
#plt.show()
plt.savefig("vel{}.png".format(ID), dpi=1000)

plt.figure(figsize=(8,6))
plt.plot(t, a, color="m", label = "Kalman acceleration [m/s^2]")
plt.legend()
#plt.show()
plt.savefig("acc{}.png".format(ID), dpi=1000)

plt.figure(figsize=(8,6))
plt.plot(t, g, color="b", label = "Kalman Gain - Altitude")
plt.legend()
#plt.show()
plt.savefig("kgain{}.png".format(ID), dpi=1000)

plt.figure(figsize=(8,6))
plt.plot(t, u, color="y", label = "Kalman Gain - Uncertainty")
plt.legend()
#plt.show()
plt.savefig("uncert{}.png".format(ID), dpi=1000)

