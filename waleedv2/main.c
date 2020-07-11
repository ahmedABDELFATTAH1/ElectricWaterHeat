//////////////////////////////////////////////////////// includes ////////////////////////////////////////////////////////////////
#include <xc.h>
#include "config.h"
#include "adc.h"
#include "read_converter.h"
#include "seven_segment.h"
#include "ext_eeprom.h"
#include"timer.h"
#include"heater.h"
#include"cooler.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////// user define /////////////////////////////////////////////////////
// here we define the frequency to be able to use the delay function
#define _XTAL_FREQ 8000000
// number of reads that will be averaged to use the result as indication to 
// run the heater or the cooler
#define NUMBER_READINGS 10
#define true 1
#define false 0
#define MAX_TEMP 75
#define MIN_TEMP 35
#define UP PORTBbits.RB3
#define DOWN PORTBbits.RB4
// the address were we will store a flag as indication to show that we are storing value right now or we will use the default one 
#define FLAG_ADDRESS 0x01
// the address were we will store a wanted temperature
#define TEMP_ADDRESS 0x00
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef  unsigned char bool;

///////////////////////////////////////////////////////////// variables /////////////////////////////////////////////////////////
// set the initial temperature with 60c
int TargetTempreture = 60;
// save the average of the NUMBER_READINGS to determine the if we will make the heater work or the cooler
unsigned int TEMPRETURE;
// save the sum of the readings 
unsigned int accumilated_tmeperature;
// store the values that will  be written on the 7-segment which come from the LM35
unsigned char str[2];
// the program works in 3 states 
// ON => running and increase the temperature until reach to the defualt temperature and try to mantain it
// OFF => make the MCU in sleep mode
// setting => rewrite the default value in the external eeprom
enum States{ON_STATE,OFF_STATE,SETTING_STATE};
// as initial state for the temperature after start running the program 
// the temp will be in ok state no need to modify the temp 
// upOVE means we are in need to rise the temperature
// down means we are in need to decrease the temperature
enum Temp_Status{OK_state,UPOVE_state,DOWN_state};
// here we store the current status from {ON_STATE,OFF_STATE,SETTING_STATE}
unsigned char state;
// here we store the current temperature status from {OK_state,UPOVE_state,DOWN_state}
unsigned char temp_state;
// counter that will determine if that we are reach to the required number of values to be able to average them and also 
// determine if we are going to call the heater or the cooler
unsigned char Number_Tempreture;
// give indication that the number of overflows required to make the timer fire is ready or not
unsigned char timer1counter;
// flag used in update_tempreture function to indicate if this is the right time to switch to the cooler from the heater 
// or to the cooler from the heater 
unsigned char canuse;
//bool ReadTemp;
// indication for the previous state to know if button has been pressed before or not
enum Prev_Button_Status{PREV_UN_STABLE,PREV_STABLE};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////// function declarations ////////////////////////////////////////////
void IRQ_RB0_init ();
void update_tempreture();
void flashDisplay();
void setting_state();
void off_state();
void changeStatus();
void on_state();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    TRISB = 0x19;
    PORTB = 0x00;
    Number_Tempreture=0;
    canuse= 1;
    adc_init();
    state=OFF_STATE;
    temp_state=OK_state;
//    ReadTemp=false;
    timer1counter=0;
    accumilated_tmeperature=0;
    TargetTempreture = 60;
    unsigned char flag=EEPROM_Read(FLAG_ADDRESS);
    __delay_ms(500);
    if(flag==1)
    {
        TargetTempreture=EEPROM_Read(TEMP_ADDRESS);  
    }
    else
    {
      EEPROM_Write(FLAG_ADDRESS,0x01); 
      TargetTempreture=60;
    }    
 
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

void setting_state()
{
     TMR1ON = 0; 
     RC2=0;
     RC5=0;
     flashDisplay();
}

void off_state()
{
    RC2=0;
    RC5=0;
}
void on_state()
{  
    TMR1ON = 1; 
    if(UP==0||DOWN==0)
    {
        state=SETTING_STATE;      
        return;
    }
    displayTempreture(TEMPRETURE);
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
void update_tempreture ()
{   
    unsigned int temp=(adc_amostra(2)*10)/2;  
    convertADCReadings(temp,str);
    unsigned char y= str[0];
    unsigned char x= str[1];
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
//           ReadTemp=false;
        if (timer1counter == 4){
//                ReadTemp=true;               
             Number_Tempreture++;               
             timer1counter = 0;
             update_tempreture();
        }
         if(Number_Tempreture==NUMBER_READINGS)
         {
            TEMPRETURE=accumilated_tmeperature/NUMBER_READINGS;
            if (temp_state == UPOVE_state){
                // Event2 = Toggle LED
                RB5 = ~RB5;
            }else if (temp_state == DOWN_state){
                // Event2 = Toggle LED
                RB5 = 1;
            }else{RB5 = 0;}
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
            EEPROM_Write(TEMP_ADDRESS,TargetTempreture);
            return;
        }
        else{
            prev_status=PREV_STABLE;
            count++;
        }

    }
   for(int i=0;i<25;i++)
   {
    displayDeiredTempreture(TargetTempreture);
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


