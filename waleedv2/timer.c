#include"timer.h"

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
}