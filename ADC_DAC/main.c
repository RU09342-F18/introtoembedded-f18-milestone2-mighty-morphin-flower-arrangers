#include <msp430.h>

float slope = -0.002794;
float intercept = 54.8924;
float temp;

char space = ' ';
char lowBit, highBit;

float volt, ADC_Voltage, temp;

int main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer

  P6SEL |= 0x01;                            // Enable A/D channel A0

  ADC12CTL0 = ADC12ON+ADC12MSC+ADC12SHT0_8; // Turn on ADC12, extend sampling time to avoid overflow of results
  ADC12CTL1 = ADC12SHP+ADC12CONSEQ_3+ADC12DIV_7 ;       // Use sampling timer, repeated sequence
  ADC12MCTL0 = ADC12INCH_0;                 // ref+=AVcc, channel = A0
  ADC12IE = 0x01;                           // Enable ADC12IFG.1
  ADC12CTL0 |= ADC12ENC;                    // Enable conversions
  ADC12CTL0 |= ADC12SC;                     // Start convn - software trigger

  P4SEL |= BIT4;              // UART TX
  P4SEL |= BIT5;              // UART RX

  UCA1CTL1 |= UCSWRST;        // Clears the UART control register 1
  UCA1CTL1 |= UCSSEL_2;       // Sets SMCLK
  UCA1BR0 = 104;              // For baud rate of 9600
  UCA1BR1 = 0;                // For baud rate of 9600

  UCA1MCTL = UCBRS_2;              // set modulation pattern to high on bit 1 & 5
  UCA1CTL1 &= ~UCSWRST;            // initialize USCI
  UCA1IE |= UCRXIE;                // enable USCI_A1 RX interrupt

  __bis_SR_register(LPM0_bits + GIE);       // Enter LPM4, Enable interrupts
  __no_operation();

}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
    lowBit = ADC12MEM0;
    highBit = ADC12MEM0 >> 8;
    volt = ADC12MEM0;
    ADC_Voltage = (volt/4095) * 3.3;
    temp = -((330000-319190*(ADC_Voltage))/(4147*(ADC_Voltage)));
    UCA1TXBUF = temp;
    __delay_cycles(1000000);
}
