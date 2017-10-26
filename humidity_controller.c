#define FOSC    (7370000ULL)
#define FCY     (FOSC/2)

#include <libpic30.h>
#include <stdio.h>

#include "xc.h"
#include "data_logger.h"
#include "pwm.h"
#include "uart.h"

float sum(float *array,int count);

float k_p = 30.0;
float k_i = 0.0;
float k_d;

float humidity_setpoint = 95;
int fan_on_time = 300;

int compute_on_time(float avg_humidity, float sum_humidity_error);
float sum_humidity_error(float humidity_values[], int count);
float sum(float array[],int count);

char data_string[35];
float humidity = 0;
float temperature = 0;
       
//returns on_time in ms
int run_loop(){    
    get_latest_entry(&humidity,&temperature);                   
    return compute_on_time(humidity,0);
}

int compute_on_time(float avg_humidity, float sum_humidity_error){
    float error = humidity_setpoint - avg_humidity;
    float result = k_p * error + k_i * sum_humidity_error;
    if(result < 0){
        result = 0;
    }
    return (int)(result*10);
}

float sum_humidity_error(float humidity_values[], int count){
    int i=0;
    
    float total_error = 0;
    for(i=0; i < count - 1; count++){
        total_error += humidity_setpoint - humidity_values[i];
    }
    return total_error;
}


float sum(float array[],int count){
    float sum = 0;
    int index = 0;
    for(index=0;index<count-1;index++){
        sum += array[index];
    }
    return sum;
}

void set_humidity_setpoint(float setpoint){
    humidity_setpoint = setpoint;
}

float get_humidity_setpoint(){
    return humidity_setpoint;    
}


void set_pid_gains(float k_p_new, float k_i_new, float k_d_new){
    k_p = k_p_new;
    k_i = k_i_new;
    k_d = k_d_new;
}

void get_pid_gains(float *k_p_out, float *k_i_out, float *k_d_out){
    *k_p_out = k_p;
    *k_i_out = k_i;
    *k_d_out = k_d;
}

int get_fan_on_time(){
    return fan_on_time;
}

void set_fan_on_time(int on_time){
    fan_on_time = on_time;
}

