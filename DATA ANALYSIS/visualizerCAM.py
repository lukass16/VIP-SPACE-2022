# -*- coding: utf-8 -*-
"""
Created on Sat Sep  3 15:56:00 2022

@author: Lukass
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

plt.style.use("seaborn")

ID = "CAM"

data = pd.read_csv("{}.CSV".format(ID), sep=',')

# Extracting the data to lists
t = data["Time [ms]"].to_list()

acc_x = data["Acc_X [g]"].to_list()
acc_y = data["Acc_Y [g]"].to_list()
acc_z = data["Acc_Z [g]"].to_list()
gyr_x = data["Gyr_X [°/s]"].to_list()
gyr_y = data["Gyr_Y [°/s]"].to_list()
gyr_z = data["Gyr_Z [°/s]"].to_list()

# Convert time
start_t = t[0]
t = [x - start_t for x in t]
ts = [x/1000.0 for x in t]

# Obtain rotation frequency
f_x = [x/360.0 for x in gyr_x]
f_y = [x/360.0 for x in gyr_y]
f_z = [x/360.0 for x in gyr_z]

# Printing info
print("Data successfully extracted from {}.CSV, amount:".format(ID), len(t))

# Interval
s, e = (260000, 295000)


# Plot individual imu acc_x voltage data
plt.figure(figsize=(12,8))
plt.xlim(1270,1480)
plt.plot(ts[s:e], acc_z[s:e], color="C0", label = "IMU Acc X [g]")
plt.legend()
plt.title("Individual acc_x data")
plt.savefig("CAM/camimuacc_xdata{}.png".format(ID), dpi=300)

# Plot individual imu acc_x voltage data
plt.figure(figsize=(12,8))
plt.xlim(1270,1480)
plt.plot(ts[s:e], acc_y[s:e], color="C1", label = "IMU Acc Y [g]")
plt.legend()
plt.title("Individual acc_y data")
plt.savefig("CAM/camimuacc_ydata{}.png".format(ID), dpi=300)

# Plot individual imu acc_x voltage data
plt.figure(figsize=(12,8))
plt.xlim(1270,1480)
plt.plot(ts[s:e], acc_z[s:e], color="C2", label = "IMU Acc Z [g]")
plt.legend()
plt.title("Individual acc_z data")
plt.savefig("CAM/camimuacc_zdata{}.png".format(ID), dpi=300)



# Plot individual imu gyr_x voltage data
plt.figure(figsize=(12,8))
plt.xlim(1270,1480)
plt.plot(ts[s:e], gyr_z[s:e], color="C3", label = "IMU Gyr X [°/s]")
plt.legend()
plt.title("Individual gyr_x data")
plt.savefig("CAM/camimugyr_xdata{}.png".format(ID), dpi=300)

# Plot individual imu acc_x voltage data
plt.figure(figsize=(12,8))
plt.xlim(1270,1480)
plt.plot(ts[s:e], gyr_y[s:e], color="C4", label = "IMU Gyr Y [°/s]")
plt.legend()
plt.title("Individual gyr_y data")
plt.savefig("CAM/camimugyr_ydata{}.png".format(ID), dpi=300)

# Plot individual imu acc_x voltage data
plt.figure(figsize=(12,8))
plt.xlim(1270,1480)
plt.plot(ts[s:e], gyr_z[s:e], color="C5", label = "IMU Gyr Z [°/s]")
plt.legend()
plt.title("Individual gyr_z data")
plt.savefig("CAM/camimugyr_zdata{}.png".format(ID), dpi=300)


# Plot individual imu rotation frequency voltage data
plt.figure(figsize=(12,8))
plt.xlim(1270,1480)
plt.plot(ts[s:e], f_x[s:e], color="C6", label = "IMU Rot Freq X [Hz]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual rotation frequency x data")
plt.savefig("CAM/camimurot_freq_xdata{}.png".format(ID), dpi=300)

# Plot individual imu rotation frequency data
plt.figure(figsize=(12,8))
plt.xlim(1270,1480)
plt.plot(ts[s:e], f_y[s:e], color="C7", label = "IMU Rot Freq Y [Hz]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual rotation frequency y data")
plt.savefig("CAM/camimurot_freq_ydata{}.png".format(ID), dpi=300)

# Plot individual imu rotation frequency data
plt.figure(figsize=(12,8))
plt.xlim(1270,1480)
plt.plot(ts[s:e], f_z[s:e], color="C8", label = "IMU Rot Freq Z [Hz]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual rotation frequency z data")
plt.savefig("CAM/camimurot_freq_zdata{}.png".format(ID), dpi=300)
