#include "movement.h"
#include <atomic>
#include "uart.h"

movement::movement():Asservissement(){
}

bool movement::goToPoint(int16_t x,int16_t y,Rotation rotation, Direction direction) {
    if (commandBuffer.getAvailableSpace()>=2) {
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_LOOKAT,
            x,
            y,
            0,                  //useless
            direction,
            rotation
        });
        commandBuffer.push(Command{
            BaseCommand::LINEAR,
            x,
            y,
            0,                  //useless
            Direction::SHORTEST, //useless
            Rotation::SHORTEST    //useless
        });
    }
    else {
        return -1;
    }
    return 0;
}

bool movement::goToPoint(int16_t x,int16_t y,int16_t theta, Rotation rotationFirst, Direction direction, Rotation rotationSecond) {
    if (commandBuffer.getAvailableSpace()>=3) {
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_LOOKAT,
            x,
            y,
            0,                  //useless
            direction,
            rotationFirst
        });
        commandBuffer.push(Command{
            BaseCommand::LINEAR,
            x,
            y,
            0,                  //useless
            Direction::SHORTEST, //useless
            Rotation::SHORTEST    //useless
        });
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_THETA,
            0,                  //useless
            0,                  //useless
            theta,
            Direction::SHORTEST, //useless
            rotationSecond
        });
    }
    else {
        return -1;
    }
    return 0;
}

bool movement::setTargetAngulaire(int16_t angle,Rotation rotation) {
    if (!commandBuffer.isFull()) {
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_THETA,
            0,                  //useless
            0,                  //useless
            angle,
            Direction::SHORTEST, //useless
            rotation
        });
    }
    else {
        return -1;
    }
    return 0;
}

bool movement::setTargetLookAt(int16_t x,int16_t y,Rotation rotation, Direction direction) {
    if (!commandBuffer.isFull()) {
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_LOOKAT,
            x,
            y,
            0,                  //useless
            direction,
            rotation
        });
    }
    else {
        return -1;
    }
    return 0;
}

bool movement::setTargetMaxSpeedLinear(uint16_t max_speed,uint16_t max_acceleration,uint16_t max_deceleration) {
    if (!commandBuffer.isFull()) {
        commandBuffer.push(Command{
            BaseCommand::MAX_SPEED_LINEAR,
            max_speed,
            max_acceleration,
            max_deceleration,
            Direction::BACKWARD,    //usless
            Rotation::NONE          //usless
        });
    }
    else {
        return -1;
    }
    return 0;
}

bool movement::setTargetMaxSpeedAngular(uint16_t max_speed,uint16_t max_acceleration,uint16_t max_deceleration) {
    if (!commandBuffer.isFull()) {
        commandBuffer.push(Command{
            BaseCommand::MAX_SPEED_ANGULAR,
            max_speed,
            max_acceleration,
            max_deceleration,
            Direction::BACKWARD,    //usless
            Rotation::NONE          //usless
        });
    }
    else {
        return -1;
    }
    return 0;
}

bool movement::Stop(void){
    commandBuffer.resetTail();
    enableStop = true;
    return true;
}

bool movement::Pause(void){
    enablePause = true;
    return true;
}

bool movement::Resume(void){
    enablePause = false;
    return true;
}

void movement::launchCommand(void){
    usartprintf("\nbaseCommand %s\n",baseCommandToString(currentCommand.baseCommand));
    usartprintf("x %d\n",currentCommand.x);
    usartprintf("y %d\n",currentCommand.y);
    usartprintf("theta %d\n",currentCommand.theta);
    usartprintf("direction %s\n",directionToChar(currentCommand.direction));
    usartprintf("rotation %s\n\n\n",rotationToChar(currentCommand.rotation));

    switch (currentCommand.baseCommand)
    {
    case BaseCommand::LINEAR:
        setProtectedTargetLineaire(currentCommand.x,currentCommand.y);
        break;

    case BaseCommand::ANGULAR_LOOKAT:
        if (currentCommand.direction == Direction::SHORTEST) {
            Asservissement::setTargetLookAt(currentCommand.x,currentCommand.y,currentCommand.rotation);
        }
        else if (currentCommand.direction == Direction::FORWARD) {
            Asservissement::setTargetLookAtForward(currentCommand.x,currentCommand.y,currentCommand.rotation);
        }
        else {
            Asservissement::setTargetLookAtBackward(currentCommand.x,currentCommand.y,currentCommand.rotation);
        }
        break;

    case BaseCommand::ANGULAR_THETA:
        setProtectedTargetAngulaire(currentCommand.theta,currentCommand.rotation);
        break;

    case BaseCommand::MAX_SPEED_LINEAR:
        positionControlLineaire.vitesseMaxAv = currentCommand.x;
        positionControlLineaire.vitesseMaxAr = currentCommand.x;
        positionControlLineaire.accelerationMaxAv = currentCommand.y;
        positionControlLineaire.accelerationMaxAr = currentCommand.y;
        positionControlLineaire.decelerationMaxAv = currentCommand.theta;
        positionControlLineaire.decelerationMaxAr = currentCommand.theta;
        break;

    case BaseCommand::MAX_SPEED_ANGULAR:
        positionControlAngulaire.vitesseMaxAv = currentCommand.x;
        positionControlAngulaire.vitesseMaxAr = currentCommand.x;
        positionControlAngulaire.accelerationMaxAv = currentCommand.y;
        positionControlAngulaire.accelerationMaxAr = currentCommand.y;
        positionControlAngulaire.decelerationMaxAv = currentCommand.theta;
        positionControlAngulaire.decelerationMaxAr = currentCommand.theta;
        break;

    default:
        break;
    }
}

bool movement::currentCommandRun(void){
// slower chaining of actions with statistics to have a more consistent time
    if (currentCommand.baseCommand == BaseCommand::LINEAR) {
        return getLinearError()!=0;
    }
    else {
        return getAngularError()!=0;
    }
}

void movement::loop(void){
    Asservissement::loop();

    if (enablePause && !pause) {
        Asservissement::setTargetStop();
        pause = true;
    }

    if (!enablePause && pause) {
        launchCommand();
        pause = false;
    }

    if (!pause) {
        if (!run && !commandBuffer.isEmpty()) {
            currentCommand = commandBuffer.pop();
            launchCommand();
            run = true;
        }
        else if (run && !currentCommandRun()) {
            if (!commandBuffer.isEmpty()) {
                currentCommand = commandBuffer.pop();
                launchCommand();
            }
            else {
                run = false;
            }
        }
    }

    if (enableStop == true) {
        enableStop = false;
        commandBuffer.resetHead();
        Asservissement::setTargetStop();
    }
}

uint16_t movement::getCommandBufferSize(){
    return commandBuffer.getUsedSpace() + run;
}