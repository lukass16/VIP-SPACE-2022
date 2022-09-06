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

"""____READING IN TRUE DATA____"""
ID = 1

data = pd.read_csv("SET_{}.TXT".format(ID), sep=',')

# Extracting the time and altitude parameters by their indexes
t = data["Time [s]"].to_list()
d = data["Time Change [s]"].to_list()
bp = data["Baro_P [m]"].to_list()
p = data["Kalman_P [m]"].to_list()
v = data["Kalman_V [m/s]"].to_list()
a = data["Kalman_A [m/s^2]"].to_list()
g = data["Kalman_Gain_P"].to_list()
u = data["Kalman_Uncertainty_P"].to_list()

print(data.dtypes)

print("There are "+str(len(p))+" data points of positon")
print("There are "+str(len(t))+" data points of time")



"""____MATRIX SETUP____"""
Q = np.array([[0.001, 0, 0], [0, 0.001, 0], [0, 0, 0.00001]]) # Process Noise Uncertainty

T = np.array([[1.0, 1.0, 1.0], [1.0, 1.0, 1.0], [1.0, 1.0, 1.0]]) # Time scaling matrix

R_Baro = np.array(0.5) # Measurement Uncertainty

X = np.array([[0.0], [0.0], [0.0]])

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
        P = np.matmul(np.matmul(F, P), F.T) + Q
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
    K_Baro = np.matmul(P, H_Baro.T) * inverse( np.matmul(np.matmul(H_Baro, P), H_Baro.T) + R_Baro ) # Compute the Kalman Gain
    
    X = X + np.matmul(K_Baro, (Z_Baro - np.matmul(H_Baro, X))) # Update estimate with measurement
    P = np.matmul( np.matmul((I - np.matmul(K_Baro, H_Baro)), P), (I - np.matmul(K_Baro, H_Baro)).T) + np.matmul(K_Baro * R_Baro, K_Baro.T) # Update Estimate Uncertainty
 


"""____TESTING____"""

f_alt_l = list()
k_l = list()
e_t = 0.0 # Emulator time
last_t = 0.0

predict()

for i in range(len(t)):

    #First step: get reading
    reading = bp[i]
    
    # Second step: calculate change in time
    setDelT(d[i])
    
    # Fourth step: predict 
    predict()
    
    # Fifth step: update estimation with reading
    updateBaro(reading)
    
    # Sixth step: get estimation, save values to lists
    f_alt_l.append(X[0,0])
    k_l.append(K_Baro[0, 0])


#plotting
plt.figure(figsize=(20,12))

plt.plot(t, bp, color="b", label = "Raw Altitude [m]")
plt.plot(t, p, color="r", label = "Filtered Altitude [m]")
plt.plot(t, f_alt_l, color="g", label = "Emulator Altitude [m]")

plt.legend()
plt.savefig("Emulator vs FC filter position Set 1.png", dpi=2000)


#plotting
plt.figure(figsize=(20,12))

plt.plot(t, g, color="r", label = "Filtered Gain")
plt.plot(t, k_l, color="g", label = "Emulator Gain")

plt.legend()
#plt.savefig("Emulator vs FC filter gain.png", dpi=1000)