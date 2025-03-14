#pragma once

#include "Asservissement.h"
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
    CircularBuffer<Command, 1000> commandBuffer;
    Command currentCommand;
    bool run = false;

    bool enableStop = false;
    bool enablePause = false;
    bool enableResume = false;
    bool pause = false;

    enum StatesMovement{
        RUN,
        STOP,
        PAUSE,
        RESUME
    };

    StatesMovement currentState = RUN;

public:
    movement(position* pos);
    bool goToPoint(int16_t x,int16_t y,Rotation rotation = Rotation::SHORTEST, Direction direction = Direction::FORWARD);
    bool goToPoint(int16_t x,int16_t y,int16_t theta, Rotation rotationFirst = Rotation::SHORTEST, Direction direction = Direction::FORWARD, Rotation rotationSecond = Rotation::SHORTEST);
    bool setConsigneAngulaire(int16_t angle,Rotation rotation);
    bool setConsigneLookAtForward(int16_t x,int16_t y,Rotation rotation);
    bool setConsigneLookAtBackward(int16_t x,int16_t y,Rotation rotation);
    bool setConsigneMaxSpeedLinear(uint16_t max_speed,uint16_t max_acceleration,uint16_t max_deceleration);
    bool setConsigneMaxSpeedAngular(uint16_t max_speed,uint16_t max_acceleration,uint16_t max_deceleration);

    bool setConsigneStop(void);
    bool setConsignePause(void);
    bool setConsigneResume(void);

    void printStatistic(void);

    void loop();
    uint16_t getCommandBufferSize();
    ~movement();

private:
    void launchCommande(void);
    bool currentCommandRun(void);
};

