/*Authors: Ian Nielsen and Eric Schroeder
Written November 19th, 2018
Last updated November 28th 2018*/

#include <msp430.h>

#define FAN0 BIT0
#define LED1 BIT7
#define FAN_OUT P4OUT
#define FAN_OUT4 P4OUT
#define PULL_UP P1REN
#define BUTTON BIT1
#define BTN_INT_EN P1IE
#define BTN_INT_EDGE P1IES
#define CLRFLG P1IFG

void DutyCycleSetup();
void TimerA0Setup();
void TimerA1Setup();
void LEDSetup();
void ButtonSetup();

float slope = -0.002794;
float intercept = 54.8924;
float temp = 20;

char space = ' ';
char lowBit, highBit;

volatile float volt = 2048;
float ADC_Voltage;

float Kp = 300;                             // Proportional Term
float Ki = 6;                              // Integral Term
float Kd = 6;                              // Derivative Term
float Up, Ud, SatErr, Err, OutPreSat, Up1;
float Ui = 0;
float Out = 500;
float Out1 = 10;
float OutMax = 998;                         // Max Duty Cycle Possible
float OutMin = 0;                           // Lowest Duty Cycle Possible
float desiredTemp = 30;                     // Desired temperature
int t = 0;
int t1 = 0;
int dt = 0;

int main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer

  TimerA0Setup();                           // Setup TimerA0
  LEDSetup();                               // Setup Fan Output Pin and LED P4.7*
                                            // *LED P4.7 has the same duty cycle as fan for testing purposes

  P6SEL |= 0x01;                            // Enable A/D channel A0

  // ADC12 Setup (Code from TI Resources)
  ADC12CTL0 = ADC12ON+ADC12MSC+ADC12SHT0_8; // Turn on ADC12, extend sampling time to avoid overflow of results
  ADC12CTL1 = ADC12SHP+ADC12CONSEQ_3+ADC12DIV_7;       // Use sampling timer, repeated sequence
  ADC12MCTL0 = ADC12INCH_0;                 // ref+=AVcc, channel = A0
  ADC12IE = 0x01;                           // Enable ADC12IFG.1
  ADC12CTL0 |= ADC12ENC;                    // Enable conversions
  ADC12CTL0 |= ADC12SC;                     // Start convn - software trigger
  
  //UART Setup and UART Interrupt Setup
  P4SEL |= BIT4;                            // UART TX
  P4SEL |= BIT5;                            // UART RX

  UCA1CTL1 |= UCSWRST;                      // Clears the UART control register 1
  UCA1CTL1 |= UCSSEL_2;                     // Sets SMCLK
  UCA1BR0 = 104;                            // For baud rate of 9600
  UCA1BR1 = 0;                              // For baud rate of 9600

  UCA1MCTL |= UCBRS_2;                      // set modulation pattern to high on bit 1 & 5
  UCA1CTL1 &= ~UCSWRST;                     // initialize USCI
  UCA1IE |= UCRXIE;                         // enable USCI_A1 RX interrupt
  UCA1IFG &= ~UCRXIFG;                      // clears interrupt flags

  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM4, Enable interrupts
  __no_operation();

}
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
    // Converting ADC12 input to char for UART output
    lowBit = ADC12MEM0;
    highBit = ADC12MEM0 >> 8;
  
    volt = ADC12MEM0;                       // ADC Voltage information stored for temperature calculations
    UCA1IFG &= ~UCRXIFG;                    // clears interrupt flags

}
#pragma vector=TIMER0_A1_VECTOR             // Disables LED once time has reached duty cycle
__interrupt void Timer0_A1 (void)
{
    // Fan off for duty cycle
    FAN_OUT &= ~FAN0;                       // Fan off
    P4OUT &= ~BIT7;                         // P4.7 Indicator LED off
    TA0CCTL1 &= ~FAN0;                      // clears flag

    ADC_Voltage = (volt/4095) * 3.3;
    if((ADC_Voltage < 1.74))
    {
       temp = -((165000-(171225*ADC_Voltage))/(2849*ADC_Voltage)); // Temperature Range: 15-30
    }
    else if((ADC_Voltage >= 1.74)&&( ADC_Voltage < 2.22))
    {
       temp = -((330000-(262460*ADC_Voltage))/(2729*ADC_Voltage)); // Temperature Range: 30-45
    }
    else if((ADC_Voltage >= 2.22)&&( ADC_Voltage < 2.59))
    {
        temp = -((330000-(205960*ADC_Voltage))/(1383*ADC_Voltage)); // Temperature Range: 45-60
    }
    else if((ADC_Voltage >= 2.59)&&(ADC_Voltage < 2.84))
    {
        temp = -((330000-(169120*ADC_Voltage))/(737*ADC_Voltage)); // Temperature Range: 60-75
    }
    else if((ADC_Voltage >= 2.84))
    {
        temp = -((330000-(145640*ADC_Voltage))/(411*ADC_Voltage)); // Temperature Range: 75-90
    }
    UCA1TXBUF = temp;                       // Send temp info over UART
    Err = temp - desiredTemp;               // Compute the error
    Up1 = Up;                               // Track previous Up and current Up
    Up = Kp * Err;                          // Compute Proportional Control
    Ui += (Ki * (Up * 0.001));              // Compute Integral Control
    Ud = Kd * (((Up - Up1)) / 0.001);       // Compute Derivative Control
    OutPreSat = Up;                         // Output before saturation is taken into account
    if (OutPreSat > OutMax)                 // Ensure that output is within saturation range
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
    SatErr = Out - OutPreSat;               // Compute saturate difference
    TA0CCR1 = Out;                          // Fan PWM changed in accordance to temp
}
#pragma vector=TIMER0_A0_VECTOR             // enables the LED at the end of the cycle
__interrupt void Timer0_A0 (void)
{
    // Fan on for duty cycle
    FAN_OUT |= (FAN0);                      // turns on fan
    P4OUT |= BIT7;                          // P4.7 Indicator LED on
    TA0CCTL0 &= ~FAN0;                      // clears flag
}

#pragma vector = USCI_A1_VECTOR
__interrupt void USCI_A1_ISR(void) {
    desiredTemp = UCA1RXBUF;
}
