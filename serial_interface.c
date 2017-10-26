//Implements a simple serial interface for the controller

#define FOSC    (7370000ULL)
#define FCY     (FOSC/2)

#include <libpic30.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include "xc.h"

#include "uart.h"
#include "data_logger.h"
#include "pwm.h"
#include "humidity_controller.h"

//Multiplier for getting SP from duty cycle
const int duty_cycle_factor = 655;

short inVerboseMode = 0;
short isStreaming = 0;

volatile int transmissionComplete = 0;

typedef enum MODE {
    command = 0,
    data,
    error,
    init
} MODE;

typedef enum COMMAND {
    reset = 'r',
    verboseMode = 'v',
    terse = 'V',
    requestTemperature = 't',     
    requestHumidity = 'T',
    requestHumiditySetpoint = 'h',
    setHumiditySetpoint = 'H',
    requestDutyCycle = 'd',
    setDutyCycle = 'D',
    requestFanOnTime = 'f',
    setFanOnTime = 'F',
    getGains = 'g',
    setGains = 'G',
    setStreamingOn = 's',
    setStreamingOff = 'S'            
} COMMAND;

void process_rx_data(char *rxBuffer,int length);
void transmit_callback(int numberOfBytesSent);
void process_data(char *rxBuffer,int length,COMMAND command);
void process_command(COMMAND command);
void print_response(COMMAND command);

//Used to store strings to transmit
char transmit_string_buffer[25]; 

MODE mode = init;
COMMAND last_command = reset;

void configure_serial_interface(){
    register_serial_listener(&process_rx_data);
    register_transmit_callback(&transmit_callback);
    mode = command;
}

//Processes the data received from the remote depending on our current mode
void process_rx_data(char *rxBuffer,int length){    
    if(mode == command){
        COMMAND cmd = (COMMAND)rxBuffer[0];
        last_command = cmd;
        process_command(cmd);        
    } else if (mode == data){
        process_data(rxBuffer,length,last_command);
    } else if (mode == init){
        transmit_string("Still initializing...\r\n");
    } else if (mode == error){
        transmit_string("Error\r\n");
    }
}

void transmit_callback(int numberOfBytesSent){
    transmissionComplete = 1;
}

//processes an incoming command
void process_command(COMMAND command){       
    float humidity, temperature;
        
    
    switch(command){
        case verboseMode:            
            inVerboseMode = 1;   
            print_response(command);
            break;
        case requestTemperature:            
            //get record of temp and send it over
            print_response(command);
            get_latest_entry(&humidity,&temperature);
            sprintf(transmit_string_buffer,"%f\r\n",(double)temperature); 
            transmit_string(transmit_string_buffer);
            break;
        case requestHumidity:
            //get record of humidity and send it over                   
            print_response(command);
            get_latest_entry(&humidity,&temperature);
            sprintf(transmit_string_buffer,"%f\r\n",(double)humidity);            
            transmit_string(transmit_string_buffer);
            break;
        case requestHumiditySetpoint:
        {            
            float sp = get_humidity_setpoint();
            print_response(command);                  
            sprintf(transmit_string_buffer,"RH Setpoint: %2.2f\r\n",(double)sp);
            transmit_string(transmit_string_buffer);
            break;
        }
        case setGains:            
            print_response(command);
            mode = data;
            set_receive_length(10);     
            break;
        case getGains:
        {
            float p,i,d;
            print_response(command);         
            get_pid_gains(&p,&i,&d);
            sprintf(transmit_string_buffer,"p: %2.2f i: %2.2f d: %2.2f\r\n",(double)p,(double)i,(double)d);              
            transmit_string(transmit_string_buffer);
            break;
        }
        case setHumiditySetpoint:
            print_response(command);
            //set the desired target humidity                         
            mode = data;
            set_receive_length(10);                        
            break;
        case setDutyCycle:
            print_response(command);
            mode = data;            
            set_receive_length(10);                        
            break;
        case requestDutyCycle:
        {            
            int pwm_setpoint = get_pwm_setpoint();
            int duty_cycle = pwm_setpoint / duty_cycle_factor;
            sprintf(transmit_string_buffer,"Duty cycle: %d\n\r",duty_cycle);
            transmit_string(transmit_string_buffer);            
            break;
        }
        case requestFanOnTime:
        {            
            int on_time = get_fan_on_time();
            sprintf(transmit_string_buffer,"Req. Fan on time: %d [ms] \n\r",on_time);
            transmit_string(transmit_string_buffer);       
            break;
        }
        case setFanOnTime:
        {
            print_response(command);
            mode = data;            
            set_receive_length(10);    
            break;
        }  
        case setStreamingOn:
        {
            print_response(command);
            isStreaming = 1;
            break;
        }
        case setStreamingOff:
        {
            print_response(command);
            isStreaming = 0;
            break;
        }
        case terse:                 
            print_response(command);
            inVerboseMode = 0;            
            break;
        default:            
            break;        
    }   
}

void process_data(char *buffer, int length,COMMAND for_command){                                
    switch(for_command){
        case setDutyCycle:
        {
            int duty_cycle = strtol(buffer,NULL,10);
            if(duty_cycle < 0 || duty_cycle > 100){
                transmit_string("Value out of range.\n\r");    
            } else {
                transmit_string("OK\n\r");    
                set_pwm_setpoint(duty_cycle*duty_cycle_factor);
            }
            break;
        }
        case setGains:
        {
            int gain = strtol(buffer,NULL,10);
            if(gain < 0 || gain > 500){
                transmit_string("Value out of range.\n\r");    
            } else {
                transmit_string("OK\n\r");    
                set_pid_gains((float)(gain),0,0);
            }
            break;            
        }
        case setHumiditySetpoint:
        {
            int rh_setpoint = strtol(buffer,NULL,10);
            if(rh_setpoint < 0 || rh_setpoint > 100){
                transmit_string("Value out of range.\n\r");    
            } else {
                transmit_string("OK\n\r");    
                set_humidity_setpoint((float)rh_setpoint);
            }            
            break;
        }
        case setFanOnTime:
        {
            int fan_on_time = strtol(buffer,NULL,10);
            if(fan_on_time < 0 || fan_on_time > 1000){
                transmit_string("Value out of range.\n\r");    
            } else {
                transmit_string("OK\n\r");    
                set_fan_on_time(fan_on_time);
            }            
            break;
        }
        default:
            break;
    }    
    mode = command;
    set_receive_length(1);
}

void print_response(COMMAND command){
    if(inVerboseMode != 1) return;
    
    switch(command){
        case verboseMode:
            transmit_string("In verbose mode\r\n");
            break;
        case requestTemperature:
            transmit_string("Req. temp: ");                        
            break;
        case requestHumidity:
            transmit_string("Req. RH: ");
            break;
        case setGains:
            transmit_string("Set gains\r\n");                        
            break;
        case getGains:            
            transmit_string("Get gains\r\n");            
            break;
        case setHumiditySetpoint:
            transmit_string("Set RH [0-100]:");            
            break;
        case setDutyCycle:
            transmit_string("Set Duty Cycle [0-100]: ");            
            break;
        case requestDutyCycle:
            transmit_string("Duty cycle [0-100]: ");
            break;
        case requestFanOnTime:
            transmit_string("Req. fan on time: ");
            break;
        case setFanOnTime:
            transmit_string("Set fan on time [0 - 1000 ms]: ");
            break;
        case setStreamingOn:
            transmit_string("Streaming ON\n\r");
            break;
        case setStreamingOff:
            transmit_string("Streaming OFF\n\r");
            break;
        case terse:
            transmit_string("Disabled verbose mode\r\n");
            break;
        default:
            transmit_string("Unknown cmd\r\n");
            break;        
    }                 
}

void serial_run_loop(){        
    if(isStreaming == 1){    
        float temperature = 0;
        float humidity = 0;
        get_latest_entry(&humidity,&temperature);
        sprintf(transmit_string_buffer,"t: %3.3f h: %3.3f\n\r",(double)temperature,(double)humidity);
        transmit_string(transmit_string_buffer);
            
    }
}