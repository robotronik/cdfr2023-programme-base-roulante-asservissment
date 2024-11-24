#pragma once

#include "Asservissement.h"
#include "robot.h"
#include "stdint.h"
#include "circularBuffer.h"
#include "config.h"


struct Command {
    BaseCommand baseCommand;
    int x = 0;
    int y = 0;
    int theta = 0;
    Direction direction = Direction::FORWARD;
    Rotation rotation = Rotation::SHORTEST;
};

class movement : public Asservissement
{
private:

    CircularBuffer<Command, 50> commandBuffer;
    Command currentCommand;
    bool run = false;

public:
    movement(robot* bot);
    bool goToPoint(uint16_t x,uint16_t y,Rotation rotation = Rotation::SHORTEST, Direction direction = Direction::FORWARD);
    bool goToPoint(uint16_t x,uint16_t y,uint16_t theta, Rotation rotationFirst = Rotation::SHORTEST, Direction direction = Direction::FORWARD, Rotation rotationSecond = Rotation::SHORTEST);
    bool setConsigneAngulaire(uint16_t angle,Rotation rotation);
    bool setConsigneLookAtForward(uint16_t x,uint16_t y,Rotation rotation);
    bool setConsigneLookAtBackward(uint16_t x,uint16_t y,Rotation rotation);
    bool commandRun(void);
    void loop(void);
    ~movement();

private:
    void launchCommande(void);
    bool currentCommandRun(void);
};

