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
#include "seven_segment.h"
#include "ext_eeprom.h"
#include "7-Segment.h"
#define _XTAL_FREQ 8000000
#define NUMBER_READINGS 10
#define true 1
#define false 0
#define MAX_TEMP 75
#define MIN_TEMP 35
#define UP PORTBbits.RB3
#define DOWN PORTBbits.RB4
#define FLAG_ADDRESS 0x01
#define TEMP_ADDRESS 0x00
typedef  unsigned char bool;

// global flag to control the program flow
int TargetTempreture = 60;
unsigned int TEMPRETURE;
unsigned int accumilated_tmeperature;
unsigned char str[6];
enum States{ON_STATE,OFF_STATE,SETTING_STATE};
enum Temp_Status{OK_state,UPOVE_state,DOWN_state};
unsigned char state;
unsigned char temp_state;
unsigned char Number_Tempreture;
unsigned char timer1counter;
unsigned char canuse= 1;
bool ReadTemp;
void IRQ_RB0_init ();
void TIMER1_init ();
void displayTempreture();
void update_tempreture();
void displayDeiredTempreture();
void flashDisplay();
void off_state()
{
    RC2=0;
    RC5=0;
}

enum Prev_Button_Status{PREV_UN_STABLE,PREV_STABLE};
void flashDisplay()
{
    static unsigned char count=0;
    static unsigned char prev_status=PREV_STABLE;
    if(UP&&DOWN)
    {
        if(count==5)
        {
            state=ON_STATE;           
            count=0;
//            EEPROM_Write(TEMP_ADDRESS,TargetTempreture);
            return;
        }
        else{
            prev_status=PREV_STABLE;
            count++;
        }

    }
   for(int i=0;i<25;i++)
   {
    displayDeiredTempreture();
    if(UP==0&&prev_status==PREV_STABLE)
       {
           if(TargetTempreture<MAX_TEMP)
           {
               TargetTempreture += 5;
           __delay_ms(10);
           prev_status=PREV_UN_STABLE;
           }
       } 
         
    if(DOWN==0&&prev_status==PREV_STABLE)
    {
        if(TargetTempreture>MIN_TEMP)
        {
         TargetTempreture -= 5;
        __delay_ms(10);
         prev_status=PREV_UN_STABLE;
        }
       
    }
     

   }    
    PORTAbits.RA4=0;
    PORTAbits.RA5=0;   

    
      for(int i=0;i<25;i++)
   {
       __delay_ms(20);       
       if(UP==0&&prev_status==PREV_STABLE)
       {
           if(TargetTempreture<MAX_TEMP)
           {
               TargetTempreture += 5;
           __delay_ms(10);
           prev_status=PREV_UN_STABLE;
           }
       }
         
         
     if(DOWN==0&&prev_status==PREV_STABLE)
     {
        if(TargetTempreture>MIN_TEMP)
        {
         TargetTempreture -= 5;
        __delay_ms(10);
         prev_status=PREV_UN_STABLE;
        }
     }
      
   }

}
void changeStatus()
{
    if(TEMPRETURE<TargetTempreture+5 && canuse == 1)
    {
        temp_state=UPOVE_state;

    }else{
        canuse= 0;
        if(TEMPRETURE>TargetTempreture && canuse == 0)
        {
            temp_state=DOWN_state; 
        }
        else{
            canuse= 1;
            temp_state=OK_state;
            RC2=0;
            RC5=0;
        }
    }
}


void heaterOn()
{
    RC2=0;
    RC5=1;
}

void coolerOn()
{
    RC5 = 0;
    RC2=1;

}


void on_state()
{  
    TMR1ON = 1; 
    if(UP==0||DOWN==0)
    {
        state=SETTING_STATE;      
        return;
    }
    displayTempreture();
    changeStatus();
   
    switch (temp_state)
    {
    case OK_state:
        break;
    case UPOVE_state:       
        heaterOn();
        break;    
    case DOWN_state:     
        coolerOn();
        break;
    default:
        break;
    }

}

void setting_state()
{
     TMR1ON = 0; 
     RC2=0;
     RC5=0;
     flashDisplay();

}

void main(void) {
    
    TIMER1_init ();
    IRQ_RB0_init ();
    I2C_Master_Init(100000);
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
    RC5 = 0;
    TRISB7 = 0;
    TRISB3 = 1;
    TRISB4 = 1;
    RB7 =0;
    TRISB5 = 0;
    RB5 =0;
    Number_Tempreture=0;
    adc_init();
    state=OFF_STATE;
    temp_state=OK_state;
    ReadTemp=false;
    timer1counter=0;
    accumilated_tmeperature=0;
    TargetTempreture = 60;
//  unsigned char flag=EEPROM_Read(FLAG_ADDRESS);
//  __delay_ms(500);
//  if(flag==1)
//  {
//      TargetTempreture=EEPROM_Read(TEMP_ADDRESS);  
//  }
//  else
//  {
//    EEPROM_Write(FLAG_ADDRESS,0x01); 
//    TargetTempreture=60;
//  }    
 
while (1)
{  
    switch(state)
    {
        case ON_STATE:            
            on_state();
            break;
        case OFF_STATE:
            off_state();
            break;      
        case SETTING_STATE:
            setting_state();  
            break;
        default:
            break;     
    }
  
}
}
void displayTempreture()
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

void displayDeiredTempreture()
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

void update_tempreture ()
{   
    unsigned int temp=(adc_amostra(2)*10)/2;  
    itoa(temp,str);
    unsigned char y= str[2];
    unsigned char x= str[3];
    accumilated_tmeperature += (y*10)+x;
    return;
}



void __interrupt() ISR(void)
{
    
  if (INTF)   // Check The Flag
  {
    INTF = 0;  //  Clear The Flag
    switch(state)
    {
        case ON_STATE:
            TMR1ON = 0;
            TMR1IF = 0;
            state=OFF_STATE;
            break;
        case OFF_STATE:
            TMR1ON = 1;
            state=ON_STATE;
            break;     
        case SETTING_STATE:
            TMR1ON = 0;
            TMR1IF = 0;
            state=OFF_STATE;
            break;
        default:
            break;     
    }
   
        
    } 
  // Check The Flag Bit

   if (TMR1IF)
   {
           TMR1IF = 0; // Clear The Flag Bit
           timer1counter++;
           ReadTemp=false;
           if (timer1counter == 4){
                ReadTemp=true;               
                Number_Tempreture++;               
                timer1counter = 0;
                update_tempreture();
           }
            if(Number_Tempreture==NUMBER_READINGS)
            {
               TEMPRETURE=accumilated_tmeperature/NUMBER_READINGS;
                // Event2 = Toggle LED
                RB5 = ~RB5;
                // Clear The Global Counter
                Number_Tempreture = 0;
                accumilated_tmeperature= 0;
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

