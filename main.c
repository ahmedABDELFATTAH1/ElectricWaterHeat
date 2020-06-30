/*
 * File:   main.c
 * Author: ahmed
 *
 * Created on June 30, 2020, 6:05 PM
 */


#include <xc.h>
#include"config.h"
#include"seven_segment.h"
#define _XTAL_FREQ 8000000
void main(void) {
    TRISD=0x00;//which means output bin    
    unsigned int count=0;
    while(1)
    {
        writeNumber(count,0);
        __delay_ms(500);
        count++;
        if(count==10)
            count=0;        
    }
    
        
    
}
