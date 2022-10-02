# -*- coding: utf-8 -*-
"""
Created on Sat Sep  3 15:56:00 2022

@author: Lukass
"""

import numpy as np
import time

import random
from random import randint
import matplotlib.pyplot as plt
import pandas as pd

"""
NOTE: Should check what happens if we reverse predict and update steps
NOTE: Should check what happens if remove time scaling - evaluate whether time scaling is necessary

https://stats.stackexchange.com/questions/49300/how-does-one-apply-kalman-smoothing-with-irregular-time-steps
"so to properly incorporate measurements at irregular time intervals, all you really need to do is to ensure that your process noise covariance is scaled by a factor of the square root of the difference in time between the last filter state and the current measurement time (Δ)"

* TUNABLE PARAMETERS:
Q - regulates how much you trust the dynamics of the chosen model - gets scaled with time (seconds)
P - estimate uncertainty - how much you trust your estimate (but initially this just means how much you trust your initial estimate)
R - measurement uncertainty - for the MS5607 sensor this uncertainty (variance) was measured to be 0.0282 - the higher this is the less you trust your sensor measurements

* SEQUENCE OF CALCULATIONS  
1) Calculate delT - time interval from last estimate
2) Predict current state
3) Update estimate according to prediction and measurement (updateBaro())
"""

"""
List of tested values deemed to be ok:
    
SET 1 - could potentially change v to 0.01
Q = np.array([[0.001, 0, 0], [0, 0.001, 0], [0, 0, 0.00001]]) # Process Noise Uncertainty 
R_Baro = np.array(0.5) # Measurement Uncertainty
"""





"""____MATRIX SETUP____"""
Q = np.array([[0.001, 0, 0], [0, 0.001, 0], [0, 0, 0.00001]]) # Process Noise Uncertainty

T = np.array([[1.0, 1.0, 1.0], [1.0, 1.0, 1.0], [1.0, 1.0, 1.0]]) # Time scaling matrix

R_Baro = np.array(0.5) # Measurement Uncertainty

X = np.array([[10.0], [0.0], [0.0]])

P = np.array([[10.0, 10.0, 10.0], [10.0, 10.0, 10.0], [10.0, 10.0, 10.0]])

I = np.array([[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]])

H_Baro = np.array([[1.0, 0.0, 0.0]])

F = np.array([[0.0, 0.0, 0.0], [0.0, 0.0, 0.0], [0.0, 0.0, 0.0]])

Z_Baro = np.array(0.0) 

K_Baro = np.array([[0.0], [0.0], [0.0]])



"""____VARIABLE SETUP____"""
delT = 0.0
firstStep = True
start_time = time.time() * 1000



"""____FUNCTIONS FOR KALMAN FILTER____"""
def inverse(K):
    K[0, 0] = 1 / K[0, 0]
    return K


def setDelT(d):
    global delT
    delT = d


def predict():
    global F
    global X
    global delT
    global firstStep
    global P
    
    if not firstStep:
        # State Transition Matrix
        F = np.array([[1, delT, (delT*delT)/2.0], [0.0, 1.0, delT], [0.0, 0.0, 1.0]])
        
        # Update scaling matrix
        T.fill(1) # !Time scaling is currently disabled
        
        # State extrapolation equation
        X = np.matmul(F, X)
        P = np.matmul(np.matmul(F, P), F.T) + Q # !Time scaling is currently disabled
    else:
        firstStep = False


def updateBaro(altitude):
    global Z_Baro
    global K_Baro
    global P
    global H_Baro
    global R_Baro
    global X
    global I
    
    Z_Baro.fill(altitude)
    K_Baro = np.matmul( np.matmul(P, H_Baro.T), inverse( np.matmul(np.matmul(H_Baro, P), H_Baro.T) + R_Baro )) # Compute the Kalman Gain
    
    X = X + np.matmul(K_Baro, (Z_Baro - np.matmul(H_Baro, X))) # Update estimate with measurement
    P = np.matmul( np.matmul((I - np.matmul(K_Baro, H_Baro)), P), (I - np.matmul(K_Baro, H_Baro)).T) + np.matmul(K_Baro * R_Baro, K_Baro.T) # Update Estimate Uncertainty
 
    
 
"""____FUNCTIONS FOR SIMULATION____"""
def getTime(): # returns time [ms]
        return int(time.time() * 1000 - start_time) 
    


"""____SIMULATION____"""
###____CASE 'A'____###

"""
### Setup
random.seed(12)

t = 0.0 # Time used in code [s]
last_t = 0.0
duration = 20 # Duration of simulation [s]
alt = 10 # True altitude
iteration = 0.01 # Iteration of simulation [s]

alt_l = list()
f_alt_l = list()
t_l = list()
k_l = list()

### Define reading
def getReading():
    return alt + (float)(randint(-30, 30) / 100.0)

### Start simulation
print("Simulating case 'A'")
predict()
while t < duration:
    
    # First step: iterate
    t+=iteration
    
    # Second step: get reading
    reading = getReading()
    
    # Third step: calculate change in time
    setDelT(t - last_t)
    last_t = t
    
    # Fourth step: predict 
    predict()
    
    # Fifth step: update estimation with reading
    updateBaro(reading)
    
    # Sixth step: get estimation, save values to lists
    t_l.append(t)
    alt_l.append(reading)
    f_alt_l.append(X[0,0])
    k_l.append(K_Baro[0, 0])
    
### Plotting
plt.figure(figsize=(10,6))
plt.title("Raw Altitude vs Filtered Altitude: Case 'A'")
plt.plot(t_l, alt_l, color="b", label = "Raw Altitude [m]")
plt.plot(t_l, f_alt_l, color="r", label = "Filtered Altitude [m]")
plt.legend()
#plt.savefig("A_Set_1.png", dpi=1000)


plt.figure(figsize=(10,6))
plt.title("Kalman Gain Position: Case 'A'")
plt.plot(t_l, k_l, color="c", label = "Kalman Gain - Position")
plt.legend()

"""



###____CASE 'B'____###
"""
### Setup
random.seed(20)

t = 0.0 # Time used in code [s]
last_t = 0.0
duration = 5 # Duration of simulation [s]
p = 0.0 # True inital altitude [m]
v = 25.0 # True speed [m/s]
a = -9.8 # True acceleration [m/s^2]
iteration = 0.01 # Iteration of simulation [s]

alt_l = list()
f_alt_l = list()
t_l = list()
k_l = list()

### Define reading
def getReading(t):
    noise = (float)(randint(-30, 30) / 100.0)
    return (float)(p + v*t + a * t * t / 2.0) + noise

### Start simulation
print("Simulating Case 'B'")
predict()
while t < duration:
    
    # First step: iterate
    t+=iteration
    
    # Second step: get reading
    reading = getReading(t)
    
    # Third step: calculate change in time
    setDelT(t - last_t)
    last_t = t
    
    # Fourth step: predict 
    predict()
    
    # Fifth step: update estimation with reading
    updateBaro(reading)
    
    # Sixth step: get estimation, save values to lists
    t_l.append(t)
    alt_l.append(reading)
    f_alt_l.append(X[0,0])
    k_l.append(K_Baro[0, 0])
    
### Plotting
plt.figure(figsize=(10,6))
plt.title("Raw Altitude vs Filtered Altitude: Case 'B'")
plt.plot(t_l, alt_l, color="b", label = "Raw Altitude [m]")
plt.plot(t_l, f_alt_l, color="r", label = "Filtered Altitude [m]")
plt.legend()
#plt.savefig("B_Set_1.png", dpi=1000)

plt.figure(figsize=(10,6))
plt.title("Kalman Gain Position: Case 'B'")
plt.plot(t_l, k_l, color="c", label = "Kalman Gain - Position")
plt.legend()
"""


###____CASE 'C'____###
"""
### Setup
random.seed(20)

t = 0.0 # Time used in code [s]
last_t = 0.0
duration = 5 # Duration of simulation [s]
alt = 10 # True altitude
iteration = 0.005 # Iteration of simulation [s]

spike_amplitude = 70 # Spike amplitude [m]
spike_duration = 0.01 # Spike duration [s]
spike_location = 2.5 # Spike time of occurrence [s]

alt_l = list()
f_alt_l = list()
t_l = list()
k_l = list()

### Define reading
def getReading(t):
    # Introduce large spike noise
    if t > spike_location and t < spike_location + spike_duration:
        return alt + (float)(randint(-20, 20) / 100.0) + spike_amplitude
    else:
        return alt + (float)(randint(-20, 20) / 100.0)


### Start simulation
print("Simulating case 'C'")
predict()
while t < duration:
    
    # First step: iterate
    t+=iteration
    
    # Second step: get reading
    reading = getReading(t)
    
    # Third step: calculate change in time
    setDelT(t - last_t)
    last_t = t
    
    # Fourth step: predict 
    predict()
    
    # Fifth step: update estimation with reading
    updateBaro(reading)
    
    # Sixth step: get estimation, save values to lists
    t_l.append(t)
    alt_l.append(reading)
    f_alt_l.append(X[0,0])
    k_l.append(K_Baro[0, 0])
    
### Plotting
plt.figure(figsize=(10,6))
plt.title("Raw Altitude vs Filtered Altitude: Case 'C'")
plt.plot(t_l, alt_l, color="b", label = "Raw Altitude [m]")
plt.plot(t_l, f_alt_l, color="r", label = "Filtered Altitude [m]")
plt.legend()
#plt.savefig("C_Set_1.png", dpi=1000)


plt.figure(figsize=(10,6))
plt.title("Kalman Gain Position: Case 'C'")
plt.plot(t_l, k_l, color="c", label = "Kalman Gain - Position")
plt.legend()
"""


###____CASE 'D'____###
"""
### Setup
random.seed(20)

t = 0.0 # Time used in code [s]
last_t = 0.0
duration = 31 # Duration of simulation [s]
p = 0.0 # True inital altitude [m]
v = 150.0 # True speed [m/s]
a = -9.8 # True acceleration [m/s^2]
iteration = 0.01 # Iteration of simulation [s]

spike_amplitude = 200 # Spike amplitude [m]
spike_duration = 0.04 # Spike duration [s]
spike_location = 2 # Spike time of occurrence [s]

alt_l = list()
f_alt_l = list()
t_l = list()
k_l = list()

### Define reading
def getReading(t):
    noise = (float)(randint(-50, 50) / 100.0)
    # Introduce large spike noise
    if t > spike_location and t < spike_location + spike_duration:
        return (float)(p + v*t + a * t * t / 2.0) + noise - spike_amplitude
    else:
        return (float)(p + v*t + a * t * t / 2.0) + noise
    
### Start simulation
print("Simulating Case 'B'")
predict()
while t < duration:
    
    # First step: iterate
    t+=iteration
    
    # Second step: get reading
    reading = getReading(t)
    
    # Third step: calculate change in time
    setDelT(t - last_t)
    last_t = t
    
    # Fourth step: predict 
    predict()
    
    # Fifth step: update estimation with reading
    updateBaro(reading)
    
    # Sixth step: get estimation, save values to lists
    t_l.append(t)
    alt_l.append(reading)
    f_alt_l.append(X[0,0])
    k_l.append(K_Baro[0, 0])
    
### Plotting
plt.figure(figsize=(10,6))
plt.title("Raw Altitude vs Filtered Altitude: Case 'D'")
plt.plot(t_l, alt_l, color="b", label = "Raw Altitude [m]")
plt.plot(t_l, f_alt_l, color="r", label = "Filtered Altitude [m]")
plt.legend()
plt.savefig("D_Set_1_spike2s.png", dpi=1000)


plt.figure(figsize=(10,6))
plt.title("Kalman Gain Position: Case 'D'")
plt.plot(t_l, k_l, color="c", label = "Kalman Gain - Position")
plt.legend()
"""


###____CASE 'E'____###
"""
### Read in data
data = pd.read_csv("openrocketsim.csv", sep=',')

t = data["Time (s)"].to_list()
p = data["Altitude (m)"].to_list()

### Setup
random.seed(20)

e_t = 0.0 # Emulator time
last_t = 0.0

spike_amplitude = 300 # Spike amplitude [m]
spike_duration = 0.8 # Spike duration [s]
spike_location = 19 # Spike time of occurrence [s]

alt_l = list()
f_alt_l = list()
t_l = list()
k_l = list()

### Define reading
def getReading(p, t):
    noise = (float)(randint(-50, 50) / 100.0)
    # Introduce large spike noise
    if t > spike_location and t < spike_location + spike_duration:
        return p + noise - spike_amplitude
    else:
        return p + noise
    
### Start simulation
print("Simulating Case 'E'")
predict()

for i in range(len(t)):

    # First step: iterate
    e_t = t[i]
    
    # Second step: get reading
    reading = getReading(p[i], t[i])
    
    # Third step: calculate change in time
    setDelT(e_t - last_t)
    last_t = e_t
    
    # Fourth step: predict 
    predict()
    
    # Fifth step: update estimation with reading
    updateBaro(reading)
    
    # Sixth step: get estimation, save values to lists
    t_l.append(e_t)
    alt_l.append(reading)
    f_alt_l.append(X[0,0])
    k_l.append(K_Baro[0, 0])
    
    
### Plotting

# Limit observed time - 739 datapoints in total are available
start = 462
end = 471

plt.figure(figsize=(10,6))
plt.title("Raw Altitude vs Filtered Altitude: Case 'E'")
plt.plot(t_l[start:end], alt_l[start:end], color="b", label = "Raw Altitude [m]")
plt.plot(t_l[start:end], f_alt_l[start:end], color="r", label = "Filtered Altitude [m]")
plt.legend()
#plt.savefig("Ealt2_Set_2.png", dpi=1000)


plt.figure(figsize=(10,6))
plt.title("Kalman Gain Position: Case 'D'")
plt.plot(t_l[start:end], k_l[start:end], color="c", label = "Kalman Gain - Position")
plt.legend()
plt.show()
"""


###____CASE 'F'____###
"""
### Read in data
data = pd.read_csv("real3.csv", sep=',')

t = data["Time"].to_list()
p = data["Altitude"].to_list()

e_t = 0.0 # Emulator time
last_t = 0.0

alt_l = list()
f_alt_l = list()
t_l = list()
k_l = list()

### Define reading
def getReading(i):
    return p[i]
    
### Start simulation
print("Simulating Case 'E'")
predict()

for i in range(len(t)):

    # First step: iterate
    e_t = t[i]
    
    # Second step: get reading
    reading = getReading(i)
    
    # Third step: calculate change in time
    setDelT(e_t - last_t)
    last_t = e_t
    
    # Fourth step: predict 
    predict()
    
    # Fifth step: update estimation with reading
    updateBaro(reading)
    
    # Sixth step: get estimation, save values to lists
    t_l.append(e_t)
    alt_l.append(reading)
    f_alt_l.append(X[0,0])
    k_l.append(K_Baro[0, 0])
    
    
### Plotting

plt.figure(figsize=(10,6))
plt.title("Raw Altitude vs Filtered Altitude: Case 'F'")
plt.plot(t_l, alt_l, color="b", label = "Raw Altitude [m]")
plt.plot(t_l, f_alt_l, color="r", label = "Filtered Altitude [m]")
plt.legend()


plt.figure(figsize=(10,6))
plt.title("Kalman Gain Position: Case 'D'")
plt.plot(t_l[start:end], k_l[start:end], color="c", label = "Kalman Gain - Position")
plt.legend()
plt.show()
"""


###____CASE 'G'____###
"""
### Read in data
data = pd.read_csv("real2.csv", sep=',')

t_r = data["timestamp_s"].to_list()
t = list()

start_t = t_r[0]
for i, el in enumerate(t_r):
    t.append((el - start_t) / 1000.0)
p = data["baro_height"].to_list()

e_t = 0.0 # Emulator time
last_t = 0.0

alt_l = list()
f_alt_l = list()
t_l = list()
k_l = list()

### Define reading
def getReading(i):
    return p[i]
    
### Start simulation
print("Simulating Case 'E'")
predict()

for i in range(len(t)):

    # First step: iterate
    e_t = t[i]
    
    # Second step: get reading
    reading = getReading(i)
    
    # Third step: calculate change in time
    setDelT(e_t - last_t)
    last_t = e_t
    
    # Fourth step: predict 
    predict()
    
    # Fifth step: update estimation with reading
    updateBaro(reading)
    
    # Sixth step: get estimation, save values to lists
    t_l.append(e_t)
    alt_l.append(reading)
    f_alt_l.append(X[0,0])
    k_l.append(K_Baro[0, 0])
    
    
### Plotting

plt.figure(figsize=(10,6))
plt.title("Raw Altitude vs Filtered Altitude: Case 'G'")
plt.plot(t_l, alt_l, color="b", label = "Raw Altitude [m]")
plt.plot(t_l, f_alt_l, color="r", label = "Filtered Altitude [m]")
plt.legend()



plt.figure(figsize=(10,6))
plt.title("Kalman Gain Position: Case 'G'")
plt.plot(t_l[start:end], k_l[start:end], color="c", label = "Kalman Gain - Position")
plt.legend()
plt.show()
"""


###____CASE 'H'____###
"""
### Read in data
data = pd.read_csv("real1.csv", sep=',')

t_r = data["timestamp_s"].to_list()
t = list()

start_t = t_r[0]
for i, el in enumerate(t_r):
    t.append((el - start_t) / 1000.0)
p = data["baro_height"].to_list()

e_t = 0.0 # Emulator time
last_t = 0.0

alt_l = list()
f_alt_l = list()
t_l = list()
k_l = list()

### Define reading
def getReading(i):
    return p[i]
    
### Start simulation
print("Simulating Case 'E'")
predict()

for i in range(len(t)):

    # First step: iterate
    e_t = t[i]
    
    # Second step: get reading
    reading = getReading(i)
    
    # Third step: calculate change in time
    setDelT(e_t - last_t)
    last_t = e_t
    
    # Fourth step: predict 
    predict()
    
    # Fifth step: update estimation with reading
    updateBaro(reading)
    
    # Sixth step: get estimation, save values to lists
    t_l.append(e_t)
    alt_l.append(reading)
    f_alt_l.append(X[0,0])
    k_l.append(K_Baro[0, 0])
    
    
### Plotting

plt.figure(figsize=(10,6))
plt.title("Raw Altitude vs Filtered Altitude: Case 'H'")
plt.plot(t_l, alt_l, color="b", label = "Raw Altitude [m]")
plt.plot(t_l, f_alt_l, color="r", label = "Filtered Altitude [m]")
plt.legend()



plt.figure(figsize=(10,6))
plt.title("Kalman Gain Position: Case 'H'")
plt.plot(t_l[start:end], k_l[start:end], color="c", label = "Kalman Gain - Position")
plt.legend()
plt.show()
"""


