#define FOSC    (7370000ULL)
#define FCY     (FOSC/2)

#include <libpic30.h>
#include <stdio.h>

#include "xc.h"

#include "fogger.h"

void configure_fogger(){
    I2C1CON = 0x0000;
    TRISBbits.TRISB9 = 0;
    TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB12 = 0;
}

void start_fogger(){
    LATBbits.LATB9 = 1;
    LATBbits.LATB10 = 1;
    LATBbits.LATB12 = 1;
}
void stop_fogger(){    
    LATBbits.LATB9 = 0;
    LATBbits.LATB10 = 0;
    LATBbits.LATB12 = 0;
}