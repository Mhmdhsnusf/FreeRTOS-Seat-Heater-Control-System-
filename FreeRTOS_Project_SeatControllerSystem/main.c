/******************************************************************************
 *
 * Module: Seat Heater Control System
 *
 * File Name: main.c
 *
 * Description:
   This project implements a seat heater control system for front car seats using FreeRTOS on Tiva C microcontrollers.
   Each seat integrates a temperature sensor (LM35) and heating element with adjustable intensities (25°C, 30°C, 35°C).
   Features include real-time diagnostics for sensor failures,
   LED indicators (green, blue, cyan), and UART communication for displaying system status.
   Tasks are managed efficiently with at least 6 tasks handling GPIO, UART, GPTM, and ADC modules,
   ensuring responsive user interaction and robust performance monitoring with GPTM.
 * Author: Mohamed Hassan
 ******************************************************************************/
/* Include standard header files for ADC, FreeRTOS, tasks, event groups, etc. */
#include "adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "Port.h"
#include "Dio.h"
#include "uart0.h"
#include "Button.h"
#include "led.h"
#include "GPTM.h"
#include "FreeRTOS_Project.h"

/* Define initial heating levels for driver and passenger */
HeatingLevel ucDriverHeaterIntensity = TURN_OFF_HEATER;       /* Initialize driver heater intensity */
HeatingLevel ucPassengerHeaterIntensity = TURN_OFF_HEATER;    /* Initialize passenger heater intensity */

/* Define desired temperature for driver and passenger seats */
uint8 usDriver_Seat_Desired_Temp = SEAT_HEATING_OFF;          /* Initialize driver seat desired temperature */
uint8 usPassenger_Seat_Desired_Temp= SEAT_HEATING_OFF;        /* Initialize passenger seat desired temperature */

/* Define current temperature variables for driver and passenger seats */
uint16 usDriverSeatCurrentTemp;                               /* Variable to hold driver seat current temperature */
uint16 usPassengerSeatCurrentTemp;                            /* Variable to hold passenger seat current temperature */

/* Define a structure to record failures */
FailureRecord latestFailure[2];                               /* Array to hold latest failure records */

/* Define a constant for delay iterations */
#define NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND 369          /* Define macro for delay iterations */

/* Function to create a delay in milliseconds */
void Delay_MS(unsigned long long n)                           /* Function to create delay */
{
    volatile unsigned long long count = 0;                    /* Volatile to prevent optimization */
    while(count++ < (NUMBER_OF_ITERATIONS_PER_ONE_MILI_SECOND * n) ); /* Delay loop */
}

/* Structure to hold driver seat task information */
TaskInformation DriverSeatTask =
{
 /* Driver Task ID*/ DRIVER_TASK_ID,                          /* Task ID for driver seat */
 /* Driver Seat Button */  SW1_BUTTON_PIN_NUM_INDEX,          /* Button pin number for driver seat */
 /* Driver Seat Button Presses Count */ 0,                    /* Button presses count for driver seat */
 /* Driver Seat Heating Temp*/ SEAT_HEATING_OFF               /* Initial heating level for driver seat */
};

/* Structure to hold passenger seat task information */
TaskInformation PassengerSeatTask =
{
 /* Passenger Task ID*/ PASSENGER_TASK_ID,                    /* Task ID for passenger seat */
 /* Passenger Seat Button */  SW2_BUTTON_PIN_NUM_INDEX,       /* Button pin number for passenger seat */
 /* Passenger Seat Button Presses Count*/ 0,                  /* Button presses count for passenger seat */
 /* Passenger Seat Heating Temp*/ SEAT_HEATING_OFF            /* Initial heating level for passenger seat */
};

/* Function prototypes */
void prvSetupHardware(void);                                  /* Prototype for hardware setup function */
void vSeatHeatingLevelTask(void *pvParameters);               /* Prototype for seat heating level task */
void vHeaterMonitorTask(void *pvParameters);                  /* Prototype for heater monitor task */
void vHeaterControlTask(void *pvParameters);                  /* Prototype for heater control task */
void vGetCurrentTempTask(void *pvParameters);                 /* Prototype for get current temperature task */
void vDashboardDisplayTask(void *pvParameters);               /* Prototype for dashboard display task */
void vFailureHandleTask(void *pvParameters);                  /* Prototype for failure handle task */
void vRunTimeMeasurementsTask(void *pvParameters);            /* Prototype for runtime measurements task */

/* Task handles */
TaskHandle_t xDriverSeatHeatingLevelTask;                     /* Task handle for driver seat heating level task */
TaskHandle_t xPassengerSeatHeatingLevelTask;                  /* Task handle for passenger seat heating level task */
TaskHandle_t xHeaterMonitorTask;                              /* Task handle for heater monitor task */
TaskHandle_t xHeaterControlTask;                              /* Task handle for heater control task */
TaskHandle_t xGetCurrentTempTask;                             /* Task handle for get current temperature task */
TaskHandle_t xDashboardDisplayTask;                           /* Task handle for dashboard display task */
TaskHandle_t xFailureHandleTask;                              /* Task handle for failure handle task */
TaskHandle_t xRunTimeMeasurementsTask;                        /* Task handle for runtime measurements task */

/* Variables to hold task times */
uint32 ullTasksOutTime[9];                                    /* Array to hold tasks out time */
uint32 ullTasksInTime[9];                                     /* Array to hold tasks in time */
uint32 ullTasksTotalTime[9];                                  /* Array to hold tasks total time */
uint8 ucCPU_Load=0;                                           /* Variable to hold CPU load */

/* Event group handle */
EventGroupHandle_t xSystemEventGroup;                         /* Handle for system event group */

/* Main function */
void main(void)
{
    prvSetupHardware();                                       /* Setup hardware */
    xSystemEventGroup = xEventGroupCreate();                  /* Create event group */

    /* Create tasks with appropriate parameters and priorities */
    xTaskCreate(vSeatHeatingLevelTask, "DriverSeatHeatingLevelTask", 150, (void*)&DriverSeatTask, 4, &xDriverSeatHeatingLevelTask);
    xTaskCreate(vSeatHeatingLevelTask, "PassengerSeatHeatingLevelTask", 150, (void*)&PassengerSeatTask, 4, &xPassengerSeatHeatingLevelTask);
    xTaskCreate(vGetCurrentTempTask, "GetCurrentTempTask", 150, NULL, 3, &xGetCurrentTempTask);
    xTaskCreate(vFailureHandleTask, "Failure", 150, NULL, 1, &xFailureHandleTask);
    xTaskCreate(vHeaterMonitorTask, "HeaterMonitorTask", 256, NULL, 2, &xHeaterMonitorTask);
    xTaskCreate(vHeaterControlTask, "HeaterControlTask", 100, NULL, 1, &xHeaterControlTask);
    xTaskCreate(vDashboardDisplayTask, "DashboardDisplayTask", 150, NULL, 1, &xDashboardDisplayTask);
    xTaskCreate(vRunTimeMeasurementsTask, "RunTimeMeasurementsTask", 256, NULL, 1, &xRunTimeMeasurementsTask);

    /* Set application task tags for runtime statistics */
    vTaskSetApplicationTaskTag(xDriverSeatHeatingLevelTask, (void *) 1);
    vTaskSetApplicationTaskTag(xPassengerSeatHeatingLevelTask, (void *) 2);
    vTaskSetApplicationTaskTag(xGetCurrentTempTask, (void *) 3);
    vTaskSetApplicationTaskTag(xFailureHandleTask, (void *) 4);
    vTaskSetApplicationTaskTag(xHeaterMonitorTask, (void *) 5);
    vTaskSetApplicationTaskTag(xHeaterControlTask, (void *) 6);
    vTaskSetApplicationTaskTag(xDashboardDisplayTask, (void *) 7);
    vTaskSetApplicationTaskTag(xRunTimeMeasurementsTask, (void *) 8);

    /* Start the scheduler */
    vTaskStartScheduler();

    /* Infinite loop to handle error if scheduler fails */
    for (;;)
    {
        /* Ensure to handle error here if required */
    }
}

/************************************************************************************
Service name:           prvSetupHardware
void                    prvSetupHardware(void)
Syntax:                 void prvSetupHardware(void)
Service ID[hex]:        N/A
Sync/Async:             Synchronous
Reentrancy:             Non Reentrant
Parameters (in):        None
Parameters (inout):     None
Parameters (out):       None
Return value:           None
Description:            Initializes hardware components including Port, Dio, UART0, ADC0, ADC1, and GPTM_WTimer0.
 ************************************************************************************/
void prvSetupHardware(void)
{
    Port_Init(&Port_Configuration);     /* Initialize Port Driver module */
    Dio_Init(&Dio_Configuration);       /* Initialize Dio Driver module */
    UART0_Init();                       /* Initialize UART0 */
    ADC0_Init();                        /* Initialize ADC0 */
    ADC1_Init();                        /* Initialize ADC1 */
    GPTM_WTimer0Init();                 /* Initialize General Purpose Timer Module WTimer0 */
}

/************************************************************************************
Service name:           vSeatHeatingLevelTask
void                    vSeatHeatingLevelTask(void *pvParameters)
Syntax:                 void vSeatHeatingLevelTask(void *pvParameters)
Service ID[hex]:        N/A
Sync/Async:             Asynchronous
Reentrancy:             Reentrant for different tasks
Parameters (in):        pvParameters - Pointer to TaskInformation structure
Parameters (inout):     None
Parameters (out):       None
Return value:           None
Description:            Task to manage seat heating levels based on button presses.
                        Handles button presses, updates desired temperatures, and communicates
                        with UART0 if necessary. Monitors a specific seat identified by TaskInformation.
 ************************************************************************************/
void vSeatHeatingLevelTask(void *pvParameters)
{
    uint8 Button_flag = 0;  /* Variable to track button press state */
    uint8 SeatsRequiredTemperatures[4] = {SEAT_HEATING_OFF, LOW_SEAT_HEATING_TEMPERATURE, MEDIUM_SEAT_HEATING_TEMPERATURE, HIGH_SEAT_HEATING_TEMPERATURE}; /* Array defining heating levels */
    TaskInformation *pTaskInformation = (TaskInformation *)pvParameters; /* Cast pvParameters to TaskInformation pointer */

    for (;;)
    {
        if (buttonCheckState(pTaskInformation->ucSeatButton) == BUTTON_PRESSED) /* Check if button is pressed */
        {
            if (Button_flag == 0) /* Check if button flag is not set */
            {
                pTaskInformation->ucSeatButtonPressesCount += 1; /* Increment button press count */
                if ((pTaskInformation->ucSeatButtonPressesCount) == 4) /* Check if button press count reaches maximum */
                {
                    pTaskInformation->ucSeatButtonPressesCount = 0; /* Reset button press count */
                }
                pTaskInformation->ucSeatHeatingTemp = SeatsRequiredTemperatures[pTaskInformation->ucSeatButtonPressesCount]; /* Update desired temperature */

                if (pTaskInformation->ucTaskID == PASSENGER_TASK_ID) /* Check if task is for passenger */
                {
                    usPassenger_Seat_Desired_Temp = pTaskInformation->ucSeatHeatingTemp; /* Update passenger seat desired temperature */
                    // UART0_SendInteger(usPassenger_Seat_Desired_Temp); /* Example: Send temperature via UART0 */
                }
                else
                {
                    usDriver_Seat_Desired_Temp = pTaskInformation->ucSeatHeatingTemp; /* Update driver seat desired temperature */
                    // UART0_SendInteger(usDriver_Seat_Desired_Temp); /* Example: Send temperature via UART0 */
                    // UART0_SendString("Driver ^^\r\n"); /* Example: Send debug message via UART0 */
                }
                Button_flag = 1; /* Set button flag indicating button was pressed */
                xEventGroupSetBits(xSystemEventGroup, SEAT_MONITOR_TASK_BIT); /* Set event bit for seat monitor task */
            }
        }
        else
        {
            Button_flag = 0; /* Reset button flag indicating button is not pressed */
        }
        vTaskDelay(pdMS_TO_TICKS(200)); /* Delay task execution for 200 milliseconds */
    }
}

/************************************************************************************
Service name:           vGetCurrentTempTask
void                    vGetCurrentTempTask(void *pvParameters)
Syntax:                 void vGetCurrentTempTask(void *pvParameters)
Service ID[hex]:        N/A
Sync/Async:             Asynchronous
Reentrancy:             Reentrant for different tasks
Parameters (in):        pvParameters - Pointer to task parameters (not used)
Parameters (inout):     None
Parameters (out):       None
Return value:           None
Description:            Task to read current temperatures from ADC channels.
                        Calculates current temperatures for driver and passenger seats
                        and updates global variables. Communicates temperature readings
                        using UART0. Monitors ADC channels ADC0 and ADC1.
 ************************************************************************************/
void vGetCurrentTempTask(void *pvParameters)
{
    for (;;)
    {
        usDriverSeatCurrentTemp = (ADC0_ReadChannel() * MAX_VALID_TEMP) / ADC_FULL_SCALE_VALUE; /* Read and calculate driver seat temperature */
        usPassengerSeatCurrentTemp = (ADC1_ReadChannel() * MAX_VALID_TEMP) / ADC_FULL_SCALE_VALUE; /* Read and calculate passenger seat temperature */
        xEventGroupSetBits(xSystemEventGroup, SEAT_CURRENT_TEMP_TASK_BIT); /* Set event bit for current temperature task */
        vTaskDelay(pdMS_TO_TICKS(500)); /* Delay task execution for 500 milliseconds */
    }
}

/************************************************************************************
Service name: vHeaterMonitorTask
Task ID: None
Syntax: void vHeaterMonitorTask(void *pvParameters)
Service ID[hex]: None
Sync/Async: Synchronous
Reentrancy: Non Reentrant
Parameters (in): pvParameters - Pointer to task parameters
Parameters (inout): None
Parameters (out): None
Return value: None
Description: Monitors seat heater temperatures and adjusts heater intensities based on desired and current temperatures for both driver and passenger seats.
 ************************************************************************************/
void vHeaterMonitorTask(void *pvParameters)
{
    EventBits_t xEventGroupValue;
    const EventBits_t xBitsToWaitFor = (SEAT_CURRENT_TEMP_TASK_BIT | SEAT_MONITOR_TASK_BIT);

    for (;;)
    {
        /* Block to wait for event bits to become set within the event group. */
        xEventGroupValue = xEventGroupWaitBits(
                xSystemEventGroup, /* The event group to read. */
                xBitsToWaitFor,   /* Bits to test. */
                pdTRUE,           /* Clear bits on exit if the unblock condition is met. */
                pdFALSE,          /* Don't wait for all bits. */
                portMAX_DELAY     /* Don't time out. */
        );

        if (((xEventGroupValue & SEAT_CURRENT_TEMP_TASK_BIT) != 0) || ((xEventGroupValue & SEAT_MONITOR_TASK_BIT) != 0))
        {
            if ((usDriver_Seat_Desired_Temp - usDriverSeatCurrentTemp) >= 10)
            {
                ucDriverHeaterIntensity = HIGH_HEATER_INTENSITY;
            }
            else if ((usDriver_Seat_Desired_Temp - usDriverSeatCurrentTemp) >= 5 && (usDriver_Seat_Desired_Temp - usDriverSeatCurrentTemp) < 10)
            {
                ucDriverHeaterIntensity = MEDIUM_HEATER_INTENSITY;
            }
            else if ((usDriver_Seat_Desired_Temp - usDriverSeatCurrentTemp) > 2 && (usDriver_Seat_Desired_Temp - usDriverSeatCurrentTemp) < 5)
            {
                ucDriverHeaterIntensity = LOW_HEATER_INTENSITY;
            }
            else if ((usDriverSeatCurrentTemp - usDriver_Seat_Desired_Temp) <= 3)
            {
                ucDriverHeaterIntensity = TURN_OFF_HEATER;
            }
            else
            {
                /* Nothing to do */
            }

            if ((usPassenger_Seat_Desired_Temp - usPassengerSeatCurrentTemp) >= 10)
            {
                ucPassengerHeaterIntensity = HIGH_HEATER_INTENSITY;
            }
            else if ((usPassenger_Seat_Desired_Temp - usPassengerSeatCurrentTemp) >= 5 && (usPassenger_Seat_Desired_Temp - usPassengerSeatCurrentTemp) < 10)
            {
                ucPassengerHeaterIntensity = MEDIUM_HEATER_INTENSITY;
            }
            else if ((usPassenger_Seat_Desired_Temp - usPassengerSeatCurrentTemp) > 2 && (usPassenger_Seat_Desired_Temp - usPassengerSeatCurrentTemp) < 5)
            {
                ucPassengerHeaterIntensity = LOW_HEATER_INTENSITY;
            }
            else if ((usPassengerSeatCurrentTemp - usPassenger_Seat_Desired_Temp) <= 3)
            {
                ucPassengerHeaterIntensity = TURN_OFF_HEATER;
            }
            else
            {
                /* Nothing to do */
            }
        }

        xEventGroupSetBits(xSystemEventGroup, SEAT_HEATER_INTENSITY_TASK_BIT);
        xEventGroupSetBits(xSystemEventGroup, SEAT_CURRENT_TEMP_TASK_BIT);
        vTaskDelay(500);
    }
}

/************************************************************************************
Service name: vHeaterControlTask
Task ID: None
Syntax: void vHeaterControlTask(void *pvParameters)
Service ID[hex]: None
Sync/Async: Synchronous
Reentrancy: Non Reentrant
Parameters (in): pvParameters - Pointer to task parameters
Parameters (inout): None
Parameters (out): None
Return value: None
Description: Controls the activation of seat heaters based on the intensity levels calculated by the Heater Monitor task.
 ************************************************************************************/
void vHeaterControlTask(void *pvParameters)
{
    for (;;)
    {
        xEventGroupWaitBits(
                xSystemEventGroup,             /* The event group to read. */
                SEAT_HEATER_INTENSITY_TASK_BIT, /* Bits to test. */
                pdTRUE,                        /* Clear bits on exit if the unblock condition is met. */
                pdFALSE,                       /* Don't wait for all bits. */
                portMAX_DELAY                  /* Don't time out. */
        );

        /* Control driver seat heater */
        if (ucDriverHeaterIntensity == LOW_HEATER_INTENSITY)
        {
            Dio_WriteChannel(DioConf_BLUE_LED_CHANNEL_ID_INDEX, STD_OFF);
            Dio_WriteChannel(DioConf_GREEN_LED_CHANNEL_ID_INDEX, STD_ON);
            Delay_MS(250);
        }
        else if (ucDriverHeaterIntensity == MEDIUM_HEATER_INTENSITY)
        {
            Dio_WriteChannel(DioConf_GREEN_LED_CHANNEL_ID_INDEX, STD_OFF);
            Dio_WriteChannel(DioConf_BLUE_LED_CHANNEL_ID_INDEX, STD_ON);
            Delay_MS(250);
        }
        else if (ucDriverHeaterIntensity == HIGH_HEATER_INTENSITY)
        {
            Dio_WriteChannel(DioConf_GREEN_LED_CHANNEL_ID_INDEX, STD_ON);
            Dio_WriteChannel(DioConf_BLUE_LED_CHANNEL_ID_INDEX, STD_ON);
            Delay_MS(250);
        }
        else
        {
            Dio_WriteChannel(DioConf_GREEN_LED_CHANNEL_ID_INDEX, STD_OFF);
            Dio_WriteChannel(DioConf_BLUE_LED_CHANNEL_ID_INDEX, STD_OFF);
            Delay_MS(250);
        }

        /* Control passenger seat heater */
        if (ucPassengerHeaterIntensity == LOW_HEATER_INTENSITY)
        {
            Dio_WriteChannel(DioConf_BLUE_LED_OUT_CHANNEL_ID_INDEX, STD_OFF);
            Dio_WriteChannel(DioConf_GREEN_LED_OUT_CHANNEL_ID_INDEX, STD_ON);
            Delay_MS(250);
        }
        else if (ucPassengerHeaterIntensity == MEDIUM_HEATER_INTENSITY)
        {
            Dio_WriteChannel(DioConf_GREEN_LED_OUT_CHANNEL_ID_INDEX, STD_OFF);
            Dio_WriteChannel(DioConf_BLUE_LED_OUT_CHANNEL_ID_INDEX, STD_ON);
            Delay_MS(250);
        }
        else if (ucPassengerHeaterIntensity == HIGH_HEATER_INTENSITY)
        {
            Dio_WriteChannel(DioConf_GREEN_LED_OUT_CHANNEL_ID_INDEX, STD_ON);
            Dio_WriteChannel(DioConf_BLUE_LED_OUT_CHANNEL_ID_INDEX, STD_ON);
            Delay_MS(250);
        }
        else
        {
            Dio_WriteChannel(DioConf_GREEN_LED_OUT_CHANNEL_ID_INDEX, STD_OFF);
            Dio_WriteChannel(DioConf_BLUE_LED_OUT_CHANNEL_ID_INDEX, STD_OFF);
            Delay_MS(250);
        }
    }
}

/************************************************************************************
Service name: vDashboardDisplayTask
Task ID: None
Syntax: void vDashboardDisplayTask(void *pvParameters)
Service ID[hex]: None
Sync/Async: Synchronous
Reentrancy: Non Reentrant
Parameters (in): pvParameters - Pointer to task parameters
Parameters (inout): None
Parameters (out): None
Return value: None
Description: Updates and displays dashboard information including seat heater states, desired and current temperatures, task execution times, and CPU load on the UART console.
 ************************************************************************************/
void vDashboardDisplayTask(void *pvParameters)
{
    for (;;)
    {
        xEventGroupWaitBits(
                xSystemEventGroup,             /* The event group to read. */
                SEAT_HEATER_INTENSITY_TASK_BIT, /* Bits to test. */
                pdTRUE,                        /* Clear bits on exit if the unblock condition is met. */
                pdFALSE,                       /* Don't wait for all bits. */
                portMAX_DELAY                  /* Don't time out. */
        );

        UART0_SendString("\033[2J");  // Clear the entire screen
        UART0_SendString("\033[H");   // Move cursor to the top-left corner
        UART0_SendString("\t\tDriver Seat ");
        UART0_SendString("\t  Passenger Seat ");
        UART0_SendString("\r\n\nHEATER STATE:       ");
        switch (ucDriverHeaterIntensity)
        {
        case 4:
            UART0_SendString("OFF");
            break;
        case 3:
            UART0_SendString("HIGH");
            break;
        case 2:
            UART0_SendString("MEDIUM");
            break;
        case 1:
            UART0_SendString("LOW");
            break;
        default:
            break;
        }

        UART0_SendString("                 ");
        switch (ucPassengerHeaterIntensity)
        {
        case 4:
            UART0_SendString("OFF");
            break;
        case 3:
            UART0_SendString("HIGH");
            break;
        case 2:
            UART0_SendString("MEDIUM");
            break;
        case 1:
            UART0_SendString("LOW");
            break;
        default:
            break;
        }

        UART0_SendString("\r\n\nRequired Temp:       ");
        UART0_SendInteger(usDriver_Seat_Desired_Temp);
        UART0_SendString("                   ");
        UART0_SendInteger(usPassenger_Seat_Desired_Temp);
        UART0_SendString("\r\n\nCurrent Temp:       ");
        UART0_SendInteger(usDriverSeatCurrentTemp);
        UART0_SendString("                   ");
        UART0_SendInteger(usPassengerSeatCurrentTemp);

        taskENTER_CRITICAL();
        UART0_SendString("\r\n\nIdleTask execution time is ");
        UART0_SendInteger(ullTasksTotalTime[0] / 10);
        UART0_SendString(" msec \r\n");
        UART0_SendString("DriverSeatHeatingLevelTask execution time is ");
        UART0_SendInteger(ullTasksTotalTime[1] / 10);
        UART0_SendString(" msec \r\n");
        UART0_SendString("PassengerSeatHeatingLevelTask execution time is ");
        UART0_SendInteger(ullTasksTotalTime[2] / 10);
        UART0_SendString(" msec \r\n");
        UART0_SendString("HeaterMonitorTask execution time is ");
        UART0_SendInteger(ullTasksTotalTime[5] / 10);
        UART0_SendString(" msec \r\n");
        UART0_SendString("HeaterControlTask execution time is ");
        UART0_SendInteger(ullTasksTotalTime[6] / 10);
        UART0_SendString(" msec \r\n");
        UART0_SendString("GetCurrentTempTask execution time is ");
        UART0_SendInteger(ullTasksTotalTime[3] / 10);
        UART0_SendString(" msec \r\n");
        UART0_SendString("DashboardDisplayTask execution time is ");
        UART0_SendInteger(ullTasksTotalTime[7] / 10);
        UART0_SendString(" msec \r\n");
        UART0_SendString("FailureHandleTask execution time is ");
        UART0_SendInteger(ullTasksTotalTime[4] / 10);
        UART0_SendString(" msec \r\n");
        UART0_SendString("RunTimeMeasurementsTask execution time is ");
        UART0_SendInteger(ullTasksTotalTime[8] / 10);
        UART0_SendString(" msec \r\n");
        UART0_SendString("CPU Load is ");
        UART0_SendInteger(ucCPU_Load);
        UART0_SendString("% \r\n");
        taskEXIT_CRITICAL();

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/************************************************************************************
Service name: vFailureHandleTask
Task ID: None
Syntax: void vFailureHandleTask(void *pvParameters)
Service ID[hex]: None
Sync/Async: Synchronous
Reentrancy: Non Reentrant
Parameters (in): pvParameters - Pointer to task parameters
Parameters (inout): None
Parameters (out): None
Return value: None
Description: Handles temperature sensor failure conditions for driver and passenger seats.
             Updates latest failure information and adjusts heater intensity accordingly.
 ************************************************************************************/
void vFailureHandleTask(void *pvParameters)
{
    for (;;)
    {
        xEventGroupWaitBits(
                xSystemEventGroup,               /* The event group to read. */
                SEAT_CURRENT_TEMP_TASK_BIT,      /* Bits to test. */
                pdTRUE,                          /* Clear bits on exit if the unblock condition is met. */
                pdFALSE,                         /* Don't wait for all bits. */
                portMAX_DELAY                    /* Don't time out. */
        );

        if (((usDriverSeatCurrentTemp < 5) || (usDriverSeatCurrentTemp > 40)))
        {
            latestFailure[0].failureMessage = "Invalid Driver Temperature Sensor Range ";
            latestFailure[0].level = ucDriverHeaterIntensity;
            latestFailure[0].timestamp = GPTM_WTimer0Read();
            Dio_WriteChannel(DioConf_RED_LED_CHANNEL_ID_INDEX, STD_ON);
            ucDriverHeaterIntensity = TURN_OFF_HEATER;
        }
        else
        {
            Dio_WriteChannel(DioConf_RED_LED_CHANNEL_ID_INDEX, STD_OFF);
        }

        if (((usPassengerSeatCurrentTemp < 5) || (usPassengerSeatCurrentTemp > 40)))
        {
            latestFailure[1].failureMessage = "Invalid Passenger Temperature Sensor Range ";
            latestFailure[1].level = ucPassengerHeaterIntensity;
            latestFailure[1].timestamp = GPTM_WTimer0Read();
            Dio_WriteChannel(DioConf_RED_LED_OUT_CHANNEL_ID_INDEX, STD_ON);
            ucPassengerHeaterIntensity = TURN_OFF_HEATER;
        }
        else
        {
            Dio_WriteChannel(DioConf_RED_LED_OUT_CHANNEL_ID_INDEX, STD_OFF);
        }
    }
}

/************************************************************************************
Service name: vRunTimeMeasurementsTask
Task ID: None
Syntax: void vRunTimeMeasurementsTask(void *pvParameters)
Service ID[hex]: None
Sync/Async: Synchronous
Reentrancy: Non Reentrant
Parameters (in): pvParameters - Pointer to task parameters
Parameters (inout): None
Parameters (out): None
Return value: None
Description: Measures and updates the total execution time of critical tasks and calculates
             CPU load based on the execution times.
 ************************************************************************************/
void vRunTimeMeasurementsTask(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        uint8_t ucCounter;
        uint32_t ullTotalTasksTime = 0;

        vTaskDelayUntil(&xLastWakeTime, RUNTIME_MEASUREMENTS_TASK_PERIODICITY);

        for (ucCounter = 1; ucCounter < 9; ucCounter++)
        {
            ullTotalTasksTime += ullTasksTotalTime[ucCounter];
        }

        ucCPU_Load = ((ullTotalTasksTime) * 100) / GPTM_WTimer0Read();
    }
}
