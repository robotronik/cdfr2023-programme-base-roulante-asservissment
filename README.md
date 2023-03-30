# CDFR 2023 Control Board

## Description

This program is designed for the CDFR 2023 control board. This board supports motor control, odometry for position sensing, closed-loop control, and I2C communication. The program is intended to run on an STM32F401RE.

## I2C (Slave mode only)

| Number (8 bits) | Command | Data |
| --- | --- | --- |
| 10 | Turn on LED 1 | |
| 11 | Turn off LED 1 | |
| 12 | Turn on LED 2 | |
| 13 | Turn off LED 2 | |
| 20 | Get Coordinates | Send 6 bits: (uint16_t) x, (uint16_t) y, (uint16_t) theta |
| 21 | Set Coordinates | Recieve 6 bits : (uint16_t) x, (uint16_t) y, (uint16_t) theta |
| 30 | Set Linear controle | Recieve 6 bits : (uint16_t) x, (uint16_t) y, (uint16_t) direction |
| 31 | Set Angular controle | Recieve 2 bits : (uint16_t) teta
| 32 | Stop controle
| 33 | Get Angular error | Send 2 bits: (uint16_t) error|
| 34 | Get Linear error | Send 2 bits: (uint16_t) error|

Stop controle stop the robot with the brake. After a Stop controle, the servitude restar automatically when a set angular or a Linear controle are realised.
Linear controle realise first a rotation and then, the robot go to the position
Angular controle 

## Installation

To install this project, follow these steps:

1. Clone the repository to your local machine.
2. Open the project with Visual Studio Code and the PlatformIO extension.
3. Connect an STLink as follows:
4. Make sure the 2 STLink jumpers are disconnected.
5. Make sure the correct port is set up in the platform.ini file.

## Author

This program was created by Guillaume DALLE. (guillaume.dalle@grenoble-inp.org)