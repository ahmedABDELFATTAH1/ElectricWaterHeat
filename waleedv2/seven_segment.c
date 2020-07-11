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
