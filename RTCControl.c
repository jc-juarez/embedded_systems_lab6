/*
 * RTCControl.c
 *
 * Created: 4/5/2022 16:51:32
 *  Author: jcjua
 */ 

// File for RTC Control

#include <stdint.h>
#include "sam.h"
#include "stdbool.h"
#include "RTCControl.h"

void sendI2CDataArray(uint8_t SlAddr, uint8_t ptr, char ptrData[], int size) {
	
	int i;
	/******************  SENDING SEQUENCE  **********************/
	// Sending 4 bytes to an RTC. First Byte is the pointer of the RTC memory.
	/* slave address with write signal (0) */
	SERCOM3->I2CM.ADDR.reg = (SlAddr << 1) | 0;    /* Sending slave address in write mode */
	while(!SERCOM3->I2CM.INTFLAG.bit.MB);		   /* MB = 1 if slave NACKS the address */
	
	for(i=0; i<size; i++) {
		/* placing the data from transmitting buffer to DATA register*/
		SERCOM3->I2CM.DATA.reg = ptrData[i];
		while(!SERCOM3->I2CM.INTFLAG.bit.MB){}; /* MB = 1 if slave NACKS the address */
	}

	/* After transferring the last byte stop condition will be sent */
	SERCOM3->I2CM.CTRLB.bit.CMD = 0x3;
	
}

void receiveI2CDataArray(uint8_t SlAddr, uint8_t ptr, char ptrData[], int size) {
	
	SendI2CData(SlAddr, ptr);
	
	
	int i;
	/******************  RECIEVING SEQUENCE  *************************/
	/* slave address with read signal (1) */
	SERCOM3->I2CM.ADDR.reg = (SlAddr << 1) | 1;
	while(SERCOM3->I2CM.INTFLAG.bit.SB==0){};

	for(i=0; i< size-1; i++) {
		ptrData[i] = SERCOM3->I2CM.DATA.reg;
		while(SERCOM3->I2CM.INTFLAG.bit.SB==0){};
	}
	
	// Data Filtering Section
	
	
	
	
}

void StopCondition() {
	/* After receiving the last byte stop condition will be sent */
	SERCOM3->I2CM.CTRLB.bit.CMD = 0x3;
}

void SendI2CData(uint8_t SlAddr, uint8_t data) {
	/******************  RELOCATING POINTER BEFORE RECEIVING  *************************/
	/* slave address with write signal (0) */
	SERCOM3->I2CM.ADDR.reg = (SlAddr << 1) | 0;    /* Sending slave address in write mode */

	while(SERCOM3->I2CM.INTFLAG.bit.MB ==0);		   /* MB = 1 if slave NACKS the address */
	SERCOM3->I2CM.DATA.reg = data;                 /* Sending address (seconds) for internal pointer */

	while(SERCOM3->I2CM.INTFLAG.bit.MB ==0){};         /* MB = 1 if slave NACKS the address */

	SERCOM3->I2CM.CTRLB.bit.CMD = 0x1;                 /* Sending repetead start condition */
}


