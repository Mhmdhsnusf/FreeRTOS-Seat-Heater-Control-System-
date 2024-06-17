/******************************************************************************
 *
 * Module: ADC0
 *
 * File Name: adc0.c
 *
 * Description: Source file for the TM4C123GH6PM ADC driver
 *
 * Author: Mohamed Hassan
 *
 *******************************************************************************/

#include "adc.h"
#include "tm4c123gh6pm_registers.h"

/*******************************************************************************
 *                         Private Functions Definitions                       *
 *******************************************************************************/



/*******************************************************************************
 *                         Public Functions Definitions                        *
 *******************************************************************************/

void ADC0_Init(void)
{
    /* Enable clock for ADC */
    SYSCTL_RCGCADC_REG |= 0x01;
    /* Wait until ADC clock is activated and it is ready for access*/
    while(!(SYSCTL_PRADC_REG & 0x01));
    /*the trigger source for Sample Sequencer 0 is Processor (default)*/
    ADC0_ADCEMUX_REG|=(ADC_PROCESSOR<<ADC_EM0);
    /*Dither mode enabled*/
    ADC0_ADCCTL_REG|=(1<<ADC_DITHER);
    /*VDDA and GNDA are the voltage references for all ADC modules.*/
    ADC0_ADCCTL_REG&=~(1<<ADC_VREF);
    /*AIN0 analog inputs is sampled for the analog-to-digital conversion.*/
    ADC0_ADCSSMUX0_REG=0x00;
     /*
     Set bit 1 (IE - Interrupt Enable) to enable interrupt when the sample is completed.
     Set bit 2 (END - End of Sequence) to indicate this is the last conversion in the sequence.
     */
    ADC0_ADCSSCTL0_REG |= (1<<1) | (1<<2);
    /*Enable Sequencer 0*/
    ADC0_ADCACTSS_REG|=0x01;

}
uint16 ADC0_ReadChannel()
{
    uint16 digitalData = 0;
    /*SS0 bit is set*/
    ADC0_ADCPSSI_REG|=0x01;
    /* Check if the raw interrupt status is set */
    while(!(ADC0_ADCRIS_REG & 0x01));
    /* Get the 12-bit result */
    digitalData = ADC0_ADCSSFIFO0_REG;
    /* Clear the flag by writing a 1 to the ISC register */
    ADC0_ADCISC_REG|=0x01;
    return digitalData;
}
void ADC1_Init(void) {
    /* Enable clock for ADC1 */
    SYSCTL_RCGCADC_REG |= 0x02;
    /* Wait until ADC1 clock is activated and it is ready for access */
    while(!(SYSCTL_PRADC_REG & 0x02));
    /* Configure the trigger source for Sample Sequencer 0 as Processor (default) */
    ADC1_ADCEMUX_REG |= (ADC_PROCESSOR << ADC_EM0);
    /* Dither mode enabled */
    ADC1_ADCCTL_REG |= (1 << ADC_DITHER);
    /* Use default VDDA and GNDA as the voltage references */
    ADC1_ADCCTL_REG &= ~(1 << ADC_VREF);
    /* AIN1 analog input is sampled for the analog-to-digital conversion */
    ADC1_ADCSSMUX0_REG = 0x01;
    /* Enable interrupt and mark as end of sequence */
    ADC1_ADCSSCTL0_REG |= (1 << 1) | (1 << 2);
    /* Enable Sequencer 0 */
    ADC1_ADCACTSS_REG |= 0x01;
}

uint16 ADC1_ReadChannel() {
    uint16 digitalData = 0;
    /* Start SS0 conversion */
    ADC1_ADCPSSI_REG |= 0x01;
    /* Wait for conversion to complete */
    while(!(ADC1_ADCRIS_REG & 0x01));
    /* Read the 12-bit result */
    digitalData = ADC1_ADCSSFIFO0_REG & 0xFFF; // Mask to 12 bits
    /* Clear the interrupt flag */
    ADC1_ADCISC_REG |= 0x01;
    return digitalData;
}
