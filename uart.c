#include <libpic30.h>
#include <string.h>
#include "xc.h"
#include "helper.h"

#define FOSC    (7370000ULL)
#define FCY     (FOSC/2)
#define BAUDRATE 38400
#define BRGVAL ((FCY/BAUDRATE)/4)-1

#define TX_BUFFER_SIZE 64
#define RX_BUFFER_SIZE 64

//TX params
int transmitting = 0;
int tx_length = 0;
volatile int tx_index = 0;
void (*tx_callback)(int);
volatile char tx_buffer[TX_BUFFER_SIZE];

//RX params
int should_echo = 0;
int rx_length = 1;
volatile int rx_index = 0;
void (*rx_callback)(char *,int);
volatile char rx_buffer[RX_BUFFER_SIZE];

void __attribute__((__interrupt__, no_auto_psv)) _U1TXInterrupt(void)
{   
    IFS0bits.U1TXIF = 0;    
    
    if(transmitting != 1){
        return;
    }
            
    //Transmit next byte
    if(tx_index <= tx_length){
        U1TXREG = tx_buffer[tx_index++];        
    } else {
        transmitting = 0;
        U1STAbits.UTXEN = 0;
        if(tx_callback!=NULL){
            (*tx_callback)(tx_length);
        }
    }    
    
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void){    	
    char received_char;
    
    if(U1STAbits.URXDA == 1){
        received_char = U1RXREG;
        rx_buffer[rx_index++] = received_char;
        
        if(rx_index==rx_length || (received_char == '\n' || received_char == '\r')){
            if(rx_callback != NULL){
                (*rx_callback)((char*)rx_buffer,rx_length);
            }
            rx_index = 0;
        }            
    }       
    IFS0bits.U1RXIF = 0;        
}

void __attribute__((interrupt, no_auto_psv)) _U1ErrInterrupt(void){
    IFS4bits.U1EIF = 0;
}

/* configure the uart:
 * 1: clear receiving buffer
 * 2: set UART mode
 * 3: calculate BRG value based on baud rate & crystal speed
 * 4: enable interrupts for RX (priority 2) & TX (priority 1)
 * 5: turn on the UART peripheral
 */
short configure_uart() {	
    AD1PCFGLbits.PCFG3 = 1;
    AD1PCFGLbits.PCFG4 = 1;	
		
    unlock_pps();

    //RXD -> RP2
    RPINR18bits.U1RXR = 2;
            
    //TXD -> RP3
    RPOR1bits.RP3R = 0b00011;     
	
	lock_pps();
	
	//1 stop bits, 8 bit data, no parity, low baud rate
	U1MODEbits.STSEL = 0;
	U1MODEbits.PDSEL = 0b00;
	U1MODEbits.BRGH = 1;
	U1MODEbits.UEN = 0b00;
		
	U1BRG = BRGVAL;
	
	/* rx priority 2, tx priority 1
     * we can stand to miss sending data to the main controller,
     * but we'd prefer not to miss critical speed inputs */
	IPC3bits.U1TXIP = 5;
	IPC2bits.U1RXIP = 4;
    IPC16bits.U1EIP = 6;
            
	IFS0bits.U1RXIF = 0;
	IFS0bits.U1TXIF = 0;
    IFS4bits.U1EIF = 0;
    
	//enable rx & tx interrupts 
	IEC0bits.U1RXIE = 1;
	IEC0bits.U1TXIE = 1;
    IEC4bits.U1EIE = 1;
            
	//enable the uart peripheral		
	U1MODEbits.UARTEN = 1;	

    //Reset module state
    transmitting = 0;
    tx_index = 0;
    tx_length = 0;
    memset((void*)tx_buffer,0,TX_BUFFER_SIZE);
    memset((void*)rx_buffer,0,TX_BUFFER_SIZE);
    
	return 0;
}

//Transmit a buffer over the UART
int transmit_buffer(char *buffer, int length){
    if(transmitting == 1 || length > TX_BUFFER_SIZE){
        return -1;
    }
    
    //Clear module state
    memcpy((void*)tx_buffer,buffer,length);
    tx_length = length;   
    tx_index = 0;
                   
    U1STAbits.UTXEN = 1;    
    transmitting = 1;
    return length;
}

int transmit_string(char *string){
    int length = strlen(string);
    transmit_buffer(string,length);
    return length;
}

//Registers a callback listener for incoming bytes over UART
void register_serial_listener(void (*callback)(char *,int)){
    rx_callback = callback;
}

void register_transmit_callback(void (*callback)(int)){
    tx_callback = callback;
}

//Allows the listener to specify the number of expected bytes
void set_receive_length(int length){
    if(length < 1 || length > RX_BUFFER_SIZE) return;
    rx_length = length;
}

//Sets whether we should echo incoming characters
void set_should_echo(int echo){
    should_echo = echo;
}