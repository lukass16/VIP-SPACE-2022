//source: https://www.youtube.com/watch?v=ruB917YmtgE 
double KALMAN(double U);
double KALMAN(double U){

    //static variables are only inicialized once and that helps to save memory
    static const double R  =40;     //noise covariance
    static const double H = 1.00;   //measurement map
    static double Q = 10;           //initial estimated covariance
    static double P = 0;            //initial error covariance - must be 0
    static double U_hat = 0;        //initial estimated state
    static double K = 0;            //Kalman gain

    K = P*H/(H*P*H+R);
    U_hat = U_hat +K*(U-H*U_hat);
    P=(1-K*H)*P+Q;

    return U_hat; //returning the estimated error


}