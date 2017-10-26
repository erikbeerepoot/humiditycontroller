/*
 * File:   pwm.c
 * Author: E. E. Beerepoot
 *
 * Created on July 20, 2017, 6:45 PM
 */

#include "xc.h"
#include "helper.h"

volatile int pwm_setpoint = 0;
int persistent_setpoint = 0;

void __attribute__((__interrupt__,no_auto_psv)) _T2Interrupt( void )
{
    OC1RS = pwm_setpoint;  
    IFS0bits.T2IF = 0; 
}

//configure the PWM peripheral 
void configure_pwm(){    
    //configure timer 2    
    T2CONbits.TCKPS = 0b00; //1:1
    
    unlock_pps();
        
    //pwm with no fault protection
    OC1CONbits.OCM = 0b110;
    
    //configure OC1 on RP8
    RPOR4bits.RP8R = 0b10010;
        
    lock_pps();
    
    OC1R = 0x000f;
    OC1RS = 0xffff;
    
    IPC1bits.T2IP = 0x01; // Set Timer 2 Interrupt Priority Level
    IFS0bits.T2IF = 0;    // Clear Timer 2 Interrupt Flag
    IEC0bits.T2IE = 1;    // Enable Timer 2 interrupt
    
    T2CONbits.TON = 1;    
}

void start_pwm(){
    pwm_setpoint = persistent_setpoint;
}

void stop_pwm(){
    pwm_setpoint = 0;    
}

void set_pwm_setpoint(int setpoint){
    pwm_setpoint = setpoint;
    persistent_setpoint = setpoint;
}

int get_pwm_setpoint(){
    return persistent_setpoint;
}