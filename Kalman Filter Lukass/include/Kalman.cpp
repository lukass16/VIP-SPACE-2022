#include "Kalman.h"

//https://www.youtube.com/watch?v=G2Ohf9GpHf4
//https://www.kalmanfilter.net/multiSummary.html

using namespace BLA;

BLA::Matrix<3, 3> Q = {0.01, 0, 0, //*Process Noise Uncertainty - how confident are you that the process is correct i.e. that the estimation model is valid
                       0, 0.01, 0,
                       0, 0, 0.01};

BLA::Matrix<1, 1> R_Baro = {0.0327}; //*Measurement Uncertainty - Variance in sensor measurement 

BLA::Matrix<3, 1> X = {
    0, //p
    0, //v
    0  //a
};

BLA::Matrix<3, 3> P = {1, 0, 0, //*Estimate Uncertainty
                       0, 1, 0,
                       0, 0, 1};

BLA::Matrix<3, 3> I = {1, 0, 0,
                       0, 1, 0,
                       0, 0, 1};

BLA::Matrix<1, 3> H_Baro = {1, 0, 0}; //*Observation Matrix - matches feature space of state vector to measurement (z = Hx)

BLA::Matrix<3, 3> F;

BLA::Matrix<1, 1> Z_Baro;
BLA::Matrix<3, 1> K_Baro; //*Kalman Gain

//defining inverse function

BLA::Matrix<1, 1> Inverse(BLA::Matrix<1,1> K)
{
	K(0,0) = 1/K(0,0);
	return K;
}


unsigned long currentTime = 0;
unsigned long prevTime = 0;
float delT = 0.0f;

bool isFirstStep = true;

void predict()
{
  	currentTime = micros();
  	delT = (currentTime - prevTime) / 1000000.0f;
  	//data.loopTime = delT;
  	prevTime = currentTime;

  	if (!isFirstStep)
  	{
		//*State Transition Matrix
    	F = {							
        	1, delT, (delT*delT)/2.0,
        	0, 1, delT,
        	0, 0, 1};

		//*State Extrapolation Equation
    	X = F * X;
    	P = F * P * ~F + Q; //*to be more accurate the Q matrix would be scaled with delT 
  	}
  	isFirstStep = false;
}

void updateBaro(float altitude)
{
	predict();
  	Z_Baro = {altitude};
  	K_Baro = P * ~H_Baro * Inverse(H_Baro * P * ~H_Baro + R_Baro); //*Compute the Kalman Gain

  	X = X + K_Baro * (Z_Baro - H_Baro * X); //*Update estimate with measurement
  	P = (I - K_Baro * H_Baro) * P * (~(I - K_Baro * H_Baro)) + K_Baro * R_Baro * ~K_Baro; //*Update the estimate uncertainty
}

float getKalmanPosition() {
  	return X(0,0);
}

float getKalmanVelocity() {
  	return X(1,0);
}

float getKalmanAcceleration() {
  	return X(2,0);
}




