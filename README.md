# Seat Heater Control System

This project implements a seat heater control system for front car seats using FreeRTOS on Tiva C microcontrollers.

## Description

The seat heater control system manages the heating intensity of front car seats using LM35 temperature sensors and adjustable heating elements. The system allows users to set desired temperatures for both driver and passenger seats through button presses, with feedback provided via LED indicators and UART communication.

Key features include:
- Real-time temperature monitoring and control.
- Failure detection and handling for temperature sensor anomalies.
- Task management using FreeRTOS, with tasks handling GPIO, UART, ADC, and more.
- Runtime measurements and CPU load monitoring for performance analysis.

## Components

- Tiva C Series TM4C123G LaunchPad (or similar Tiva C microcontroller)
- LM35 temperature sensors
- LEDs for status indicators
- UART communication module
- Buttons for user input

## Requirements

- TivaWare™ Peripheral Driver Library
- FreeRTOS kernel

## Setup

1. **Hardware Setup**: Connect LM35 sensors, LEDs, buttons, and UART module to the Tiva C microcontroller as per the hardware configuration.

2. **Software Setup**: Ensure TivaWare™ Peripheral Driver Library and FreeRTOS are properly installed and configured in your development environment.

3. **Compilation**: Compile the project using the provided Makefile or IDE setup.

4. **Upload**: Upload the compiled binary to the Tiva C microcontroller.

## Usage

- **Button Operation**: Pressing buttons adjusts the heating intensity levels in predefined steps (e.g., off, low, medium, high).
- **Temperature Monitoring**: Current temperatures of driver and passenger seats are monitored and displayed periodically.
- **UART Communication**: System status and debug messages are communicated via UART for external monitoring.

## Folder Structure

```
├── drivers/           # TivaWare™ Peripheral Driver Library
├── FreeRTOS/          # FreeRTOS kernel files
├── include/           # Header files
├── src/               # Source files
├── README.md          # This file
└── Makefile           # Makefile for project compilation
```

## Contributing

Contributions are welcome! Please fork the repository and submit pull requests with your enhancements.

## Authors

- Mohamed Hassan

## License

This project is licensed under the [MIT License](LICENSE).

---
