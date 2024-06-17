 /******************************************************************************
 *
 * Module: Port
 *
 * File Name: Port.c
 *
 * Description: Source file for TM4C123GH6PM Microcontroller - Port Driver.
 *
 * Author: Mohamed Hassan
 ******************************************************************************/

#include "Port.h"
#include "Port_Regs.h"
#include "tm4c123gh6pm_registers.h"


#if (DIO_DEV_ERROR_DETECT == STD_ON)

#include "Det.h"
/* AUTOSAR Version checking between Det and Port Modules */
#if ((DET_AR_MAJOR_VERSION != PORT_AR_RELEASE_MAJOR_VERSION)\
 || (DET_AR_MINOR_VERSION != PORT_AR_RELEASE_MINOR_VERSION)\
 || (DET_AR_PATCH_VERSION != _AR_RELEASE_PATCH_VERSION))
  #error "The AR version of Det.h does not match the expected version"
#endif

#endif
   
STATIC const Port_ConfigType* Port_pinConfigPtr = NULL_PTR;
STATIC uint8 Port_Status = PORT_NOT_INITIALIZED;
/************************************************************************************ 
Service name:           Port_Init
void                    Port_Init( const Port_ConfigType* ConfigPtr )
Syntax:                 void Port_Init( const Port_ConfigType* ConfigPtr )
Service ID[hex]:        0x00
Sync/Async:             Synchronous
Reentrancy:             Non Reentrant
Parameters (in):        ConfigPtr - Pointer to configuration set.
Parameters (inout):     None
Parameters (out):       None
Return value:           None
Description:            Initializes the Port Driver module.
************************************************************************************/
void Port_Init(const Port_ConfigType * ConfigPtr )
{
  #if (PORT_DEV_ERROR_DETECT == STD_ON)
	/* check if the input configuration pointer is not a NULL_PTR */
	if (NULL_PTR == ConfigPtr)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_INIT_SID,
		     PORT_E_PARAM_CONFIG);
	}
	else
#endif
        {       
    volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
    volatile uint32 delay = 0;
    sint8 PortNumber = -1;
    sint8 PinNumber = -1;
    STATIC sint8 count=-1;
    Port_pinConfigPtr=ConfigPtr;
    for(count=0;count<NUMBER_OF_TM4C123GH6PM_CHANNLES;count++)
    {
      /*Get the Actual Pin and Port ID*/
      if(ConfigPtr->PinsConfiguration[count].PinID<38)
      {
        PortNumber = (ConfigPtr->PinsConfiguration[count].PinID)/COMMON_NUMBER_OF_CHANNELS_IN_EACH_PORT;
        PinNumber = (ConfigPtr->PinsConfiguration[count].PinID) - (COMMON_NUMBER_OF_CHANNELS_IN_EACH_PORT * PortNumber);
      }
      /* PF0 to PF4 */
      else
      {
        PortNumber = GPIO_PORTF_ID;
        PinNumber = ConfigPtr->PinsConfiguration[count].PinID - 38;
      }
    switch(PortNumber)
    {
        case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
		 break;
	case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
		 break;
	case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
		 break;
	case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
		 break;
        case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
		 break;
        case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
		 break;
    }
    
    
    /* Enable clock for PORT and allow time for clock to start*/
    SYSCTL_RCGC2_REG |= (1<<PortNumber);
    delay = SYSCTL_RCGC2_REG ;
    
    if( ((PortNumber == GPIO_PORTD_ID) && (PinNumber == PORT_PIN_7_ID)) || ((PortNumber == GPIO_PORTF_ID) && (PinNumber == PORT_PIN_0_ID)) ) /* PD7 or PF0 */
    {
        *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = GPIO_UNLOCK_COMMIT;                     /* Unlock the GPIOCR register */   
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , PinNumber);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
    }
    else if( (PortNumber == GPIO_PORTC_ID) && (PinNumber <= PORT_PIN_3_ID) ) /* PC0 to PC3 */
    {
        continue;
    }
    else
    {
        /* Do Nothing ... No need to unlock the commit register for this pin */
    }
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , PinNumber);       /* Clear the corresponding bit in the GPIODIGITAL register to disable digital functionality on this pin */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , PinNumber);      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , PinNumber);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (PinNumber * 4));     /* Clear the PMCx bits for this pin */
    
    if(ConfigPtr->PinsConfiguration[count].PinDirection == PORT_PIN_OUT)
    {
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , PinNumber);                /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
        
        if(ConfigPtr->PinsConfiguration[count].PinLevelValue == STD_HIGH)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , PinNumber);          /* Set the corresponding bit in the GPIODATA register to provide initial value 1 */
        }
        else
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , PinNumber);        /* Clear the corresponding bit in the GPIODATA register to provide initial value 0 */
        }
    }
    else if(ConfigPtr->PinsConfiguration[count].PinDirection == PORT_PIN_IN)
    {
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , PinNumber);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
        
        if(ConfigPtr->PinsConfiguration[count].PortInternalResistor == PORT_PIN_INTERNAL_RESISTOR_PULL_UP)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , PinNumber);       /* Set the corresponding bit in the GPIOPUR register to enable the internal pull up pin */
        }
        else if(ConfigPtr->PinsConfiguration[count].PortInternalResistor == PORT_PIN_INTERNAL_RESISTOR_PULL_DOWN)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , PinNumber);     /* Set the corresponding bit in the GPIOPDR register to enable the internal pull down pin */
        }
        else
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , PinNumber);     /* Clear the corresponding bit in the GPIOPUR register to disable the internal pull up pin */
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , PinNumber);   /* Clear the corresponding bit in the GPIOPDR register to disable the internal pull down pin */
        }
    }
    else
    {
        /* Do Nothing */
    }
    if(ConfigPtr->PinsConfiguration[count].PinModeType == PORT_PIN_MODE_DIO)
    {
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , PinNumber); 
      
    }
    else if(ConfigPtr->PinsConfiguration[count].PinModeType == PORT_PIN_MODE_ANALOG)
    { 
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , PinNumber);             /* ENABLE Alternative function for this pin by setting the corresponding bit in GPIOAFSEL register */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , PinNumber);      /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
      
    }
    else
    {


        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , PinNumber);             /* ENABLE Alternative function for this pin by setting the corresponding bit in GPIOAFSEL register */
        *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (PinNumber * 4));
        *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET)|= (ConfigPtr->PinsConfiguration[count].PinModeType << (PinNumber * 4));   /* SET the PMCx bits for this pin */
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , PinNumber);

    }
    
    
        }
    }
    Port_Status = PORT_INITIALIZED;
}

/************************************************************************************ 
Service name:          Port_SetPinDirection 
Syntax:                void Port_SetPinDirection( Port_PinType Pin, Port_PinDirectionType Direction )                
Service ID[hex]:       0x01        
Sync/Async:            Synchronous
Reentrancy:            Reentrant 
Parameters (in):       Pin - Port Pin ID number , Direction - Port Pin Direction
Parameters (inout):    None 
Parameters (out):      None 
Return value:          None 
Description:           Sets the port pin direction 
************************************************************************************/
#if (PORT_SET_PIN_DIRECTION == STD_ON)
void Port_SetPinDirection
( 
  Port_PinType Pin, 
  Port_PinDirectionType Direction 
 )
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)

	if(Port_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SID, PORT_E_UNINIT);
		error = TRUE;
	}
	else
	{
		/* Do nothing */
	}
	
	if( Pin >= NUMBER_OF_TM4C123GH6PM_CHANNLES )
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SID, PORT_E_PARAM_PIN);
		error = TRUE;
	}
	else
	{
		/* Do nothing */
	}
	
	if(Port_pinConfigPtr->PinsConfiguration[count].PortPinDirectionChangeable == STD_OFF)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SID, PORT_E_DIRECTION_UNCHANGEABLE);
		error = TRUE;
	}
	else
	{
		/* Do nothing */
	}

#endif
      /*Get the Actual Pin and Port ID*/
      if(Port_pinConfigPtr->PinsConfiguration[count].PinID<38)
      {
        PortNumber = Port_pinConfigPtr->PinsConfiguration[count].PinID/COMMON_NUMBER_OF_CHANNELS_IN_EACH_PORT;
        PinNumber = Port_pinConfigPtr->PinsConfiguration[count].PinID - (COMMON_NUMBER_OF_CHANNELS_IN_EACH_PORT * PortNumber);
      }
      /* PF0 to PF4 */
      else
      {
        PortNumber = GPIO_PORTF_ID;
        PinNumber = Port_pinConfigPtr->PinsConfiguration[count].PinID - 38;
      }
    switch(PortNumber)
    {
        case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
		 break;
	case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
		 break;
	case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
		 break;
	case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
		 break;
        case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
		 break;
        case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
		 break;
    }
    
    if( ((PortNumber == GPIO_PORTD_ID) && (PinNumber == PORT_PIN_7_ID)) || ((PortNumber == GPIO_PORTF_ID) && (PinNumber == PORT_PIN_0_ID)) ) /* PD7 or PF0 */
    {
        *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = GPIO_UNLOCK_COMMIT;     /* Unlock the GPIOCR register */   
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , PinNumber);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
    }
    else if( (PortNumber == GPIO_PORTC_ID) && (PinNumber <= PORT_PIN_3_ID) ) /* PC0 to PC3 */
    {
        /* Do Nothing ...  this is the JTAG pins */
    }
    else
    {
        /* Do Nothing ... No need to unlock the commit register for this pin */
    }
 if(Direction  == PORT_PIN_OUT)
    {
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , PinNumber);                /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
        
        if(Port_pinConfigPtr->PinsConfiguration[count].PinLevelValue == STD_HIGH)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , PinNumber);          /* Set the corresponding bit in the GPIODATA register to provide initial value 1 */
        }
        else
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DATA_REG_OFFSET) , PinNumber);        /* Clear the corresponding bit in the GPIODATA register to provide initial value 0 */
        }
    }
    else if(Direction == PORT_PIN_IN)
    {
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , PinNumber);             /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */
        
        if(Port_pinConfigPtr->PinsConfiguration[count].PortInternalResistor == PULL_UP)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , PinNumber);       /* Set the corresponding bit in the GPIOPUR register to enable the internal pull up pin */
        }
        else if(Port_pinConfigPtr->PinsConfiguration[count].PortInternalResistor == PULL_DOWN)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , PinNumber);     /* Set the corresponding bit in the GPIOPDR register to enable the internal pull down pin */
        }
        else
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_UP_REG_OFFSET) , PinNumber);     /* Clear the corresponding bit in the GPIOPUR register to disable the internal pull up pin */
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_PULL_DOWN_REG_OFFSET) , PinNumber);   /* Clear the corresponding bit in the GPIOPDR register to disable the internal pull down pin */
        }
    }
  
  }
#endif

/************************************************************************************ 
Service name:          Port_RefreshPortDirection 
Syntax:                void Port_RefreshPortDirection( void )                
Service ID[hex]:       0x02     
Sync/Async:            Synchronous
Reentrancy:            Non-Reentrant 
Parameters (in):       None
Parameters (inout):    None 
Parameters (out):      None 
Return value:          None 
Description:           Refreshes port direction. 
************************************************************************************/
void Port_RefreshPortDirection( void )
{
 #if (PORT_DEV_ERROR_DETECT == STD_ON)

	if( Port_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_SET_PIN_DIRECTION_SID, PORT_REFRESH_PIN_DIRECTION_SID);
	}
	else
#endif            
	{
	
    
    volatile uint32 * PortGpio_Ptr = NULL_PTR; /* point to the required Port Registers base address */
    sint8 PortNumber = -1;
    sint8 PinNumber = -1;
    sint8 count;
    volatile uint32 delay = 0;
    for(count=0;count<NUMBER_OF_TM4C123GH6PM_CHANNLES;count++)
    {
      /*Get the Actual Pin and Port ID*/
      if(Port_pinConfigPtr->PinsConfiguration[count].PinID<38)
      {
        PortNumber = Port_pinConfigPtr->PinsConfiguration[count].PinID/COMMON_NUMBER_OF_CHANNELS_IN_EACH_PORT;
        PinNumber = Port_pinConfigPtr->PinsConfiguration[count].PinID - (COMMON_NUMBER_OF_CHANNELS_IN_EACH_PORT * PortNumber);
      }
      /*PF0 - PF4*/
      else
      {
        PortNumber = GPIO_PORTF_ID;
        PinNumber = Port_pinConfigPtr->PinsConfiguration[count].PinID - 38;
      }
    switch(PortNumber)
    {
        case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
		 break;
	case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
		 break;
	case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
		 break;
	case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
		 break;
        case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
		 break;
        case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
		 break;
    }
    
    
    /* Enable clock for PORT and allow time for clock to start*/
    SYSCTL_RCGC2_REG  |= (1<<PortNumber);
    delay = SYSCTL_RCGC2_REG ;
    
    if( ((PortNumber == GPIO_PORTD_ID) && (PinNumber == PORT_PIN_7_ID)) || ((PortNumber == GPIO_PORTF_ID) && (PinNumber == PORT_PIN_0_ID)) ) /* PD7 or PF0 */
    {
        *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = GPIO_UNLOCK_COMMIT;                     /* Unlock the GPIOCR register */   
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , PinNumber);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
    }
    else if( (PortNumber == GPIO_PORTC_ID) && (PinNumber <= PORT_PIN_3_ID) ) /* PC0 to PC3 */
    {
        /* Do Nothing ...  this is the JTAG pins */
    }
    else
    {
        /* Do Nothing ... No need to unlock the commit register for this pin */
    }
      if(Port_pinConfigPtr->PinsConfiguration[count].PinDirection == PORT_PIN_OUT)
    {
	SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , PinNumber); 
        /* Set the corresponding bit in the GPIODIR register to configure it as output pin */
    }
      else
      {
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIR_REG_OFFSET) , PinNumber); 
      }
        
    }
        }
}

/************************************************************************************ 
Service name:          Port_SetPinMode 
Syntax:                void Port_SetPinMode( Port_PinType Pin, Port_PinModeType Mode )                
Service ID[hex]:       0x04        
Sync/Async:            Synchronous
Reentrancy:            Reentrant 
Parameters (in):       Pin - Port Pin ID number , Mode - New Port Pin mode to be set on port pin.
Parameters (inout):    None 
Parameters (out):      None 
Return value:          None 
Description:           Sets the port pin mode.
************************************************************************************/
#if (PORT_SET_PIN_MODE == STD_ON)
void Port_SetPinMode
( 
Port_PinType Pin,
Port_PinModeType Mode
)
{
  #if (PORT_DEV_ERROR_DETECT == STD_ON)

	if( Port_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,  PORT_SET_PIN_MODE_SID, PORT_E_UNINIT);
		error = TRUE;
	}
	else
	{
		/* Do nothing */
	}
	
	if( Pin >= NUMBER_OF_TM4C123GH6PM_CHANNLES )
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,  PORT_SET_PIN_MODE_SID, PORT_E_PARAM_PIN);
		error = TRUE;
	}
	else
	{
		/* Do nothing */
	}
	
	if(Port_pinConfigPtr->PinsConfiguration[count].PortPinModeChangeable == STD_OFF)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,  PORT_SET_PIN_MODE_SID, PORT_E_MODE_UNCHANGEABLE);
	}
	else
	{
		/* Do nothing */
	}

#endif
    uint8 PortNumber = -1;
    uint8 PinNumber = -1;
    /*Get the Actual Pin and Port ID*/
      if(Pin<38)
      {
        PortNumber = Port_pinConfigPtr->PinsConfiguration[count].PinID/COMMON_NUMBER_OF_CHANNELS_IN_EACH_PORT;
        PinNumber = Port_pinConfigPtr->PinsConfiguration[count].PinID - (COMMON_NUMBER_OF_CHANNELS_IN_EACH_PORT * PortNumber);
      }
      /*PF0 to PF4*/
      else
      {
        PortNumber = GPIO_PORTF_ID;
        PinNumber = Port_pinConfigPtr->PinsConfiguration[count].PinID - 38;
      }
    switch(PortNumber)
    {
        case  0: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTA_BASE_ADDRESS; /* PORTA Base Address */
		 break;
	case  1: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTB_BASE_ADDRESS; /* PORTB Base Address */
		 break;
	case  2: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTC_BASE_ADDRESS; /* PORTC Base Address */
		 break;
	case  3: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTD_BASE_ADDRESS; /* PORTD Base Address */
		 break;
        case  4: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTE_BASE_ADDRESS; /* PORTE Base Address */
		 break;
        case  5: PortGpio_Ptr = (volatile uint32 *)GPIO_PORTF_BASE_ADDRESS; /* PORTF Base Address */
		 break;
    }
    
    
    /* Enable clock for PORT and allow time for clock to start*/
    SYSCTL_REGCGC2_REG |= (1<<PortNumber);
    delay = SYSCTL_REGCGC2_REG;
    
    if( ((PortNumber == GPIO_PORTD_ID) && (PinNumber == PORT_PIN_7_ID)) || ((PortNumber == GPIO_PORTF_ID) && (PinNumber == PORT_PIN_0_ID)) ) /* PD7 or PF0 */
    {
        *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_LOCK_REG_OFFSET) = GPIO_UNLOCK_COMMIT;                     /* Unlock the GPIOCR register */   
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_COMMIT_REG_OFFSET) , PinNumber);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
    }
    else if( (PortNumber == GPIO_PORTC_ID) && (PinNumber <= PORT_PIN_3_ID) ) /* PC0 to PC3 */
    {
        /* Do Nothing ...  this is the JTAG pins */
    }
    else
    {
        /* Do Nothing ... No need to unlock the commit register for this pin */
    }
     if(Mode == PORT_PIN_MODE_DIO)
    {
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , PinNumber);      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , PinNumber);             /* Disable Alternative function for this pin by clear the corresponding bit in GPIOAFSEL register */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (PinNumber * 4));     /* Clear the PMCx bits for this pin */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , PinNumber); 
      
    }
    else if(Mode == PORT_PIN_MODE_ANALOG)
    { 
    CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , PinNumber);   
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , PinNumber);             /* ENABLE Alternative function for this pin by setting the corresponding bit in GPIOAFSEL register */
    SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , PinNumber);      /* Set the corresponding bit in the GPIOAMSEL register to enable analog functionality on this pin */
    *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (PinNumber * 4));     /* Clear the PMCx bits for this pin */
      
    }
    else
    {
      CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ANALOG_MODE_SEL_REG_OFFSET) , PinNumber);      /* Clear the corresponding bit in the GPIOAMSEL register to disable analog functionality on this pin */
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_DIGITAL_ENABLE_REG_OFFSET) , PinNumber); 
      SET_BIT(*(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_ALT_FUNC_REG_OFFSET) , PinNumber);             /* ENABLE Alternative function for this pin by setting the corresponding bit in GPIOAFSEL register */
      *(volatile uint32 *)((volatile uint8 *)PortGpio_Ptr + PORT_CTL_REG_OFFSET) &= ~(0x0000000F << (PinNumber * 4)) | (Mode << (PinNumber * 4));   /* SET the PMCx bits for this pin */
    }
    
  
}
#endif
/************************************************************************************ 
Service name:          Port_GetVersionInfo 
Syntax:                void Port_GetVersionInfo( Std_VersionInfoType* versioninfo )               
Service ID[hex]:       0x03      
Sync/Async:            Synchronous
Reentrancy:            Non Reentrant 
Parameters (in):       Pin - Port Pin ID number , Mode - New Port Pin mode to be set on port pin.
Parameters (inout):    None 
Parameters (out):      versioninfo - Pointer to where to store the version information of this module.
Return value:          None 
Description:           Returns the version information of this module.
************************************************************************************/
#if (PORT_VERSION_INFO_API == STD_ON)
void Port_GetVersionInfo
(
Std_VersionInfoType* versioninfo 
)
{
#if (PORT_DEV_ERROR_DETECT == STD_ON)

	if( Port_Status == PORT_NOT_INITIALIZED)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID,  PORT_GET_VERSION_INFO_SID, PORT_E_UNINIT);
	}
	else
	{
		/* Do nothing */
	}
	
	if(versioninfo == NULL_PTR)
	{
		Det_ReportError(PORT_MODULE_ID, PORT_INSTANCE_ID, PORT_GET_VERSION_INFO_SID, PORT_E_PARAM_POINTER);
	}
	else
	{
		/* Do nothing */
	}

#endif
  		/* Copy the vendor Id */
		versioninfo->vendorID = (uint16)PORT_VENDOR_ID;
		/* Copy the module Id */
		versioninfo->moduleID = (uint16)PORT_MODULE_ID;
		/* Copy Software Major Version */
		versioninfo->sw_major_version = (uint8)PORT_SW_MAJOR_VERSION;
		/* Copy Software Minor Version */
		versioninfo->sw_minor_version = (uint8)PORT_SW_MINOR_VERSION;
		/* Copy Software Patch Version */
		versioninfo->sw_patch_version = (uint8)PORT_SW_PATCH_VERSION;
}
#endif
