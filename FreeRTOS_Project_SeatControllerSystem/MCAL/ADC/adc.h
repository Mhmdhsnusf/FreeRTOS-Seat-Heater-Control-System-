 /******************************************************************************
 *
 * Module: ADC0
 *
 * File Name: adc0.h
 *
 * Description: Header file for the TM4C123GH6PM ADC driver
 *
 * Author: Mohamed Hassan
 *
 *******************************************************************************/

#ifndef ADC0_H_
#define ADC0_H_

#include "std_types.h"
/*******************************************************************************
 *                              Module Data Types                              *
 *******************************************************************************/
/* Enumeration for Port_PinDirectionType used by the PORT APIs */
typedef enum
{
    ADC_PROCESSOR,ADC_ANALOG_COMPARATOR0,ADC_ANALOG_COMPARATOR1,ADC_GPIO_PINS,ADC_TIMER,
    ADC_PWM_GENERATOR1,ADC_PWM_GENERATOR2,ADC_PWM_GENERATOR3
}ADC_EventMultiplexerSelect;
/*******************************************************************************
 *                             Preprocessor Macros                             *
 *******************************************************************************/
#define ADC_ASEN0 0
#define ADC_EM0 0
#define ADC_EM1 4
#define ADC_EM2 8
#define ADC_EM3 12
#define ADC_DITHER 6
#define ADC_VREF 0
#define ADC_AIN0 0

/*******************************************************************************
 *                            Functions Prototypes                             *
 *******************************************************************************/

extern void ADC0_Init(void);
extern uint16 ADC0_ReadChannel();
extern void ADC1_Init(void);
extern uint16 ADC1_ReadChannel();
#endif
