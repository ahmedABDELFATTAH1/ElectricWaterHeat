#include <pic16f877a.h>

#include"seven_segment.h"

unsigned char segments_code[]={0xBF,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0xFF,0x6F};
//this function should write number to specific 7segment
void lcd_writeNumber(unsigned char number)
{
    if(number>=0 && number<10 )
    {
        unsigned char code=segments_code[number];
        PORTD=code;
    }    
    else{
        PORTD=0;
    }
}

void displayTempreture(unsigned int TEMPRETURE)
{
    unsigned char low=TEMPRETURE%10;
    unsigned char high=TEMPRETURE/10;
     PORTAbits.RA5=1;   
    lcd_writeNumber(low);     
    __delay_ms(10);
    PORTAbits.RA5=0;
    lcd_writeNumber(high);
    PORTAbits.RA4=1;    
    __delay_ms(10);
    PORTAbits.RA4=0;
}

void displayDeiredTempreture(int TargetTempreture)
{
    unsigned char low=TargetTempreture%10;
    unsigned char high=TargetTempreture/10;
     PORTAbits.RA5=1;   
    lcd_writeNumber(low);     
    __delay_ms(10);
    PORTAbits.RA5=0;
    lcd_writeNumber(high);
    PORTAbits.RA4=1;    
    __delay_ms(10);
    PORTAbits.RA4=0;
}