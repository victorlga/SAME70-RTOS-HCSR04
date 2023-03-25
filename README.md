# SAME70-RTOS-HCSR04
This project implements a distance measurement system using an HC-SR04 ultrasonic sensor and displays the distance on a 128x32 OLED display using a SAME70-XPLD microcontroller.

The system works by triggering the ultrasonic sensor with a 10 µs pulse and measuring the duration of the returned echo signal. The duration is then converted to distance using the speed of sound. The measured distance and a chart of the distance over time is then displayed on the OLED display.

The code is written in C language and uses the Atmel Software Framework (ASF) library for the SAME70-XPLD microcontroller. It also uses the FreeRTOS library for task scheduling and synchronization.

## Dependencies

### Libraries
- `asf.h`: Atmel Software Framework
- `conf_board.h`: board configuration
- `gfx_mono_ug_2832hsweg04.h`: monochrome graphics
- `gfx_mono_text.h`: monochrome graphics text
- `gfx_mono_generic.h`: monochrome graphics generic
- `sysfont.h`: system font

### Modules: 
- [HC-SR04](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf)
- [OLED-Xplained-Pro](https://ww1.microchip.com/downloads/en/DeviceDoc/40002176A.pdf)

### Peripherals:
- RTT: Real-time Timer
- USART: Universal Asynchronous Receiver/Transmitter
- PIOA: Parallel Input/Output A
- PMC: Power Management Controller
- NVIC: Nested Vectored Interrupt Controller
    
### Pins:
- `PA2`: Trigger
- `PA24`: Echo

### APIs:
- freeRTOS


## Installation
- Clone the repository to your local machine.
- Open the project in Microship Studio.
- Build the project.
- Connect the SAME70 microcontroller to your computer via USB.
- Flash the code to the microcontroller.

## Usage
The system will start running as soon as the code is flashed to the microcontroller. The measured distance will be displayed on the OLED display in centimeters.

## Documentation

- SAME70-XPLD: https://www.farnell.com/datasheets/2007959.pdf
- HC-SR04: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
- OLED-Xplained-Pro: https://ww1.microchip.com/downloads/en/DeviceDoc/40002176A.pdf

## License
This code is released under the MIT License. See LICENSE file for details.
