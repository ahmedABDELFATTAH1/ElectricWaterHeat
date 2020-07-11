#define _XTAL_FREQ 8000000
#include <xc.h>
void itoa(unsigned int val, unsigned char* str )
{ 
  str[0]=((val%1000)/100);  
  str[1]=((val%100)/10);
  if (str[1] != 0)
      str[1]-=1;  
}
