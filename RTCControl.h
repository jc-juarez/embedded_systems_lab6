/*
 * RTCControl.h
 *
 * Created: 4/5/2022 17:10:14
 *  Author: jcjua
 */ 

#ifndef RTCCONTROL_H_
#define RTCCONTROL_H_

void sendI2CDataArray(uint8_t SlAddr, uint8_t ptr, char ptrData[], int size);
void receiveI2CDataArray(uint8_t SlAddr, uint8_t ptr, char ptrData[], int size);

#endif