#include <stdio.h>
#include "msp432p401r.h"
#include "SSEG.h"

uint8_t index = 0;

void test1(){
    int i;
    for (i = 0; i < 16; i++){
        SSEG_Out(i);
        SysTick_Wait10ms(50);
    }
}
void test2(){
    while(1){
        WaitForInterrupt();
        SSEG_Out(index);
    }
}
void test3(){
    SSEG_Shift1();  // 1
    SSEG_Shift1();  // 11
    SSEG_Shift0();  // 110
    SSEG_Shift0();  // 1100
    shiftOut();
    int i;
    for (i = 0; i < 8; i++){
        SSEG_Shift0();
    }
    shiftOut();
    SSEG_Shift_Out(1);
    shiftOut();
    SysTick_Wait10ms(50);
    SSEG_Shift_Out(8);
    shiftOut();
    SysTick_Wait10ms(50);
    SSEG_Shift_Out(7);
    shiftOut();
    SysTick_Wait10ms(50);
//    for(i = 0; i < 10000; i++)
//        SSEG_Disp_Num(i);
    for(i = 0; i < 3000; i++)
        SSEG_Disp_Num(1234);
}

void main() {
    SSEG_Init();
    //Port5_Init();
    SysTick_Init();
    //SSEG_Out(0);
    //SysTick_Wait10ms(50);
    //SSEG_Out(1);
    //test1();
    //test2();
    P4OUT |= 0x01;
    //SSEG_Shift_Out('a');
    int in = 0;
    while (1){
        printf("Enter a value 0-9999: ");
        scanf("%d",&in);
        printf("\n");
        if (in == -1)
            break;
        else if((in >= 0)&&(in < 10000)){
            int i;
            for(i = 0; i < 200; i++)
                SSEG_Disp_Num(in);
        }
        else
            printf("Invalid input\n");
    }
    SSEG_Off();

    test3();
}
