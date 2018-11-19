#include <msp430.h>
/*
 * DutyCycleSetup.c
 *
 *  Created on: Oct 2, 2018
 *      Author: ianni
 */

void DutyCycleSetup(void){

    TA0CCR0 = 1000;                             // PWM Period
    TA0CCTL1 = OUTMOD_7;                        // CCR1 reset/set
    TA0CCR1 = 0;                                // CCR1 PWM duty cycle
    TA0CTL = TASSEL_2 + MC_1 + ID_1;            // SMCLK, up mode
}


