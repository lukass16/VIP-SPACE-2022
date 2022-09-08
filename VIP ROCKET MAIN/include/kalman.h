#pragma once
#include "BasicLinearAlgebra.h"

/*
* TUNABLE PARAMETERS:
Q - regulates how much you trust the dynamics of the chosen model - gets scaled with time (seconds)
P - estimate uncertainty - how much you trust your estimate (but initially this just means how much you trust your initial estimate)
R - measurement uncertainty - for the MS5607 sensor this uncertainty (variance) was measured to be 0.0282 - the higher this is the less you trust your sensor measurements

* SEQUENCE OF CALCULATIONS  
1) Calculate delT - time interval from last estimate
2) Predict current state
3) Update estimate according to prediction and measurement (updateBaro())
*/

//https://www.youtube.com/watch?v=G2Ohf9GpHf4
//https://www.kalmanfilter.net/multiSummary.html


using namespace BLA;

namespace kalman {

    float delT = 0.0f;
    bool isFirstStep = true;

    BLA::Matrix<3, 3> Q = {0.001, 0, 0, //*Process Noise Uncertainty - how confident are you that the process is correct i.e. that the estimation model is valid
                           0, 0.001, 0,
                           0, 0, 0.00001};

    BLA::Matrix<3, 3> T = {0.01, 0, 0, //*Time scaling matrix
                           0, 0.01, 0,
                           0, 0, 0.01};

    BLA::Matrix<1, 1> R_Baro = {0.5}; //*Measurement Uncertainty - Variance in sensor measurement - measured to be 0.0282

    BLA::Matrix<3, 1> X = {
        0, //p
        0, //v
        0  //a
    };

    BLA::Matrix<3, 3> P = {100.0, 0, 0, //*Estimate Uncertainty
                           0, 100.0, 0,
                           0, 0, 100.0};

    BLA::Matrix<3, 3> I = {1, 0, 0,
                           0, 1, 0,
                           0, 0, 1};

    BLA::Matrix<1, 3> H_Baro = {1, 0, 0}; //*Observation Matrix - matches feature space of state vector to measurement (z = Hx)

    BLA::Matrix<3, 3> F;

    BLA::Matrix<1, 1> Z_Baro;
    BLA::Matrix<3, 1> K_Baro; //*Kalman Gain

    //defining inverse function
    BLA::Matrix<1, 1> Inverse(BLA::Matrix<1, 1> K)
    {
        K(0, 0) = 1 / K(0, 0);
        return K;
    }

    //function for setting change in time (which is used for estimate and estiamte uncertainty calculations)
    void setDelT(float t_change)
    {
        delT = t_change;
    }

    void predict()
    {
        if (!isFirstStep)
        {
            //*State Transition Matrix
            F = {
                1, delT, (delT * delT) / 2.0,
                0, 1, delT,
                0, 0, 1};

            //*Update scaling matrix - matrix consists of delT and scales Q according to delT
            T.Fill(1); // removed time scaling for now

            //*State Extrapolation Equation
            X = F * X;
            P = F * P * ~F + Q;
        }
        isFirstStep = false;
    }

    void updateBaro(float altitude)
    {
        //predict(); //doesn't necessarily need to be done right before estimate update with measurement
        Z_Baro = {altitude};
        K_Baro = P * ~H_Baro * Inverse(H_Baro * P * ~H_Baro + R_Baro); //*Compute the Kalman Gain

        X = X + K_Baro * (Z_Baro - H_Baro * X);                                               //*Update estimate with measurement
        P = (I - K_Baro * H_Baro) * P * (~(I - K_Baro * H_Baro)) + K_Baro * R_Baro * ~K_Baro; //*Update the estimate uncertainty
    }

    //functions for retrieving data
    float getKalmanPosition()
    {
        return X(0, 0);
    }

    float getKalmanVelocity()
    {
        return X(1, 0);
    }

    float getKalmanAcceleration()
    {
        return X(2, 0);
    }

    float getPositionUncertainty()
    {
        return P(0, 0);
    }

    float getPositionKalmanGain()
    {
        return K_Baro(0, 0);
    }

    //functions for setting data
    void setR(float var)
    {
        R_Baro(0, 0) = var;
    }

    void setQ(float q_p, float q_v, float q_a)
    {
        Q(0, 0) = q_p;
        Q(1, 1) = q_v;
        Q(2, 2) = q_a;
    }


    void printKalmanState()
    {
        Serial.println("P: " + String(getKalmanPosition()) + " V: " + String(getKalmanVelocity())+ " A: " + String(getKalmanAcceleration()));
    }

    void printPostionUncertainty()
    {
        Serial.println("P (uncertainty): " + String(getPositionUncertainty()));
    }

    void printFullInfoPosition(double baro_alt)
    {
        Serial.print("Z: " + String(baro_alt, 2) + "    X: " + String(kalman::getKalmanPosition(), 2));
	    Serial.print("    K_Gain: " + String(kalman::getPositionKalmanGain(), 4));
        Serial.println("    P: " + String(getPositionUncertainty(), 4));
    }

}
