 /******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port.h
 *
 * Description: Header file for TM4C123GH6PM Microcontroller - Port Driver
 *
 * Author: Mohamed Hassan
 ******************************************************************************/

#ifndef PORT_H
#define PORT_H

/* Id for the company in the AUTOSAR
 * for example Mohamed Hassan's ID = 2001 :) */
#define PORT_VENDOR_ID    (2001U)

/* Dio Module Id */
#define PORT_MODULE_ID    (124U)

/* Dio Instance Id */
#define PORT_INSTANCE_ID  (0U)

/*
 * Module Version 1.0.0
 */
#define PORT_SW_MAJOR_VERSION           (1U)
#define PORT_SW_MINOR_VERSION           (0U)
#define PORT_SW_PATCH_VERSION           (0U)

/*
 * AUTOSAR Version 4.0.3
 */
#define PORT_AR_RELEASE_MAJOR_VERSION   (4U)
#define PORT_AR_RELEASE_MINOR_VERSION   (0U)
#define PORT_AR_RELEASE_PATCH_VERSION   (3U)

/*
 * Macros for Port Status
 */
#define PORT_INITIALIZED                (1U)
#define PORT_NOT_INITIALIZED            (0U)

/* Standard AUTOSAR types */
#include <std_types.h>
#include "Det.h"

/* AUTOSAR checking between Std Types and Port Module */
#if ((STD_TYPES_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (STD_TYPES_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Std_Types.h does not match the expected version"
#endif

/* Port Pre-Compile Configuration Header file */
#include "Port_Cfg.h"

/* AUTOSAR Version checking between Port_Cfg.h and Port.h files */
#if ((PORT_CFG_AR_RELEASE_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 ||  (PORT_CFG_AR_RELEASE_PATCH_VERSION != PORT_AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Port_Cfg.h does not match the expected version"
#endif

/* Software Version checking between Port_Cfg.h and Port.h files */
#if ((PORT_CFG_SW_MAJOR_VERSION != PORT_SW_MAJOR_VERSION)\
 ||  (PORT_CFG_SW_MINOR_VERSION != PORT_SW_MINOR_VERSION)\
 ||  (PORT_CFG_SW_PATCH_VERSION != PORT_SW_PATCH_VERSION))
  #error "The SW version of Port_Cfg.h does not match the expected version"
#endif

/* Non AUTOSAR files */
#include "Common_Macros.h"

/* Number of the TM4C123GH6PM Channels (JTAG pinds excluded) */
#define NUMBER_OF_TM4C123GH6PM_CHANNLES             (43U)

/* Number of Channels in each port */
#define PORTA_CHANNEL_NO        (8U)
#define PORTB_CHANNEL_NO        (8U)
#define PORTC_CHANNEL_NO        (8U)
#define PORTD_CHANNEL_NO        (8U)
#define PORTE_CHANNEL_NO        (8U)
#define PORTF_CHANNEL_NO        (6U)

#define COMMON_NUMBER_OF_CHANNELS_IN_EACH_PORT 8
/******************************************************************************
 *                      API Service Id Macros                                 *
 ******************************************************************************/
/* Service ID for Port Init */
#define PORT_INIT_SID                          (uint8)0x00

/* Service ID for Port Set Channel direction */
#if (PORT_SET_PIN_DIRECTION_API  == STD_ON)
#define PORT_SET_PIN_DIRECTION_SID             (uint8)0x01
#endif

/* Service ID for Port Refersh Channel direction */
#define PORT_REFRESH_PIN_DIRECTION_SID         (uint8)0x02

/* Service ID for Port GetVersionInfo  */
#if (PORT_VERSION_INFO_API == STD_ON)
#define PORT_GET_VERSION_INFO_SID                   (uint8)0x03
#endif

/* Service ID for Port Set Channel Mode */
#if (PORT_SET_PIN_MODE_API == STD_ON)
#define PORT_SET_PIN_MODE_SID                   (uint8)0x04
#endif

/*******************************************************************************
 *                      DET Error Codes                                        *
 *******************************************************************************/
/* DET code to report Invalid Port Pin ID requested */
#define PORT_E_PARAM_PIN                        (uint8)0x0A

/* DET code to report Port Pin not configured as changeable */
#define PORT_E_DIRECTION_UNCHANGEABLE           (uint8)0x0B

/* DET code to report API Port_Init service called with wrong parameter. */
#define PORT_E_PARAM_CONFIG                     (uint8)0x0C

/* DET code to report API Port_SetPinMode service called when mode is unchangeable. */
#define PORT_E_PARAM_INVALID_MODE               (uint8)0x0D

/* DET code to report API Port_SetPinMode service called when mode is unchangeable. */
#define PORT_E_MODE_UNCHANGEABLE                (uint8)0x0E

/*  DET code to report API service called without module initialization.  */
#define PORT_E_UNINIT                           (uint8)0x0F

/*  DET code to report APIs called with a Null Pointer */
#define PORT_E_PARAM_POINTER                    (uint8)0x10

/*******************************************************************************
 *                              Module Data Types                              *
 *******************************************************************************/


/* Type definition for Port_PinType used by the PORT APIs */
typedef uint8 Port_PinType;

/* Enumeration for Port_PinDirectionType used by the PORT APIs */
typedef enum
{
  PORT_PIN_IN,PORT_PIN_OUT
}Port_PinDirectionType;
/* Enumeration for Port_PinLevelValue used by the PORT APIs */
typedef enum
{
  PORT_PIN_LEVEL_LOW,PORT_PIN_LEVEL_HIGH
}PortPinLevelValue;

/* Type definition for Port_PinModeType used by the PORT APIs */
typedef uint8 Port_PinModeType;
/* Enumeration for Port_InternalResistor used by the PORT APIs */
typedef enum
{
   PORT_PIN_INTERNAL_RESISTOR_OFF,PORT_PIN_INTERNAL_RESISTOR_PULL_UP,PORT_PIN_INTERNAL_RESISTOR_PULL_DOWN
}Port_InternalResistor;

 /* Data Structure required for initializing the Port Driver */
typedef struct
{
  Port_PinType PinID;
  Port_PinDirectionType PinDirection;
  Port_PinModeType PinModeType;
  Port_InternalResistor PortInternalResistor;
  PortPinLevelValue PinLevelValue;
  boolean PortPinModeChangeable;
  boolean PortPinDirectionChangeable;
}PortPinConfiguration;

 /* Data Structure required for initializing All the ports pin's configurations*/
typedef struct
{
  PortPinConfiguration PinsConfiguration[NUMBER_OF_TM4C123GH6PM_CHANNLES];
}Port_ConfigType;
/*******************************************************************************
 *                      Function Prototypes                                    *
 *******************************************************************************/

/* Function for PORT Initialization API */
void Port_Init( const Port_ConfigType* ConfigPtr );

/* Function for PORT Set Channel Direction API */
#if (PORT_SET_PIN_DIRECTION == STD_ON)
void Port_SetPinDirection( Port_PinType Pin, Port_PinDirectionType Direction );
#endif

/* Function for PORT Referesh Channel Direction API */
void Port_RefreshPortDirection( void );

/* Function for PORT Set Channel Mode API */
#if (PORT_SET_PIN_MODE == STD_ON)
void Port_SetPinMode( Port_PinType Pin, Port_PinModeType Mode );
#endif

/* Function for PORT Get Version Info API */
#if (PORT_VERSION_INFO_API == STD_ON)
void Port_GetVersionInfo( Std_VersionInfoType* versioninfo );
#endif

/*******************************************************************************
 *                              Module Definitions                             *
 *******************************************************************************/

/*Port ID in TM4C123GH6PM*/
#define GPIO_PORTA_ID 0
#define GPIO_PORTB_ID 1
#define GPIO_PORTC_ID 2
#define GPIO_PORTD_ID 3
#define GPIO_PORTE_ID 4
#define GPIO_PORTF_ID 5

/*Port Pin Mode ID in TM4C123GH6PM*/
#define PORT_PIN_MODE_DIO (0U)
#define PORT_PIN_MODE_ALTERNATE_1 (1U)
#define PORT_PIN_MODE_ALTERNATE_2 (2U)
#define PORT_PIN_MODE_ALTERNATE_3 (3U)
#define PORT_PIN_MODE_ALTERNATE_4 (4U)
#define PORT_PIN_MODE_ALTERNATE_5 (5U)
#define PORT_PIN_MODE_ALTERNATE_6 (6U)
#define PORT_PIN_MODE_ALTERNATE_7 (7U)
#define PORT_PIN_MODE_ALTERNATE_8 (8U)
#define PORT_PIN_MODE_ALTERNATE_9 (9U)
#define PORT_PIN_MODE_ALTERNATE_14 (14U)
#define PORT_PIN_MODE_ANALOG (15U)

#define PORT_PIN_0_ID    (Port_PinType)0
#define PORT_PIN_1_ID    (Port_PinType)1
#define PORT_PIN_2_ID    (Port_PinType)2
#define PORT_PIN_3_ID    (Port_PinType)3
#define PORT_PIN_4_ID    (Port_PinType)4
#define PORT_PIN_5_ID    (Port_PinType)5
#define PORT_PIN_6_ID    (Port_PinType)6
#define PORT_PIN_7_ID    (Port_PinType)7

/* Pins ID in TM4C123GH6PM */
#define PORT_PIN_A0_ID    (Port_PinType)0
#define PORT_PIN_A1_ID    (Port_PinType)1
#define PORT_PIN_A2_ID    (Port_PinType)2
#define PORT_PIN_A3_ID    (Port_PinType)3
#define PORT_PIN_A4_ID    (Port_PinType)4
#define PORT_PIN_A5_ID    (Port_PinType)5
#define PORT_PIN_A6_ID    (Port_PinType)6
#define PORT_PIN_A7_ID    (Port_PinType)7
#define PORT_PIN_B0_ID    (Port_PinType)8
#define PORT_PIN_B1_ID    (Port_PinType)9
#define PORT_PIN_B2_ID    (Port_PinType)10
#define PORT_PIN_B3_ID    (Port_PinType)11
#define PORT_PIN_B4_ID    (Port_PinType)12
#define PORT_PIN_B5_ID    (Port_PinType)13
#define PORT_PIN_B6_ID    (Port_PinType)14
#define PORT_PIN_B7_ID    (Port_PinType)15
#define PORT_PIN_C0_ID    (Port_PinType)16
#define PORT_PIN_C1_ID    (Port_PinType)17
#define PORT_PIN_C2_ID    (Port_PinType)18
#define PORT_PIN_C3_ID    (Port_PinType)19
#define PORT_PIN_C4_ID    (Port_PinType)20
#define PORT_PIN_C5_ID    (Port_PinType)21
#define PORT_PIN_C6_ID    (Port_PinType)22
#define PORT_PIN_C7_ID    (Port_PinType)23
#define PORT_PIN_D0_ID    (Port_PinType)24
#define PORT_PIN_D1_ID    (Port_PinType)25
#define PORT_PIN_D2_ID    (Port_PinType)26
#define PORT_PIN_D3_ID    (Port_PinType)27
#define PORT_PIN_D4_ID    (Port_PinType)28
#define PORT_PIN_D5_ID    (Port_PinType)29
#define PORT_PIN_D6_ID    (Port_PinType)30
#define PORT_PIN_D7_ID    (Port_PinType)31
#define PORT_PIN_E0_ID    (Port_PinType)32
#define PORT_PIN_E1_ID    (Port_PinType)33
#define PORT_PIN_E2_ID    (Port_PinType)34
#define PORT_PIN_E3_ID    (Port_PinType)35
#define PORT_PIN_E4_ID    (Port_PinType)36
#define PORT_PIN_E5_ID    (Port_PinType)37
#define PORT_PIN_F0_ID    (Port_PinType)38
#define PORT_PIN_F1_ID    (Port_PinType)39
#define PORT_PIN_F2_ID    (Port_PinType)40
#define PORT_PIN_F3_ID    (Port_PinType)41
#define PORT_PIN_F4_ID    (Port_PinType)42

/*******************************************************************************
 *                            Macros for Port Status                           *
 *******************************************************************************/
 /* Port initialization status */
#define PORT_INITIALIZED                        (1U)
#define PORT_NOT_INITIALIZED                    (0U)

/*******************************************************************************
 *                       External Variables                                    *
 *******************************************************************************/

/* Extern PB structures to be used by Port and other modules */
extern const Port_ConfigType Port_Configuration;
#endif /* PORT_H */
