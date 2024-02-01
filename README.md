# CDFR 2023 Control Board

## Description

This program is designed for the CDFR 2023 control board. This board supports motor control, odometry for position sensing, closed-loop control, and I2C communication. The program is intended to run on an STM32F401RE.

## I2C (Slave mode only)

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
| 32 | set Consigne Angulaire | (uint16_t) angle,<br> (uint16_t) rotation | -
| 33 | set Consigne LookAt | (uint16_t) x,<br> (uint16_t) y,<br> (uint16_t) rotation | -
| | | | |
| 40 | robot Is Moving | - | (uint16_t) bool |
| 41 | robot Is Running | - | (uint16_t) bool |
| 42 | robot Is Turning | - | (uint16_t) bool |
| 43 | Get Angular error | - | (uint16_t) error |
| 44 | Get Linear error | - | (uint16_t) error |

 

## Installation

To install this project, follow these steps:

1. Clone the repository to your local machine.
2. Open the project with Visual Studio Code and the PlatformIO extension.
3. Connect an STLink as follows:
4. Make sure the 2 STLink jumpers are disconnected.
5. Make sure the correct port is set up in the platform.ini file.

## Author

This program was created by Guillaume DALLE. (guillaume.dalle@grenoble-inp.org)