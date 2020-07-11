
#include <xc.h>
#include "adc.h"


void adc_init(void)
{
#if defined (_18F452) || defined(_16F877A)
  ADCON1=0x02;
  ADCON0=0x41; 
#else
  ADCON0=0x01;
  ADCON1=0x0B;
  ADCON2=0x01;
#endif


}

unsigned int adc_amostra(unsigned char canal)
{
#if defined(_18F4620) || defined(_18F4550)
    switch(canal)
    {
      case 0: 
        ADCON0=0x01;
        break;
      case 1: 
        ADCON0=0x05;
        break;
      case 2: 
        ADCON0=0x09;
        break;
    }
#else   
     switch(canal)
    {
      case 0:
        ADCON0=0x01;
        break;
      case 1:
        ADCON0=0x09;
        break;
      case 2:
        ADCON0=0x11;
        break;
    }   
#endif
   

    ADCON0bits.GO=1;
    while(ADCON0bits.GO == 1);

   return ((((unsigned int)ADRESH)<<2)|(ADRESL>>6));
}
