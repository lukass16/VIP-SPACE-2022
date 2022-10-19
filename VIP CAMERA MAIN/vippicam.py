import picamera
import os

print("Starting recording process")
l = 7 #length of single video split
n = 5 # amount of video splits

dirs = os.listdir("/home/pi/Desktop/outputs/")
unique = len(dirs)
filepath = "/home/pi/Desktop/outputs/" + str(unique)

print("Unique id: " + str(unique))

os.mkdir(filepath) #making folder to store videos

#setting up camera
camera = picamera.PiCamera(sensor_mode=5)
camera.exposure_mode = 'sports'
camera.iso = 700
""" #optional arguments by Toms
camera.iso = 300
camera.awb_mode = 'incandescent'
camera.brightness = 45 #45-55
"""

print("Camera setup finished")

#first iteration
camera.start_recording(filepath + '/1.h264') #possible to add option - quality - to 'h264' format
print("Recordings in progress: 1", end = " ")
camera.wait_recording(l)

for i in range(2, n+1):
    camera.split_recording(filepath + '/%d.h264' % i)
    print(i,end = " ")
    camera.wait_recording(l)

camera.stop_recording()
print("\nRecording finished")
