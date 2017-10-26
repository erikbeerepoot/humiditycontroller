 /*
 * File:   sht1x.c
 * Author: erik
 *
 * Created on August 12, 2017, 5:31 PM
 */

#define FOSC    (7370000ULL)
#define FCY     (FOSC/2)

#include <libpic30.h>

#include "xc.h"

#include "uart.h"

#include "sht1x.h"

//RP14 -> yellow (clk)
//RP15 -> black (data)

#define CLK_TRIS TRISBbits.TRISB14
#define DATA_TRIS TRISBbits.TRISB13

#define CLK_PIN LATBbits.LATB14
#define DATA_PIN_W LATBbits.LATB13
#define DATA_PIN_R PORTBbits.RB13

#define VDD 3.3

char kReadTempCommand = 0b00000011;
char kReadRHCommand   = 0b00000101;
char kReadStatus      = 0b00000111;
char kWriteStatus     = 0b00000110;
char kSoftReset       = 0b00011110;

void sht_write_byte(char byte);
void sht_start();

void configure_sht(){
    CLK_TRIS = 0;
    DATA_TRIS = 0;    
    
    //FIXME
    AD1PCFGL = 0xffff;    
}

void DATA_read(){
    DATA_TRIS = 1;
}

void DATA_write(){
    DATA_TRIS = 0;
}

void sht_reset(){
    DATA_write();
    
    int i =0;
    for(i=0;i<16;i++){
        CLK_PIN = 1;
        Nop();
        CLK_PIN = 0;
    }
}

void sht_soft_reset(){
    sht_start();
    sht_write_byte(kSoftReset);
}

void sht_start(){
    DATA_write();
    
    DATA_PIN_W = 1;
    CLK_PIN = 1;
    DATA_PIN_W = 0;
    CLK_PIN = 0;
    CLK_PIN = 1;
    DATA_PIN_W = 1;
    CLK_PIN = 0;
}

//Reads a byte from the sensor
unsigned char sht_read_byte(){
    DATA_read();
    
    char result = 0;
    int i = 0;
    
    //Read 8 bits
    for(i=0;i<8;i++){
        CLK_PIN = 1;
        result = (result << 1) + DATA_PIN_R;        
        CLK_PIN = 0;
    }
    
    //Send ACK
    while(DATA_PIN_R==0);
    
    DATA_write();
    DATA_PIN_W = 0;
    CLK_PIN = 1;
    CLK_PIN = 0;
    
    return result;    
}

//Writes a byte to the sensor
void sht_write_byte(char byte){
    DATA_write();
    
    int i;
    for(i = 0; i < 8; i++){
        CLK_PIN = 0;
        DATA_PIN_W = !!(byte & (1 << (7 - i)));
        CLK_PIN = 1;
        Nop();Nop();Nop();
    }
    
    //Acknowledge: FIrst pin goes low after 8th clock
    //Then high after 9th clock
    CLK_PIN = 0;
    DATA_read();
    while(DATA_PIN_R == 1);
    
    CLK_PIN = 1;
    CLK_PIN = 0;
    while(DATA_PIN_R == 0);
}

//Waits until a measurement becomes available
short sht_wait_for_measurement(){
    DATA_read();
    
    if(DATA_PIN_R != 1) return 1;
    
    int i = 0;
    while(DATA_PIN_R == 1){
        if(i++ > 255) return 1;                
        __delay_ms(10);
    }      
    return 0;
}

//Reads the unconverted data from the sensor
unsigned int read_data_raw(char command){
    sht_start();            
    sht_write_byte(command);    
    if(sht_wait_for_measurement() != 0){    
        return -1;
    }
    
    unsigned char high_byte = sht_read_byte();
    unsigned char low_byte = sht_read_byte();
    sht_read_byte(); //ignore for now
          
    unsigned int result = (high_byte * 255) + low_byte;        
    return result;
}

//Returns a decimal temperature value
float sht_read_temp(){
    unsigned int raw_temp = read_data_raw(kReadTempCommand);
    if(raw_temp == -1){
        return 0;
    }
    
    float d1 = -39.65;
    float d2 = 0.01;    
    float t = d1 + d2*raw_temp;
    
    return t;
}

//Returns a decimal humidity value
float sht_read_humidity(){
    unsigned int raw_rh = read_data_raw(kReadRHCommand);
    if(raw_rh == -1){
        return 0;
    }
    
    const double c1 = -2.0468;
    const double c2 = 0.0367;
    const double c3 = -0.0000015955;
    double rh = c1 + c2*raw_rh + c3*raw_rh*raw_rh;            
    return rh;
}