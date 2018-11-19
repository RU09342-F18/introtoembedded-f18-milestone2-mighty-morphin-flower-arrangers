#include <msp430.h>


void PIDControl(float temp, float desiredTemp)
{
    /*float Kp = 100;
    float Ki = 60;
    float Kd = 60;
    float Up, Ui, Ud, SatErr, Err, OutPreSat, Out, Up1;
    float OutMax = 1000;
    float OutMin = 0;
    //float temp, desiredTemp;

    Err = temp - desiredTemp; // Compute the error
    Up = Kp * Err; /* Compute Up
    Ui = Ui + (Ki * (Up * 0.001)); // Compute Ui
    Ud = Kd * ((Up - Up1) / 0.001);
    OutPreSat = Up + Ui + Ud;
    if (OutPreSat > OutMax) // Saturate output
    {
        Out = OutMax;
    }
    else if (OutPreSat < OutMin)
    {
        Out = OutMin;
    }
    else
    {
        Out = OutPreSat;
    }
    SatErr = Out - OutPreSat; // Compute saturate difference
    Up1 = Up;*/

}
