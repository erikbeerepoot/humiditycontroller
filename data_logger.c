//Implements data logging functionality 

#define FOSC    (7370000ULL)
#define FCY     (FOSC/2)

#include <libpic30.h>

#include "xc.h"

#include "data_logger.h"

#define DATA_BUFFER_SIZE 96

unsigned int data_count = 0;
int didOverflow = 0;
int writeIndex = 0;
float humidityBuffer[96];
float temperatureBuffer[96];

void log_entry(float humidity, float temperature){    
    humidityBuffer[writeIndex] = humidity;
    temperatureBuffer[writeIndex] = temperature;
    
    writeIndex++;
    writeIndex = writeIndex % DATA_BUFFER_SIZE;
    if(writeIndex==0){
        didOverflow = 1;
    }
    data_count++;
}

short get_latest_entry(float *humidity, float *temperature){
    if(data_count == 0) return 0;
        
    
    if(writeIndex == 0 && didOverflow){
        *humidity = humidityBuffer[DATA_BUFFER_SIZE-1];
        *temperature = temperatureBuffer[DATA_BUFFER_SIZE-1];
    } else {
        *humidity = humidityBuffer[writeIndex-1];
        *temperature = temperatureBuffer[writeIndex-1];
    }
    return 1;
}

short get_n_latest_entries(float humidity_array[], float temperature_array[],int n){     
    if(data_count < n) return 0;
    
    int index=0;
    int count=0;
    int indices[n];        
    int startIndex = 0;
    if(writeIndex==0){
        startIndex = DATA_BUFFER_SIZE - 1;
    } else {
        startIndex = writeIndex - 1;
    }
    
    for(index=startIndex;count<n;index--){
        if(index==0) index = DATA_BUFFER_SIZE - 1;
        indices[count++] = index;
    }
      
    for(index=0;index<n;index++){
        humidity_array[index] = humidityBuffer[indices[index]];
        temperature_array[index] = temperatureBuffer[indices[index]];
    }  
    return 1;
}