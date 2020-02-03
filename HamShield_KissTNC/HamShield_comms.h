


#ifndef _HAMSHIELD_COMMS_H_
#define _HAMSHIELD_COMMS_H_


#if defined(ARDUINO)
#include "Arduino.h"

//#define nCS A1 //15 // USED PIN 9 INSTEAD TO NOT TIE UP ADC INPUTS. ALSO NOT SURE IF PIN DEFINITITION EVER SPECIFIED TO USE A1 AS A GPIO? USE A DEFAULT GPIO INSTEAD - KB1MHD 29JAN2020
#define nCS 9 //for Arduino Pro Mini
#define CLK A5 //19 //
#define DAT A4 //18 //
#define MIC 3
#else // assume Raspberry Pi
#include "stdint.h"
#include <wiringPi.h>
#include <softTone.h>

#define nCS 0 //BCM17, HW pin 11 
#define CLK 3 //BCM22, HW pin 15 
#define DAT 2 //BCM27, HW pin 13 
#define MIC 1 //BCM18, HW pin 12
#endif


void HSsetPins(uint8_t ncs, uint8_t clk, uint8_t dat);

int8_t HSreadBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data);
int8_t HSreadBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data);
int8_t HSreadWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data);

bool HSwriteBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
bool HSwriteBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
bool HSwriteWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);


// hardware abstraction layer

unsigned long HSmillis();
void HSdelay(unsigned long ms);
void HSdelayMicroseconds(unsigned int us);


#endif /* _HAMSHIELD_COMMS_H_ */
