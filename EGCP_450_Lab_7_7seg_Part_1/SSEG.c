#include <stdint.h>
#include "SysTick.h"
#include "msp432p401r.h"
#include "SSEG.h"

// Global Variables
char out_num;
extern uint8_t index;

void DisableInterrupts();			// Disable interrupts
void EnableInterrupts();			// Enable interrupts
long StartCritical ();				// previous I bit, disable interrupts
void EndCritical(long sr);			// restore I bit to previous value
void WaitForInterrupt();			// low power mode

/*
 * SSEG_Init Function
 * Initialize 7-segment display
 * Inputs: none
 * Outputs: none
 */
void SSEG_Init1() {
    // Cathode list                                                A
    // P4.0 = E,    P4.1 = D,   P4.2 = DP,    P4.3 = C,          _____
    // P4.4 = G,    P4.5 = B,   P4.6 = F,     P4.7 = A          |     |
    //                                                        F |     |B
    //                                                          |_____|
    //                                                          |  G  |
    //                                                        E |     |C
    //                                                          |_____|
    //                                                             D   []
    //                                                                 DP
	P4SEL0 &= ~0xFF;
	P4SEL1 &= ~0xFF;
	P4DIR |= 0xFF;
	P4OUT &= ~0xFF;

}

void SSEG_Init(){
    P4SEL0 &= ~0x0F;
    P4SEL1 &= ~0x0F;
    P4DIR |= 0x0F;
    P4OUT &= ~0x0F;

    P5SEL0 &= ~0x07;        // P5.0 = SRCLK
    P5SEL1 &= ~0x07;        // P5.1 = RCLK
    P5DIR |= 0x07;          // P5.2 = SER
    P5OUT &= ~0x07;
}

/*
 * SSEG_Out Function
 * Output a number to a single digit of the 7-segment display
 * Inputs: a number between 0 and 15
 * Outputs: none
 */
void SSEG_Out(uint8_t num) {
    switch(num){
    case 0:
        P4OUT = 0x14;
        break;
    case 1:
        P4OUT = ~0x28;
        break;
    case 2:
        P4OUT = 0x4C;
        break;
    case 3:
        P4OUT = 0x45;
        break;
    case 4:
        P4OUT = 0x87;
        break;
    case 5:
        P4OUT = 0x25;
        break;
    case 6:
        P4OUT = 0x24;
        break;
    case 7:
        P4OUT = ~0xA8;
        break;
    case 8:
        P4OUT = 0x04;
        break;
    case 9:
        P4OUT = 0x07;
        break;
    case 10:
        P4OUT = 0x06;
        break;
    case 11:
        P4OUT = 0xA4;
        break;
    case 12:
        P4OUT = 0x3C;
        break;
    case 13:
        P4OUT = 0xC4;
        break;
    case 14:
        P4OUT = 0x2C;
        break;
    case 15:
        P4OUT = 0x2E;
        break;
    default:
        P4OUT = 0x00;
    }
}

/*
 * SSEG_Shift_Out Function
 * Shifts data out serially
 * Inputs: 8-bit data
 * Outputs: none
 */
void SSEG_Shift_Out(char data){
    int i;
    switch (data){
    case 0:     // 0 11000000
        for (i = 0; i < 2; i++)
            SSEG_Shift1();
        for (i = 0; i < 6; i++)
            SSEG_Shift0();
        break;
    case 1:     // 1 11111001
        for (i = 0; i < 5; i++)
            SSEG_Shift1();
        for (i = 0; i < 2; i++)
            SSEG_Shift0();
        SSEG_Shift1();
        break;
    case 2:    // 2 10100100
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift0();
        break;
    case 3:     // 3 10110000
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift1();
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift0();
        break;
    case 4:     // 4 10011001
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift1();
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift1();
        break;
    case 5:     // 5 10010010
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift1();
        SSEG_Shift0();
        break;
    case 6:    // 6 10000010
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift1();
        SSEG_Shift0();
        break;
    case 7:     // 7 11111000
        for (i = 0; i < 5; i++)
            SSEG_Shift1();
        for (i = 0; i < 3; i++)
            SSEG_Shift0();
        break;
    case 8:     // 8 10000000
        SSEG_Shift1();
        for (i = 0; i < 7; i++)
            SSEG_Shift0();
        break;
    case 9:     // 9 10011000
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift1();
        SSEG_Shift1();
        SSEG_Shift0();
        SSEG_Shift0();
        SSEG_Shift0();
        break;
    default:    // 11111111
        for (i = 0; i < 8; i++)
            SSEG_Shift1();
    }
}
/*
 * SSEG_Disp_Num Function
 * Seperate the input number into 4 single digit
 * Inputs: num between 0 and 9999
 * Outputs: none
 */
void SSEG_Disp_Num(int num){
    //P4OUT &= ~0x0F;
    SSEG_Shift_Out(num/1000);          // get left most digit
    shiftOut();                        // output left most digit
    P4OUT |= 0x01;                     // turn on left most display
    //SysTick_Wait10ms(100);             // wait 1s
    SysTick_Wait(15000);               // wait 5ms
    P4OUT ^= 0x01;                     // turn off left most display
    SSEG_Shift_Out((num/100)%10);      // get second digit
    shiftOut();
    P4OUT ^= 0x02;                     // turn on second display
    //SysTick_Wait10ms(100);             // wait 1s
    SysTick_Wait(15000);               // wait 5ms
    P4OUT ^= 0x02;                     // turn off second display
    SSEG_Shift_Out((num/10)%10);
    shiftOut();
    P4OUT ^= 0x04;                     // turn on third display
    //SysTick_Wait10ms(100);             // wait 1s
    SysTick_Wait(15000);               // wait 5ms
    P4OUT ^= 0x04;                     // turn off third display
    SSEG_Shift_Out(num%10);
    shiftOut();
    P4OUT ^= 0x08;                     // turn on fourth display
    //SysTick_Wait10ms(100);             // wait 1s
    SysTick_Wait(15000);               // wait 5ms
    P4OUT ^= 0x08;                     // turn off fourth display
}
/*
 * SSEG_Off Function
 * Turns off all 7-seg digits
 * Inputs: none
 * Outputs: none
 */
void SSEG_Off(){
    P4OUT &= ~0x0F;
}
void SSEG_Shift1(){
    P5OUT |= 0x04; // data 1
    P5OUT |= 0x01; // srclk 1
    P5OUT ^= 0x05; // data 0 srclk 0
}
void SSEG_Shift0(){
    P5OUT &= ~0x04; // data 0
    P5OUT |= 0x01;  // srclk 1
    P5OUT ^= 0x01;  // srclk 0
}
void shiftOut(){
    P5OUT |= 0x02; // rclk 1
    P5OUT ^= 0x02; // rclk 0
}

