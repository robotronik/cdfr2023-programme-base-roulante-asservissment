# CDFR 2023 Control Board

## Description

This program is designed for the CDFR control board. This board supports motor control, odometry for position sensing, closed-loop control, and I2C communication. The program is intended to run on an STM32F401RE.

## I2C (Slave mode only)

The I2C adress of this bord is 42 in decimal.

[find function documentation ](../librairie-commune/Readme.md)

## Installation

To install this project, follow these steps:

1. Clone the repository to your local machine.
2. Open the project with Visual Studio Code and the PlatformIO extension.
3. Connect an STLink as follows:
4. Make sure the 2 STLink jumpers are disconnected.
5. Make sure the correct port is set up in the platform.ini file.


## Développement asservissement

Liste des points à améliorer

1. Auto calibration de l’odométrie (déterminer automatiquement la taille et l’écartement des roues codeuses) : 
    1. (fait) Refaire le buffer circulaire pour optimiser le stockage
    2. Essayer de trouver un moyen de faire le calcule de la façon la plus optimal
2. Système de versionnage de l’interface
3. (fait) Auto génération de l’interface coté STM32
4. (fait) Sécurisation des commandes et des déplacements (plus de moveForward qui peut casser mais des goto combinant le lookForward et le moveForward)
5. (commencé) Torque contrôle (pour cogner les mures sans patinage) + (auto set de coordonner)
6. Faire des courbes
    1. Séparation plus claire dans le code entre le générateur position et asservissement
    2. Déterminer une interface (gcode, arc de cercle : se synchroniser avec le path finding)
7. Continuer à développer le simulateur
    1. gestion des thread
    2. simulation de la physique du robot
8. Simuler le programme principal
    1.  makefile
    2. Simulation du lidar
9. In Application Programming (programmer le stm32 via la liaison I2C)


## Author

This program was created by Guillaume DALLE. (guillaume.dalle@grenoble-inp.org)