from mpu9250_jmdev.registers import *
from mpu9250_jmdev.mpu_9250 import MPU9250#
import time
import csv


start_t = int(time.time() * 1000)
def getTime():
    return int(time.time()*1000 - start_t)

print("Starting IMU readings")

mpu = MPU9250(
    address_ak=AK8963_ADDRESS,
    address_mpu_master=MPU9050_ADDRESS_68, # Master has 0x68 Address
    address_mpu_slave=MPU9050_ADDRESS_68, # Slave has 0x68 Address
    bus=1,
    gfs=GFS_1000,
    afs=AFS_8G,
    mfs=AK8963_BIT_16,
    mode=AK8963_MODE_C100HZ)

mpu.configure() # Apply the settings to the registers.


'''
print("Calibrating")
#Calibrating
mpu.calibrateMPU6500() # Calibrate sensors
mpu.configure() # The calibration function resets the sensors, so you need to reconfigure the
print("Calibration finished")

abias = mpu.abias # Get the master accelerometer biases
abias_slave = mpu.abias_slave # Get the slave accelerometer biases
gbias = mpu.gbias # Get the master gyroscope biases
gbias_slave = mpu.gbias_slave # Get the slave gyroscope biases

print("Sensor biases:")
print("abias: ", abias)
print("abias_slave: ", abias_slave)
print("gbias: ", gbias)
print("gbias_slave: ", gbias_slave)
'''

print("Setting acquired biases")
mpu.abias = [-0.04112, 0.05304, -0.02202]
mpu.gbias = [1.69484, 0.59826, 1.20153]

mpu.configure()


# Opening CSV file
f = open('test.csv', 'w')
writer = csv.writer(f)
writer.writerow(["Time [ms]","Acc_X [g]","Acc_Y [g]","Acc_Z [g]","Gyr_X [°/s]","Gyr_Y [°/s]","Gyr_Z [°/s]"])

for i in range(0,10):
    print("|.....MPU9250 in 0x68 I2C Bus - Master.....|")
    print("Accelerometer", mpu.readAccelerometerMaster())
    print("Gyroscope", mpu.readGyroscopeMaster())
    print("Magnetometer", mpu.readMagnetometerMaster())
    print("Temperature", mpu.readTemperatureMaster())
    print("\n")
    time.sleep(0.3)

    acc = mpu.readAccelerometerMaster()
    gyr = mpu.readGyroscopeMaster()

    writer.writerow([getTime(), acc[0], acc[1], acc[2], gyr[0], gyr[1], gyr[2]])


# Close csv file
f.close()
