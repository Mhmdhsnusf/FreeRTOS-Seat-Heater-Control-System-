/******************************************************************************
 *
 * Module: Dio
 *
 * File Name: Dio_Cfg.h
 *
 * Description: Pre-Compile Configuration Header file for TM4C123GH6PM Microcontroller - Dio Driver
 *
 * Author: Mohamed Hassan
 ******************************************************************************/

#ifndef DIO_CFG_H
#define DIO_CFG_H

/*
 * Module Version 1.0.0
 */
#define DIO_CFG_SW_MAJOR_VERSION              (1U)
#define DIO_CFG_SW_MINOR_VERSION              (0U)
#define DIO_CFG_SW_PATCH_VERSION              (0U)

/*
 * AUTOSAR Version 4.0.3
 */
#define DIO_CFG_AR_RELEASE_MAJOR_VERSION     (4U)
#define DIO_CFG_AR_RELEASE_MINOR_VERSION     (0U)
#define DIO_CFG_AR_RELEASE_PATCH_VERSION     (3U)

/* Pre-compile option for Development Error Detect */
#define DIO_DEV_ERROR_DETECT                (STD_ON)

/* Pre-compile option for Version Info API */
#define DIO_VERSION_INFO_API                (STD_OFF)

/* Pre-compile option for presence of Dio_FlipChannel API */
#define DIO_FLIP_CHANNEL_API                (STD_ON)

/* Number of the configured Dio Channels */
#define DIO_CONFIGURED_CHANNLES              (8U)

/* Channel Index in the array of structures in Dio_PBcfg.c */
#define DioConf_RED_LED_CHANNEL_ID_INDEX        (uint8)0x00
#define DioConf_SW1_CHANNEL_ID_INDEX         (uint8)0x01
#define DioConf_GREEN_LED_CHANNEL_ID_INDEX        (uint8)0x02
#define DioConf_SW2_CHANNEL_ID_INDEX         (uint8)0x03
#define DioConf_BLUE_LED_CHANNEL_ID_INDEX        (uint8)0x04
#define DioConf_RED_LED_OUT_CHANNEL_ID_INDEX        (uint8)0x05
#define DioConf_GREEN_LED_OUT_CHANNEL_ID_INDEX        (uint8)0x06
#define DioConf_BLUE_LED_OUT_CHANNEL_ID_INDEX        (uint8)0x07

/* DIO Configured Port ID's  */
#define DioConf_RED_LED_PORT_NUM                (Dio_PortType)5 /* PORTF */
#define DioConf_SW1_PORT_NUM                 (Dio_PortType)5 /* PORTF */
#define DioConf_GREEN_LED_PORT_NUM                (Dio_PortType)5 /* PORTF */
#define DioConf_BLUE_LED_PORT_NUM                 (Dio_PortType)5 /* PORTF */
#define DioConf_SW2_PORT_NUM                (Dio_PortType)5 /* PORTF */
#define DioConf_RED_LED_OUT_PORT_NUM                (Dio_PortType)0 /* PORTA */
#define DioConf_GREEN_LED_OUT_PORT_NUM                (Dio_PortType)1 /* PORTB */
#define DioConf_BLUE_LED_OUT_PORT_NUM                 (Dio_PortType)0 /* PORTA */


/* DIO Configured Channel ID's */
#define DioConf_RED_LED_CHANNEL_NUM             (Dio_ChannelType)1 /* PF1 Pin 1 in PORTF */
#define DioConf_SW1_CHANNEL_NUM              (Dio_ChannelType)4 /* PF4 Pin 4 in PORTF */
#define DioConf_SW2_CHANNEL_NUM              (Dio_ChannelType)0 /*PF0 Pin 0 in PORTF */
#define DioConf_GREEN_LED_CHANNEL_NUM             (Dio_ChannelType)3 /* PF3 Pin 3 in PORTF */
#define DioConf_BLUE_LED_CHANNEL_NUM             (Dio_ChannelType)2 /* PF2 Pin 2 in PORTF */
#define DioConf_RED_LED_OUT_CHANNEL_NUM             (Dio_ChannelType)4 /* PA4 Pin 4 in PORTA */
#define DioConf_GREEN_LED_OUT_CHANNEL_NUM             (Dio_ChannelType)6 /* PB6 Pin B in PORTB */
#define DioConf_BLUE_LED_OUT_CHANNEL_NUM             (Dio_ChannelType)3 /* PA3 Pin 3 in PORTA */

#endif /* DIO_CFG_H */
