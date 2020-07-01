/*
 * File:   main.c
 * Author: ahmed
 *
 * Created on June 30, 2020, 6:05 PM
 */


#include <xc.h>
#include <pic16f877a.h>
#include"config.h"
#include"seven_segment.h"
#define _XTAL_FREQ 8000000

void displayTempreture(unsigned short temp)
{
    unsigned char low=temp%10;
    unsigned char high=temp/10;
     PORTAbits.RA5=1;   
    lcd_writeNumber(low);     
    __delay_ms(10);
    PORTAbits.RA5=0;
   lcd_writeNumber(high);
   PORTAbits.RA4=1;    
   __delay_ms(10);
    PORTAbits.RA4=0;
}
void initDisplay()
{
 TRISD=0x00;//which means output bin    
 TRISB=0x00;//which means output bin
 TRISAbits.TRISA4=0;//which means output bin
 TRISAbits.TRISA5=0;//which means output bin  
   
}


void main(void) {
    initDisplay();
    unsigned int count=85;
    while(1)
    {
        displayTempreture(count);   
        count++;
        if(count==99)
            count=0;        
        __delay_ms(10);
    }
    
        
    
}
