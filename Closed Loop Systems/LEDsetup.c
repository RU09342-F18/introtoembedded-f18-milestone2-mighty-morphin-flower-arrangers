#include <msp430.h>

#define LED0 BIT0
#define LED1 BIT7
#define LED_OUT P1OUT
#define LED_OUT4 P4OUT
#define LED_DIR P1DIR
#define PULL_UP P1REN
#define LED_SEL P1SEL
#define LED_SEL2 P1SEL2
#define BUTTON BIT1
#define BTN_INT_EN P1IE
#define BTN_INT_EDGE P1IES
#define CLRFLG P1IFG

void LEDSetup(void)
{
    //Set P1.0 and P4.7 to the output direction.
    P4DIR |= BIT0;
    P4DIR |= BIT7;
}
