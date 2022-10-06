# -*- coding: utf-8 -*-
"""
Created on Sat Sep  3 15:56:00 2022

@author: Lukass
"""

import pandas as pd
import matplotlib.pyplot as plt

ID = 2

data = pd.read_csv("{}.CSV".format(ID), sep=',')

# Extracting the data to lists
t = data["Time [ms]"].to_list()

lat = data["Lat"].to_list()
lng = data["Lng"].to_list()
g_alt = data["GPS Alt [m]"].to_list()
sats = data["Sats"].to_list()

p = data["Pressure [hPa]"].to_list()
r_alt = data["Raw Baro Alt [m]"].to_list()
f_alt = data["Filter Baro Alt [m]"].to_list()
f_vel = data["Baro Vert Vel [m/s]"].to_list()
f_acc = data["Baro Vert Acc [m/s^2]"].to_list()
temp = data["Baro Temp [°C]"].to_list()

bat = data["Bat 1 Voltage [V]"].to_list()

acc_x = data["IMU Acc X [m/s^2]"].to_list()
acc_y = data["IMU Acc Y [m/s^2]"].to_list()
acc_z = data["IMU Acc Z [m/s^2]"].to_list()
gyr_x = data["IMU Gyr X [deg/s]"].to_list()
gyr_y = data["IMU Gyr Y [deg/s]"].to_list()
gyr_z = data["IMU Gyr Z [deg/s]"].to_list()

s = data["State"].to_list()


# Printing info
print("Data successfully extracted from {}.CSV, amount:".format(ID), len(t))


# Plot positional data
fig, axis = plt.subplots(2, 2, figsize=(12,8))

fig.suptitle('Positional Data')

axis[0, 0].plot(t, p, color="b", label = "Pressure [hPa]")
axis[0, 0].legend()

axis[0, 1].plot(t, r_alt, color="b", label = "Raw Altitude [m]")
axis[0, 1].plot(t, f_alt, color="r", label = "Filtered Altitude [m]")
axis[0, 1].legend()

axis[1, 0].plot(t, f_alt, color="m", label = "Filtered Velocity [m/s]")
axis[1, 0].legend()

axis[1, 1].plot(t, f_acc, color="y", label = "Filtered Acceleration [m/s]")
axis[1, 1].legend()

plt.savefig("posdata{}.png".format(ID), dpi=300)

