#include <msp430.h>

void TimerA1Setup(){
    TA1CCTL0 = CCIE; // CCR0 interrupt enabled
    TA1CCR0 = 999; //resets timer at 10000 cycles
}
