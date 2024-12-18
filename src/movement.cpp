#include "movement.h"
#include <atomic>

movement::movement(position* pos):Asservissement(pos){
}

bool movement::goToPoint(uint16_t x,uint16_t y,Rotation rotation, Direction direction){
    if(commandBuffer.getAvailableSpace()>=2){
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
            Direction::FORWARD, //useless
            Rotation::SHORTEST    //useless
        });
    }
    else{
        return -1;
    }
    return 0;
}

bool movement::goToPoint(uint16_t x,uint16_t y,uint16_t theta, Rotation rotationFirst, Direction direction, Rotation rotationSecond){
    if(commandBuffer.getAvailableSpace()>=3){
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
            Direction::FORWARD, //useless
            Rotation::SHORTEST    //useless
        });
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_THETA,
            0,                  //useless
            0,                  //useless
            theta,
            Direction::FORWARD, //useless
            rotationSecond
        });
    }
    else{
        return -1;
    }
    return 0;
}

bool movement::setConsigneAngulaire(uint16_t angle,Rotation rotation){
    if(!commandBuffer.isFull()){
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_THETA,
            0,                  //useless
            0,                  //useless
            angle,
            Direction::FORWARD, //useless
            rotation
        });
    }
    else{
        return -1;
    }
    return 0;
}

bool movement::setConsigneLookAtForward(uint16_t x,uint16_t y,Rotation rotation){
    if(!commandBuffer.isFull()){
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_LOOKAT,
            x,
            y,
            0,                  //useless
            Direction::FORWARD,
            rotation
        });
    }
    else{
        return -1;
    }
    return 0;
}

bool movement::setConsigneLookAtBackward(uint16_t x,uint16_t y,Rotation rotation){
    if(!commandBuffer.isFull()){
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_LOOKAT,
            x,
            y,
            0,                  //useless
            Direction::BACKWARD,
            rotation
        });
    }
    else{
        return -1;
    }
    return 0;
}

bool movement::setConsigneMaxSpeedLinear(uint16_t max_speed,uint16_t max_acceleration,uint16_t max_deceleration){
    if(!commandBuffer.isFull()){
        commandBuffer.push(Command{
            BaseCommand::MAX_SPEED_LINEAR,
            max_speed,
            max_acceleration,
            max_deceleration,
            Direction::BACKWARD,    //usless
            Rotation::NONE          //usless
        });
    }
    else{
        return -1;
    }
    return 0;
}

bool movement::setConsigneMaxSpeedAngular(uint16_t max_speed,uint16_t max_acceleration,uint16_t max_deceleration){
    if(!commandBuffer.isFull()){
        commandBuffer.push(Command{
            BaseCommand::MAX_SPEED_ANGULAR,
            max_speed,
            max_acceleration,
            max_deceleration,
            Direction::BACKWARD,    //usless
            Rotation::NONE          //usless
        });
    }
    else{
        return -1;
    }
    return 0;
}

bool movement::setConsigneStop(void){
    commandBuffer.resetTail();
    enableStop = true;
    return true;
}

bool movement::setConsignePause(void){
    enablePause = true;
    return true;
}

bool movement::setConsigneResume(void){
    enablePause = false;
    return true;
}


void movement::launchCommande(void){
    usartprintf("\nbaseCommand %s\n",baseCommandToString(currentCommand.baseCommand));
    usartprintf("x %d\n",currentCommand.x);
    usartprintf("y %d\n",currentCommand.y);
    usartprintf("theta %d\n",currentCommand.theta);
    usartprintf("direction %s\n",directionToChar(currentCommand.direction));
    usartprintf("rotation %s\n\n\n",rotationToChar(currentCommand.rotation));

    switch (currentCommand.baseCommand)
    {
    case BaseCommand::LINEAR:
        setProtectedConsigneLineaire(currentCommand.x,currentCommand.y);
        break;

    case BaseCommand::ANGULAR_LOOKAT:
        if(currentCommand.direction == Direction::FORWARD){
            Asservissement::setConsigneLookAtForward(currentCommand.x,currentCommand.y,currentCommand.rotation);
        }
        else{
            Asservissement::setConsigneLookAtBackward(currentCommand.x,currentCommand.y,currentCommand.rotation);
        }
        break;

    case BaseCommand::ANGULAR_THETA:
        setProtectedConsigneAngulaire(currentCommand.theta,currentCommand.rotation);
        break;

    case BaseCommand::MAX_SPEED_LINEAR:
        positionControlLineaire.vitesseMaxAv = currentCommand.x;
        positionControlLineaire.vitesseMaxAr = currentCommand.x;
        positionControlLineaire.accelerationMaxAv = currentCommand.y;
        positionControlLineaire.accelerationMaxAr = currentCommand.y;
        positionControlLineaire.decelerationMaxAv = currentCommand.theta;
        positionControlLineaire.decelerationMaxAv = currentCommand.theta;
        break;

    case BaseCommand::MAX_SPEED_ANGULAR:
        positionControlAngulaire.vitesseMaxAv = currentCommand.x;
        positionControlAngulaire.vitesseMaxAr = currentCommand.x;
        positionControlAngulaire.accelerationMaxAv = currentCommand.y;
        positionControlAngulaire.accelerationMaxAr = currentCommand.y;
        positionControlAngulaire.decelerationMaxAv = currentCommand.theta;
        positionControlAngulaire.decelerationMaxAv = currentCommand.theta;
        break;

    default:
        break;
    }
}

bool movement::currentCommandRun(void){
    if(currentCommand.baseCommand == BaseCommand::LINEAR){
        return getLinearError()!=0;
    }
    else{
        return getAngularError()!=0;
    }
}

void movement::loop(void){
    Asservissement::loop();

    if(enablePause && !pause){
        Asservissement::setConsigneStop();
        pause = true;
    }

    if(!enablePause && pause){
        launchCommande();
        pause = false;
    }

    if(!pause){
        if(!run && !commandBuffer.isEmpty()){
            currentCommand = commandBuffer.pop();
            launchCommande();
            run = true;
        }
        else if(run && !currentCommandRun()){
            if(!commandBuffer.isEmpty()){
                currentCommand = commandBuffer.pop();
                launchCommande();
            }
            else{
                run = false;
            }
        }
    }

    if(enableStop == true){
        enableStop = false;
        commandBuffer.resetHead();
        Asservissement::setConsigneStop();
    }
}

uint16_t movement::getCommandBufferSize(){
    return commandBuffer.getUsedSpace() + run;
}

movement::~movement(){
}