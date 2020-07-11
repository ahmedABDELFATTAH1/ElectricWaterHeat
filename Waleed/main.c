/*
 * File:   main.c
 * Author: Ahmed Waleed Ahmed
 *
 * Created on July 7, 2020, 7:33 AM
 */

#include <xc.h>
#include "config.h"
#include "adc.h"
#include "itoa.h"
#include "lcd.h"
#include"7-Segment.h"
#define _XTAL_FREQ 8000000
enum States{
    OFF_STATE,ON_STATE,SETTING_STATE
};
// global flag to control the program flow
unsigned char state;
int tempinit = 0;
int stayIn = 1;
int flag = 0;
int wantedTemp = 60;
unsigned int tmp;
unsigned char str[6];
void IRQ_RB0_init ();
void init_temperature (unsigned int tmp, unsigned char str[6]);

void off_state()
{



}
void main(void) {    
    
    IRQ_RB0_init ();
    // here we set the register a to be able to use him with the ADC 
    // to read the temperature and also control the seven segments 
    TRISA = 0x07;
    // make all port D output
    TRISD = 0x00;
    // out value 0 on all bits
    PORTD = 0x00;
    // to make the pin that control the heater as output
    TRISC5 = 0;
    TRISB7 = 0;
    RB7 =0;
    TRISB5 = 0;
    RB5 =0;
    adc_init();
    state=OFF_STATE;
    off_state();
    while(flag)
    {
        if (!tempinit)
        {
            init_temperature (tmp, str);
        }else{
            tmp=(adc_amostra(2)*10)/2;
            itoa(tmp,str);
            RA5 = 0;
            RA4 = 1;
            unsigned char y= str[2];
            PORTD=display7s(y);
            __delay_ms(30);
            RA4 = 0;
            RA5 = 1;
            unsigned char x= str[3];
            PORTD=display7s(x);
            __delay_ms(30);
        }
    }
    return;
}

void init_temperature (unsigned int tmp, unsigned char str[6])
{
    int res = 0;
    while (flag && res <= wantedTemp){
        RC5 = 1;
        tmp=(adc_amostra(2)*10)/2;
        itoa(tmp,str);
        RA5 = 0;
        RA4 = 1;
        unsigned char y= str[2];
        PORTD=display7s(y);
        __delay_ms(30);
        RA4 = 0;
        RA5 = 1;
        unsigned char x= str[3];
        PORTD=display7s(x);
        __delay_ms(30); 
        res = (y*10)+x;    
    }
    tempinit = 1;
    RC5 = 0;
    return;
}
void __interrupt() ISR(void)
{
  if (INTF)   // Check The Flag
  {
    INTF = 0;  //  Clear The Flag
    if(state==OFF_STATE)
    {
        state=ON_STATE;
    }
    else if(state==ON_STATE)
    {
        state=OFF_STATE;
        off_state();
    }
    else
    {
        state=OFF_STATE;
        off_state();

    }
    
  }
}

void IRQ_RB0_init (){
    // enable the interrupt by clicking on RB0
    INTE = 1; 
    // we will fire the interrupt on the falling edge
    INTEDG = 0;
    // enable the global interrupt 
    GIE = 1;
}