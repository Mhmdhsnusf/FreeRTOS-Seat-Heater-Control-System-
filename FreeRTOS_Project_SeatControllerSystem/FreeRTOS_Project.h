/******************************************************************************
 *
 * File Name: FreeRTOS_Project.h
 *
 * Description:  Header file for Seat Heater Control System
 *
 * Author: Mohamed Hassan
 ******************************************************************************/

#ifndef FREERTOS_PROJECT_H_
#define FREERTOS_PROJECT_H_

#define SEAT_HEATING_OFF 0
#define LOW_SEAT_HEATING_TEMPERATURE 25
#define MEDIUM_SEAT_HEATING_TEMPERATURE 30
#define HIGH_SEAT_HEATING_TEMPERATURE 35
#define DRIVER_TASK_ID 0
#define PASSENGER_TASK_ID 1
#define MAX_VALID_TEMP 45
#define MIN_VALID_TEMP 5
#define ADC_FULL_SCALE_VALUE 4095
#define RUNTIME_MEASUREMENTS_TASK_PERIODICITY (2000U)

/* Enum defining different heating levels */
typedef enum
{
    LOW_HEATER_INTENSITY = 1,
    MEDIUM_HEATER_INTENSITY = 2,
    HIGH_HEATER_INTENSITY = 3,
    TURN_OFF_HEATER = 4
} HeatingLevel;

/* Struct to record failure details */
typedef struct {
    char *failureMessage;  // Message describing the failure
    uint32_t timestamp;    // Timestamp of when the failure occurred
    HeatingLevel level;    // Heating level at the time of failure
} FailureRecord;

/* Struct defining task-specific information */
typedef struct
{
    const uint8_t ucTaskID;            // Task ID for identification
    const uint8_t  ucSeatButton;       // Button associated with the seat
    uint8_t ucSeatButtonPressesCount;  // Number of presses on the seat button
    uint8_t ucSeatHeatingTemp;         // Current heating temperature setting
} TaskInformation;

/* Definitions for the event bits in the event group */
#define SEAT_MONITOR_TASK_BIT ( 1UL << 0UL )               // Event bit 0
#define SEAT_CURRENT_TEMP_TASK_BIT ( 1UL << 1UL )          // Event bit 1
#define SEAT_HEATER_INTENSITY_TASK_BIT ( 1UL << 2UL )      // Event bit 2

#endif /* FREERTOS_PROJECT_H_ */
