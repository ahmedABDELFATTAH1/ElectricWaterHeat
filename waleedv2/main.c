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
// global flag to control the program flow
int tempinit = 0;
int stayIn = 1;
int flag = 0;
int wantedTemp = 50;
int heater_flag = 0;
int cooler_flag = 0;
unsigned int tmp;
unsigned char str[6];
unsigned int accumilated_tmeperature = 0;
int C = 0;
int timer1counter =0;
void IRQ_RB0_init ();
void TIMER1_init ();
void init_temperature (unsigned int x, unsigned char y[6]);
void temperature_increase (unsigned int x, unsigned char y[6]);
void temperature_decrease (unsigned int x, unsigned char y[6]);
void main(void) {
    
    TIMER1_init ();
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
    TRISC2 = 0;
    RC2 = 0;
    TRISB7 = 0;
    RB7 =0;
    TRISB5 = 0;
    RB5 =0;
    adc_init();
    while(1){
        if(flag)
        {
            if (tempinit == 0)
            {
                init_temperature (tmp, str);
            }else{
                if (heater_flag == 1)
                {
                    temperature_increase (tmp, str);
                }
                if (cooler_flag == 1)
                {
                    temperature_decrease (tmp, str);
                }
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
        }else
        {
            RA5 = 0;
            RA4 = 0;
        }
    }
    return;
}

void init_temperature (unsigned int x, unsigned char y[6])
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
    if (res >= wantedTemp)
    {
        tempinit = 1;
    }
    RA5 = 0;
    RA4 = 0;
    RC5 = 0;
    return;
}


void temperature_increase (unsigned int x, unsigned char y[6])
{
    int res = 0;
    while (res <= wantedTemp+10){
        if (flag == 1){
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
        }else
        {
            RA5 = 0;
            RA4 = 0;
            RC5 = 0;
        }
    }
    heater_flag = 0;
    RA5 = 0;
    RA4 = 0;
    RC5 = 0;
    return;
}

void temperature_decrease (unsigned int x, unsigned char y[6])
{
    int res = 80;
    while (res != wantedTemp){
        if (flag == 1){
            RC2 = 1;
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
        }else
        {
            RA5 = 0;
            RA4 = 0;
            RC2 = 0;
        }
    }
    cooler_flag = 0;
    RA5 = 0;
    RA4 = 0;
    RC2 = 0;
    return;
}


void __interrupt() ISR(void)
{
  if (INTF)   // Check The Flag
  {
    INTF = 0;  //  Clear The Flag
    if (flag)
    {
        flag = 0;
        // Switch ON Timer1 Module!
        TMR1ON = 0;
        TMR1IF = 0;
    }else{
        flag = 1;
        // Switch ON Timer1 Module!
        TMR1ON = 1;
    } 
  }
  
  // Check The Flag Bit
   if (TMR1IF)
   {
       if (flag && tempinit == 1)
       {
           timer1counter++;
           if (timer1counter == 4){
                C++;
                tmp=(adc_amostra(2)*10)/2;
                itoa(tmp,str);
                unsigned char y= str[2];
                unsigned char x= str[3];
                accumilated_tmeperature +=(y*10)+x;
                timer1counter = 0;
           }
            if(C==30)
            {
                if ((accumilated_tmeperature/30) < wantedTemp)
                {
                    heater_flag = 1;
                }
                else if ((accumilated_tmeperature/30) > wantedTemp)
                {
                    cooler_flag = 1;
                }
                // Event2 = Toggle LED
                RB5 = ~RB5;
                // Clear The Global Counter
                C = 0;
                accumilated_tmeperature= 0;
            }
            TMR1IF = 0; // Clear The Flag Bit
       }else{
            TMR1IF = 0; // Clear The Flag Bit
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

void TIMER1_init (){
    // -- [[ Configure Timer1 To Operate In Timer Mode  ]] --
    // Clear The Timer1 Register. To start counting from 0
    TMR1 = 15535;
    // Choose the local clock source (timer mode)
    TMR1CS = 0;
    // Choose the desired prescaler ratio (1:1)
    T1CKPS0 = 0;
    T1CKPS1 = 0;
    
    // -- [[ Interrupts Configurations ]] --
    TMR1IE = 1; // Timer1 Interrupt Enable Bit
    TMR1IF = 0; // Clear The Interrupt Flag Bit
    PEIE = 1;   // Peripherals Interrupts Enable Bit
//    GIE = 1;    // Global Interrupts Enable Bit
}