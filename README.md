# CDFR 2023 Control Board

## Description

This program is designed for the CDFR 2023 control board. This board supports motor control, odometry for position sensing, closed-loop control, and I2C communication. The program is intended to run on an STM32F401RE.

## I2C (Slave mode only)

The I2C of this bord is 42 in decimal.

| Command Num | Command | Master to Slave | Slave to Master |
| --- | --- | --- |  --- |
| 10 | Turn on LED 1 | - | - |
| 11 | Turn off LED 1 | - | - |
| 12 | Turn on LED 2 | - | - |
| 13 | Turn off LED 2 | - | - |
| | | | |
| 20 | Get Coordinates | -  | (uint16_t) x,<br> (uint16_t) y,<br> (uint16_t) theta |
| 21 | Set Coordinates | (uint16_t) x,<br> (uint16_t) y,<br> (uint16_t) theta | - |
| | | | |
| 30 | stop | - | - |
| 31 | set Consigne Lineaire | (uint16_t) x,<br> (uint16_t) y| - |
| 32 | set Consigne Angulaire | (uint16_t) angle,<br> (uint16_t) ROTATION | -
| 33 | set Consigne LookAt Forward | (uint16_t) x,<br> (uint16_t) y,<br> (uint16_t) ROTATION | -
| 34 | set Consigne LookAt Backward | (uint16_t) x,<br> (uint16_t) y,<br> (uint16_t) ROTATION | -
| | | | |
| 40 | robot Moving Is Finish | - | (uint16_t) bool |
| 41 | robot Running Is Finish | - | (uint16_t) bool |
| 42 | robot Turning Is Finish | - | (uint16_t) bool |
| 43 | Get Linear error | - | (uint16_t) error |
| 44 | Get Angular error | - | (uint16_t) error |
| 45 | Get Braking Distance | - | (uint16_t) distance |
| 50 | Disable motor | - | - |
| 51 | Enable motor | - | - |
| 60 | set max speed forward | (uint16_t) speed | - | 
| 61 | set max speed backward | (uint16_t) speed | - | 
| 62 | set max speed trigo | (uint16_t) speed | - |
| 63 | set max speed horloge | (uint16_t) speed | - |

 

## Installation

To install this project, follow these steps:

1. Clone the repository to your local machine.
2. Open the project with Visual Studio Code and the PlatformIO extension.
3. Connect an STLink as follows:
4. Make sure the 2 STLink jumpers are disconnected.
5. Make sure the correct port is set up in the platform.ini file.

## Author

This program was created by Guillaume DALLE. (guillaume.dalle@grenoble-inp.org)