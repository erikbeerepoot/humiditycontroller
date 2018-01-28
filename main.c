/*
 * File:   main.c
 * Author: erik
 *
 * Created on July 18, 2017, 9:02 PM
 */

// PIC24HJ32GP202 Configuration Bit Settings

// 'C' source line config statements

// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = FRC              // Oscillator Mode (Internal Fast RC (FRC))
#pragma config IESO = ON                // Internal External Switch Over Mode (Start-up device with FRC, then automatically switch to user-selected oscillator source when ready)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Source (Primary Oscillator Disabled)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow Multiple Re-configurations)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are disabled)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler (1:32,768)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // POR Timer Value (128ms)
#pragma config ALTI2C = ON             // Alternate I2C  pins (I2C mapped to SDA1/SCL1 pins)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define FOSC    (7370000ULL)
#define FCY     (FOSC/2)

#define DELAY_LENGTH 5

#include <libpic30.h>
#include <stdio.h>
#include <math.h>

#include "xc.h"

#include "pwm.h"
#include "uart.h"
#include "sht1x.h"
#include "data_logger.h"
#include "serial_interface.h"
#include "humidity_controller.h"
#include "fogger.h"

int main(void) {       
    configure_serial_interface();
    configure_uart();
    configure_pwm();    
    configure_sht();
   
    configure_fogger();
    __delay_ms(10);
                
    stop_fogger();
        
    //briefly blow fan                 
    set_pwm_setpoint(4000);
    start_pwm();
    __delay_ms(1000);
    stop_pwm();
    
        
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    // =-=-=-=-= Run loop at ~ 1 Hz =-=-=-=-= 
    // =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
    int period = 1000;
   
    transmit_string("reset... \n\r");    
      
    //Run fan continously
    start_pwm();
    while(1){                          
        float humidity = sht_read_humidity();        
        float temp = sht_read_temp();                    
        if(humidity < 1.0 || temp < 1.0){
            sht_soft_reset();
            continue;
        }         
        log_entry(humidity,temp);         

        int run_time = run_loop();
             
        //run fogger
        start_fogger();          
        __delay_ms(run_time);
        stop_fogger();        
  
        int count = (period - run_time)/DELAY_LENGTH;
        int index;
        for(index=0;index<count;index++){
            __delay_ms(DELAY_LENGTH);
            serial_run_loop();
        }                
    }     
    
    return 0;
}
