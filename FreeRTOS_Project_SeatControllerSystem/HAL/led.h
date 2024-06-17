/**********************************************************************************************
 *
 * Module: LED
 *
 * File Name: led.h
 *
 * Description: Header file for the TM4C123GH6PM  driver for TivaC Built-in LEDs
 *
 * Author: Mohamed Hassan
 *
 ***********************************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include "std_types.h"

#define PRESSED                ((uint8)0x00)
#define RELEASED               ((uint8)0x01)

void GPIO_RedLedOn(void);
void GPIO_BlueLedOn(void);
void GPIO_GreenLedOn(void);

void GPIO_RedLedOff(void);
void GPIO_BlueLedOff(void);
void GPIO_GreenLedOff(void);

void GPIO_RedLedToggle(void);
void GPIO_BlueLedToggle(void);
void GPIO_GreenLedToggle(void);



#endif /* GPIO_H_ */
