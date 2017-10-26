/*
 * File:   helper.c
 * Author: erik
 *
 * Created on August 20, 2017, 4:45 PM
 */


#include "xc.h"

void unlock_pps(){
    /* Since writes to the peripheral pin select registers are not 
     * allowed during normal execution, we need to do some magic to 
     * lock & nlock these registers. First we unlock:
     */
   	asm volatile ( "mov #OSCCONL, w1 \n"
	"mov #0x46, w2 \n"
	"mov #0x57, w3 \n"
	"mov.b w2, [w1] \n"
	"mov.b w3, [w1] \n"
	"bclr OSCCON, #6");
}

void lock_pps(){
    // Now we lock the registers
	asm volatile ( "mov #OSCCONL, w1 \n"
	"mov #0x46, w2 \n"
	"mov #0x57, w3 \n"
	"mov.b w2, [w1] \n"
	"mov.b w3, [w1] \n"
	"bset OSCCON, #6");
}