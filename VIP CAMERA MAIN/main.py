import picamera
import os
from threading import Thread
from mpu9250_jmdev.registers import *
from mpu9250_jmdev.mpu_9250 import MPU9250
import RPi.GPIO as GPIO
import time
from time import sleep
import csv

'''
Defining thread and functions
'''
class VideoThread:
    def __init__(self):
        self._running = True

    def terminate(self):
        self._running = False

    def run(self):
        print("Started video thread\n")

        # Setting up camera
        global filepath
        global flag
        global duration
        global l
        camera = picamera.PiCamera(sensor_mode=5)
        camera.exposure_mode = 'sports'
        camera.iso = 700
        print("Camera setup finished")

        # First recording
        camera.start_recording(filepath + '/1.h264', format='h264', quality=15) #possible to add option - quality - to 'h264' format
        print("First recording in progress")
        # Recording constantly for 1 second intervals while launch has not happened
        while self._running and flag == 0:
            camera.wait_recording(1)
        print("Starting time interval for ascent and deployment recording")
        camera.wait_recording(duration)
        camera.stop_recording()


        #  Recordings after ascent and deployment recording
        print("Starting recordings after ascent and deployment")
        camera.start_recording(filepath + '/2.h264', format='h264', quality=15) #possible to add option - quality - to 'h264' format
        print("Recordings in progress: 2", end = " ")
        camera.wait_recording(l)
        i = 3
        while self._running:
            camera.split_recording(filepath + '/%d.h264' % i, format='h264', quality=15)
            print(i, end = " ")
            camera.wait_recording(l)
            i += 1
        camera.stop_recording()
        print("Stopped recording")
        camera.close()

start_t = int(time.time() * 1000)
def getTime():
    return int(time.time()*1000 - start_t)


print("Start: Setting up Camera")


# Setting up buzzer for indication
BUZZPIN = 5
GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)
GPIO.setup(BUZZPIN, GPIO.OUT)


# Signaling start
GPIO.output(BUZZPIN,GPIO.HIGH)
sleep(0.1)
GPIO.output(BUZZPIN,GPIO.LOW)


# Defining variables
det = 10 # amount of times to detect acceleration outside of boundary
bound = 2.5  # boundary of acceleration value to be detected [g]

duration = 27 # amount of time for ascent and deployment recording [s]
l = 30 # length of single video split after ascent and deployment recording [s]
session = 300 # limiting amount of time of recording after launch [s]
nap = 600 # amount of seconds to sleep after start [s]


# Arranging directories
dirs = os.listdir("/home/pi/Desktop/outputs/")
unique = len(dirs)
filepath = "/home/pi/Desktop/outputs/" + str(unique)

print("Unique id: " + str(unique))
os.mkdir(filepath) #making folder to store videos


# Setting up IMU
print("Setting up IMU")
mpu = MPU9250(
    address_ak=AK8963_ADDRESS,
    address_mpu_master=MPU9050_ADDRESS_68, # Master has 0x68 Address
    address_mpu_slave=MPU9050_ADDRESS_68, # Slave has 0x68 Address
    bus=1,
    gfs=GFS_1000,
    afs=AFS_8G,
    mfs=AK8963_BIT_16,
    mode=AK8963_MODE_C100HZ)

print("Setting acquired biases")
mpu.abias = [-0.04112, 0.05304, -0.02202]
mpu.gbias = [1.69484, 0.59826, 1.20153]

mpu.configure() # Apply the settings to the registers.
print("IMU setup finished")


# Test camera
print("Testing Camera")
testcamera = picamera.PiCamera(sensor_mode=5)
testcamera.exposure_mode = 'sports'
testcamera.iso = 700
testcamera.start_recording(filepath + '/test.h264', format='h264', quality=15)
testcamera.wait_recording(2) # Record for 2 seconds
testcamera.stop_recording()
testcamera.close()


# Signal successful system health check
print("System health check finished")
for i in range(3):
    GPIO.output(BUZZPIN,GPIO.HIGH)
    sleep(0.1)
    GPIO.output(BUZZPIN,GPIO.LOW)
    sleep(0.1)


# Sleep
print("Sleeping for ", nap, " seconds")
sleep(nap)


# Signal ending of sleep mode
print("Sleeping has ended")
GPIO.output(BUZZPIN,GPIO.HIGH)
sleep(1)
GPIO.output(BUZZPIN,GPIO.LOW)


# Opening CSV file
print("Opening CSV file")
f = open(filepath + "/imudata.csv", 'w')
writer = csv.writer(f)
writer.writerow(["Time [ms]","Acc_X [g]","Acc_Y [g]","Acc_Z [g]","Gyr_X [°/s]","Gyr_Y [°/s]","Gyr_Z [°/s]"])


# Starting independent video thread
print("Starting video thread");
flag = 0
video = VideoThread()
videoThread = Thread(target=video.run)
videoThread.start()


# Waiting for acceleration to start delay
print("Waiting for launch...")
k = 0 # amount of times acceleration has been detected outside of boundary
buzzing = False
current_t = getTime()
previous_t = current_t
while(k<det):
    current_t = getTime()

    # Writing IMU data to CSV file
    acc = mpu.readAccelerometerMaster()
    gyr = mpu.readGyroscopeMaster()
    writer.writerow([current_t, acc[0], acc[1], acc[2], gyr[0], gyr[1], gyr[2]])


    # Check if acceleration is above threshold
    if acc[0] > bound:
        k+=1
        print("!", end = " ")
    else:
        k = 0

    # Signaling active recording mode
    if not buzzing and current_t - previous_t > 15000: # buzzing every 15 seconds
        GPIO.output(BUZZPIN,GPIO.HIGH)
        previous_t = current_t
        buzzing = True
    elif buzzing and current_t - previous_t > 1000:
        GPIO.output(BUZZPIN,GPIO.LOW)
        previous_t = current_t
        buzzing = False


flag = 1 #setting launch flag


# Waiting while session ends
print("Starting delay for session")
session_start_t = getTime()
current_t = session_start_t
previous_t = session_start_t
while current_t - session_start_t < session * 1000:
    current_t = getTime()

    # Writing IMU data to CSV file
    acc = mpu.readAccelerometerMaster()
    gyr = mpu.readGyroscopeMaster()
    writer.writerow([current_t, acc[0], acc[1], acc[2], gyr[0], gyr[1], gyr[2]])


    if not buzzing and current_t - previous_t > 15000: # buzzing every 15 seconds
        GPIO.output(BUZZPIN,GPIO.HIGH)
        previous_t = current_t
        buzzing = True
    elif buzzing and current_t - previous_t > 1000:
        GPIO.output(BUZZPIN,GPIO.LOW)
        previous_t = current_t
        buzzing = False

# Closing CSV file
f.close()

# Ending video recording
video.terminate()


print("\nWork finished")
