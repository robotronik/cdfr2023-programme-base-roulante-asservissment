#include "movement.h"

movement::movement():Asservissement(){
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

bool movement::commandRun(void){
    return run;
}

void movement::loop(void){
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

movement::~movement(){
}