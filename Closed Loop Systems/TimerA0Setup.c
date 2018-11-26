#include <msp430.h>

void TimerA0Setup(){
    TA0CCTL1 = CCIE;                    // Enable interrupt in compare mode
    TA0CCTL0 = CCIE;                    // Enable interrupt in compare mode
    TA0CCR0 = 999;                     // Set period of CCR0 1000 microseconds.
    TA0CCR1 = 100;                        // Set duty cycle to 50%
    TA0CTL = TACLR;
    TA0CTL = TASSEL_1 + MC_1;           // SMCLK, UP
}
