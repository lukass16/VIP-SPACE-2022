import pandas as pd
import matplotlib.pyplot as plt

"""
-----------------------------------------------------------------------------
    TOOL TO PLOT DATA FROM WATER ROCKET AT ESC 2021
-----------------------------------------------------------------------------
"""


#IMPORTING DATA, you may have to change the location
data = pd.read_csv("./DATA001.TXT",sep = '\t')


"""
-----------------------------------------------------------------------------
            Time
-----------------------------------------------------------------------------
"""


time = data["Time[s]"].to_list()

"""
-----------------------------------------------------------------------------
            Pressure/altitude
-----------------------------------------------------------------------------
"""

fig_p, ax1 = plt.subplots()

pressure = data["Pressure[Pa]"].to_list()
altitude = data["Alt[m]"].to_list()

ax1.plot(time,pressure)
ax1.set_ylabel('Pressure [Pa]')
ax1.set_xlabel('Time [s]')
ax1.set_title('Pressure')


"""
-----------------------------------------------------------------------------
            Gyroscope
-----------------------------------------------------------------------------
"""

gyro_x = data["Gyro_x[º/s]"].to_list()
gyro_y = data["Gyro_y[º/s]"].to_list()
gyro_z = data["Gyro_z[º/s]"].to_list()

fig_gyro = plt.figure()
ax_gyro = fig_gyro.add_subplot(111)    # The big subplot
ax_gyro1 = fig_gyro.add_subplot(311)
ax_gyro2 = fig_gyro.add_subplot(312)
ax_gyro3 = fig_gyro.add_subplot(313)

ax_gyro1.plot(time,gyro_x)
ax_gyro1.set_title('x')
ax_gyro1.tick_params(axis='x',labelcolor='none')

ax_gyro2.plot(time,gyro_y)
ax_gyro2.set_title('y')
ax_gyro2.tick_params(axis='x',labelcolor='none')

ax_gyro3.plot(time,gyro_z)
ax_gyro3.set_title('z')

ax_gyro.spines['top'].set_color('none')
ax_gyro.spines['bottom'].set_color('none')
ax_gyro.spines['left'].set_color('none')
ax_gyro.spines['right'].set_color('none')
ax_gyro.tick_params(labelcolor='w', top=False, bottom=False, left=False, right=False)
ax_gyro.set_xlabel('Time [s]')
ax_gyro.set_ylabel('Acceleration [m/s$^2$')


ax_gyro.set_xlabel('Time [s]')
ax_gyro.set_ylabel('Angular velocity around axis [$\degree$ /s]')

"""
-----------------------------------------------------------------------------
            Acceleration
-----------------------------------------------------------------------------
"""

acc_x = data["Acc_x[G]"].to_list()
acc_y = data["Acc_y[G]"].to_list()
acc_z = data["Acc_z[G]"].to_list()

fig_acc = plt.figure()
ax_acc = fig_acc.add_subplot(111)    # The big subplot
ax_acc1 = fig_acc.add_subplot(311)
ax_acc2 = fig_acc.add_subplot(312)
ax_acc3 = fig_acc.add_subplot(313)

ax_acc1.plot(time,acc_x)
ax_acc1.set_title('x')
ax_acc1.tick_params(axis='x',labelcolor='none')

ax_acc2.plot(time,acc_y)
ax_acc2.set_title('y')
ax_acc2.tick_params(axis='x',labelcolor='none')

ax_acc3.plot(time,acc_z)
ax_acc3.set_title('z')

ax_acc.spines['top'].set_color('none')
ax_acc.spines['bottom'].set_color('none')
ax_acc.spines['left'].set_color('none')
ax_acc.spines['right'].set_color('none')
ax_acc.tick_params(labelcolor='w', top=False, bottom=False, left=False, right=False)
ax_acc.set_xlabel('Time [s]')
ax_acc.set_ylabel('Acceleration [m/s$^2$')


"""
-----------------------------------------------------------------------------
            Temperature
-----------------------------------------------------------------------------
"""

temp_bme =  data["Temp_bme[*C]"].to_list()
temp_imu =  data["Temp_imu[*C]"].to_list()

fig_T, ax_T = plt.subplots()

ax_T.plot(time,temp_bme)
ax_T.plot(time,temp_imu)

fig_T.legend(['bme','imu'])

ax_T.set_ylabel('Temperature [$\degree C$]')
ax_T.set_xlabel('Time [s]')

ax_T.set_title('Temperature')


plt.show()