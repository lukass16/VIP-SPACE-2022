# -*- coding: utf-8 -*-
"""
Created on Sat Sep  3 15:56:00 2022

@author: Lukass
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

""" SETUP """

plt.style.use("seaborn")

ID = "FC"

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

# Convert time
start_t = t[0]
t = [x - start_t for x in t]
ts = [x/1000.0 for x in t]

# Obtain rotation frequency
f_x = [x/2/3.14159 for x in gyr_x]
f_y = [x/2/3.14159 for x in gyr_y]
f_z = [x/2/3.14159 for x in gyr_z]

# Printing info
print("Data successfully extracted from {}.CSV, amount:".format(ID), len(t))

# Extracting state change info
# 1 - PREPARATION, 2 - LAUNCH WAIT, 3 - LAUNCH DETECTED, 4 - MAIN WAIT, 5 - DESCENT, 6 TOUCHDOWN DETECTED
t_apogee = 0
t_main_eject = 0
for i, el in enumerate(s):
    if el == 4 and t_apogee == 0:
        t_apogee = t[i]
    elif el == 5:
        t_main_eject = t[i]
        break
ts_apogee = t_apogee / 1000.0
ts_main_eject = t_main_eject / 1000.0


""" ANALYSIS """
print("Maximum GPS altitude: ", max(g_alt))
print("Maximum Filtered altitude: ", max(f_alt))
print("Maximum Filtered velocity: ", max(f_vel))
print("Maximum Filtered acceleration: ", max(f_acc))
print("Minimum Filtered acceleration: ", min(f_acc))
print("Minimum Filtered velocity: ", min(f_vel))


# Plot intervaled altitude data
"""
start, end = (5000, 6000)
plt.figure(figsize=(12,8))
#plt.ylim(-100,2000)
plt.plot(ts[start:end], r_alt[start:end], color="b", label = "Raw Altitude [m]")
plt.plot(ts[start:end], f_alt[start:end], color="r", label = "Filtered Altitude [m]")
plt.legend()
plt.title("Altitude data")
plt.savefig("FC/altdataint4{}.png".format(ID), dpi=300)
"""

""" PLOTTING """

# Plot individual altitude data
plt.figure(figsize=(12,8))
plt.plot(t, g_alt, color="b", label = "GPS Altitude [m]")
#plt.axvline(x = t_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = t_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("GPS altitude data")
plt.savefig("FC/gpsaltdata{}.png".format(ID), dpi=300)


# Plot GPS altitude and satellite data
fig, ax1 = plt.subplots(figsize=(12,8))
ax2 = ax1.twinx()
ax2.set_ylim(0, 20)

ax1.plot(t, g_alt, color="b", label = "GPS Altitude [m]")
ax2.plot(t, sats, color="r", label = "Satellites")
ax1.set_ylabel("GPS Altitude [m]", color='b')
ax2.set_ylabel("Satellites", color='r')
ax1.set_xlabel("Time [ms]")
#plt.legend()
plt.title("GPS data")
plt.savefig("FC/gpsaltsatsdata{}.png".format(ID), dpi=300)
    


# Plot barometer positional data
fig, axis = plt.subplots(2, 2, figsize=(16,8))

fig.suptitle('Positional Data')

axis[0, 0].plot(ts, p, color="b", label = "Pressure [hPa]")
axis[0, 0].axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
axis[0, 0].axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
axis[0, 0].legend()

axis[0, 1].plot(ts, r_alt, color="b", label = "Raw Altitude [m]")
axis[0, 1].plot(ts, f_alt, color="r", label = "Filtered Altitude [m]")
axis[0, 1].axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
axis[0, 1].axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
axis[0, 1].legend()

axis[1, 0].plot(ts, f_vel, color="m", label = "Filtered Velocity [m/s]")
axis[1, 0].axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
axis[1, 0].axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
axis[1, 0].legend()

axis[1, 1].plot(ts, f_acc, color="y", label = "Filtered Acceleration [m/s]")
axis[1, 1].axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
axis[1, 1].axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
axis[1, 1].legend()

plt.savefig("FC/posdata{}.png".format(ID), dpi=300)


# Plot individual pressure data
plt.figure(figsize=(12,8))
plt.plot(ts, p, color="b", label = "Pressure [hPa]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual pressure data")
plt.savefig("FC/presdata{}.png".format(ID), dpi=300)

# Plot altitude data
plt.figure(figsize=(12,8))
plt.ylim(-100,2000)
plt.plot(ts, r_alt, color="b", label = "Raw Altitude [m]")
plt.plot(ts, f_alt, color="r", label = "Filtered Altitude [m]")
plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Altitude data")
plt.savefig("FC/altdata{}.png".format(ID), dpi=300)

# Plot individual velocity data
plt.figure(figsize=(12,8))
plt.plot(ts, f_vel, color="m", label = "Baro Vert Vel [m/s]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual velocity data")
plt.savefig("FC/veldata{}.png".format(ID), dpi=300)

# Plot individual acceleration data
plt.figure(figsize=(12,8))
plt.plot(ts, f_acc, color="y", label = "Baro Vert Acc [m/s^2]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual acceleration data")
plt.savefig("FC/accdata{}.png".format(ID), dpi=300)

# Plot individual temperature data
plt.figure(figsize=(12,8))
plt.plot(ts, temp, color="r", label = "Baro Temp [°C]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual temperature data")
plt.savefig("FC/tempdata{}.png".format(ID), dpi=300)


# Plot individual battery voltage data
plt.figure(figsize=(12,8))
plt.plot(ts, bat, color="g", label = "Bat 1 Voltage [V]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual battery voltage data")
plt.savefig("FC/batdata{}.png".format(ID), dpi=300)


# Plot IMU acceleration data
fig, axis = plt.subplots(1, 3, figsize=(30,5))

fig.suptitle('IMU acceleration data')

axis[0].plot(ts, acc_x, color="y", label = "IMU Acc X [m/s^2]")
axis[0].legend()

axis[1].plot(ts, acc_y, color="g", label = "IMU Acc Y [m/s^2]")
axis[1].legend()

axis[2].plot(ts, acc_z, color="b", label = "IMU Acc Z [m/s^2]")
axis[2].legend()

plt.savefig("FC/imuaccdata{}.png".format(ID), dpi=300)

# Plot individual imu acc_x voltage data
plt.figure(figsize=(12,8))
plt.plot(ts, acc_x, color="y", label = "IMU Acc X [m/s^2]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual acc_x data")
plt.savefig("FC/imuacc_xdata{}.png".format(ID), dpi=300)

# Plot individual imu acc_x voltage data
plt.figure(figsize=(12,8))
plt.plot(ts, acc_y, color="g", label = "IMU Acc Y [m/s^2]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual acc_y data")
plt.savefig("FC/imuacc_ydata{}.png".format(ID), dpi=300)

# Plot individual imu acc_x voltage data
plt.figure(figsize=(12,8))
plt.plot(ts, acc_z, color="b", label = "IMU Acc Z [m/s^2]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual acc_z data")
plt.savefig("FC/imuacc_zdata{}.png".format(ID), dpi=300)


# Plot IMU gyroscope data
fig, axis = plt.subplots(1, 3, figsize=(30,5))

fig.suptitle('IMU acceleration data')

axis[0].plot(ts, gyr_x, color="m", label = "IMU Gyr X [rad/s]")
axis[0].legend()

axis[1].plot(ts, gyr_y, color="c", label = "IMU Gyr Y [rad/s]")
axis[1].legend()

axis[2].plot(ts, gyr_z, color="k", label = "IMU Gyr Z [rad/s]")
axis[2].legend()

plt.savefig("FC/imugyrdata{}.png".format(ID), dpi=300)

# Plot individual imu gyr_x data
plt.figure(figsize=(12,8))
plt.plot(ts, gyr_x, color="m", label = "IMU Gyr X [rad/s]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual gyr_x data")
plt.savefig("FC/imugyr_xdata{}.png".format(ID), dpi=300)

# Plot individual imu gyr_y data
plt.figure(figsize=(12,8))
plt.plot(ts, gyr_y, color="c", label = "IMU Gyr Y [rad/s]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual gyr_y data")
plt.savefig("FC/imugyr_ydata{}.png".format(ID), dpi=300)

# Plot individual imu gyr_z data
plt.figure(figsize=(12,8))
plt.plot(ts, gyr_z, color="k", label = "IMU Gyr Z [rad/s]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual gyr_z data")
plt.savefig("FC/imugyr_zdata{}.png".format(ID), dpi=300)


# Plot individual imu rotation frequency voltage data
plt.figure(figsize=(12,8))
plt.plot(ts, f_x, color="m", label = "IMU Rot Freq X [Hz]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual rotation frequency x data")
plt.savefig("FC/imurot_freq_xdata{}.png".format(ID), dpi=300)

# Plot individual imu rotation frequency data
plt.figure(figsize=(12,8))
plt.plot(ts, f_y, color="c", label = "IMU Rot Freq Y [Hz]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual rotation frequency y data")
plt.savefig("FC/imurot_freq_ydata{}.png".format(ID), dpi=300)

# Plot individual imu rotation frequency data
plt.figure(figsize=(12,8))
plt.plot(ts, f_z, color="k", label = "IMU Rot Freq Z [Hz]")
#plt.axvline(x = ts_apogee, color = 'tab:orange', label = 'Apogee Detected')
#plt.axvline(x = ts_main_eject, color = 'g', label = 'Main Ejected')
plt.legend()
plt.title("Individual rotation frequency z data")
plt.savefig("FC/imurot_freq_zdata{}.png".format(ID), dpi=300)
