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
    Direction direction = Direction::SHORTEST;
    Rotation rotation = Rotation::SHORTEST;
};

class movement : public Asservissement
{
private:
    enum stateMovement {
        NONE = 0,
        ANGULAR = 1,
        LINEAR = 2,
    };

    CircularBuffer<Command, COMMANDE_BUFFER_SIZE> commandBuffer;
    Command currentCommand;
    double maxSpeedOut;
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
    bool goToPoint(int16_t x,int16_t y,Rotation rotation = Rotation::SHORTEST, Direction direction = Direction::SHORTEST);
    bool goToPoint(int16_t x,int16_t y,int16_t theta, Rotation rotationFirst = Rotation::SHORTEST, Direction direction = Direction::SHORTEST, Rotation rotationSecond = Rotation::SHORTEST);
    bool setConsigneAngulaire(int16_t angle,Rotation rotation);
    bool setConsigneLookAt(int16_t x,int16_t y,Rotation rotation,Direction direction);
    bool setConsigneMaxSpeedLinear(uint16_t max_speed,uint16_t max_acceleration,uint16_t max_deceleration);
    bool setConsigneMaxSpeedAngular(uint16_t max_speed,uint16_t max_acceleration,uint16_t max_deceleration);
    bool setPosition(int x, int y, int theta);

    bool setConsigneStop(void);
    bool setConsignePause(void);
    bool setConsigneResume(void);

    void printStatistic(void);

    void loop();
    uint16_t getCommandBufferSize();
    ~movement();

private:
    void launchCommande(void);
    double findLinearMaxSpeedOut(void);
    bool currentCommandRun(void);
};

