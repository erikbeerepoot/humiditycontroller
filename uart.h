/* 
 * File:   
 * Author: E. E. Beerepoot
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  

short   configure_uart();

// -=-=-=-=-=-=-=- TX -=-=-=-=-=-=-=-
//Send an array of bytes
int     transmit_buffer(char *buffer, int length);

//Send a null terminated string
int     transmit_string(char *string);

//Callback invoked upon tx complete
void    register_transmit_callback(void (*callback)(int));

// -=-=-=-=-=-=-=- RX -=-=-=-=-=-=-=-
//Registers a callback to be invoked when data of receive_length is invoked (see next fn))
void    register_serial_listener(void (*callback)(char *,int));

//Sets the length of the message we want to receive
void    set_receive_length(int length);

//Set to true if we should echo characters received from remote
void    set_should_echo(int echo);

#endif	/* XC_HEADER_TEMPLATE_H */

