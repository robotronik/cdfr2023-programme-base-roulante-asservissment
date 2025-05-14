#pragma once

#include "Asservissement.h"
#include "stdint.h"
#include "types/circularBuffer.h"


struct Command {
    BaseCommand baseCommand;
    int x = 0;
    int y = 0;
    int theta = 0;
    Direction direction = Direction::SHORTEST;
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
    movement();
    ~movement(){};
    bool goToPoint(int16_t x,int16_t y,Rotation rotation = Rotation::SHORTEST, Direction direction = Direction::SHORTEST);
    bool goToPoint(int16_t x,int16_t y,int16_t theta, Rotation rotationFirst = Rotation::SHORTEST, Direction direction = Direction::SHORTEST, Rotation rotationSecond = Rotation::SHORTEST);
    bool setTargetAngulaire(int16_t angle,Rotation rotation);
    bool setTargetLookAt(int16_t x,int16_t y,Rotation rotation,Direction direction);
    bool setTargetMaxSpeedLinear(uint16_t max_speed,uint16_t max_acceleration,uint16_t max_deceleration);
    bool setTargetMaxSpeedAngular(uint16_t max_speed,uint16_t max_acceleration,uint16_t max_deceleration);

    bool Stop(void);
    bool Pause(void);
    bool Resume(void);

    void printStatistic(void);

    void loop();
    uint16_t getCommandBufferSize();

private:
    void launchCommand(void);
    bool currentCommandRun(void);
};

