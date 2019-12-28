//***************************************************************************************
//  MSP430 Binary Counter
//
//  Description; count some var from 0-255 based on key switch inputs.
//  S1 decrements the counter, S2 increments the counter.
//  Counter will roll over when count would otherwise underflow/overflow.
//  Output pins on launchpad from P3.0 and P3.7
//  Input switches are the hardware switches P5.5 and P5.6
//
//  To prevent over-current, place 1k Ohm between the display LEDs and ground.
//
//  Kensho Security Labs
//  December 2019
//***************************************************************************************


#include <msp430.h> 

unsigned int count = 0x00;                  // The display physically presents a byte, so perfect.

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW + WDTHOLD;	            // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5;                   // Disable the GPIO power-on default high-impedance mode

    P3DIR |= 0xFF;                          // We want all eight pins of this port to be outbound and drive display
    P3OUT &= 0x00;

    P5DIR = 0x00;                            // Not that it matters, but we need port 5 to be inbound.
    P5IE |= (BIT5 + BIT6);                   // The hardware switches on the launchpad are our interupts.
    P5REN |= (BIT5 + BIT6);                  // Use internal hardware pulldown resistors.
    P5IES |= (BIT5 + BIT6);                  // Hi/Lo Edge for select
    P5IFG |= ~BIT5;
    P5IFG |= ~BIT6;
    _BIS_SR(CPUOFF + GIE);                   // Enter LPM0 w/ interrupt
    //__enable_interrupt();

    while(1){}                              //Loop Forever, Alles Anderes is Interrupts!

}

#pragma vector=PORT5_VECTOR
__interrupt void Port_5(void)               // A sane man would modify this to add debounce
{

    unsigned char flags = P5IFG & P5IE;

    if (flags & BIT6) {                     // If S1, Decrement
        P5IFG &= ~BIT6;
        if (count == 0x00){                 // Look out for overflow
            count = 0xFF;
        }
        else{
            count = count - 0x01;
        }
   }
    if (flags & BIT5) {                     // Must have been S2, so increment.
        P5IFG &= ~BIT5;
        if (count == 0xFF){                 // More Overflow Protection
            count = 0x00;
        }
        else{
          count = count + 0x01;
        }
      }
   P3OUT = count;                           // Display magic is in the copper



}
