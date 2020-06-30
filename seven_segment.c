#include <pic16f877a.h>

#include"seven_segment.h"

unsigned char segments_code[]={0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xF6};
//this function should write number to specific 7segment
void writeNumber(unsigned char number,unsigned char lcdNumber)
{
    if(number>0 && number<10 )
    {
        unsigned char code=segments_code[number];
        PORTD=code;
    }    
}
void clearLCD(unsigned char lcdNumber)
{
   PORTD=segments_code[0];
}