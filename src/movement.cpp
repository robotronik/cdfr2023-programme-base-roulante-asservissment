#include "movement.h"
#include <atomic>

movement::movement(position* pos):Asservissement(pos){
}

bool movement::goToPoint(int16_t x,int16_t y,Rotation rotation, Direction direction){
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
            Direction::NONE,    //useless
            Rotation::NONE,     //useless
        });
    }
    else{
        return -1;
    }
    return 0;
}

bool movement::goToPoint(int16_t x,int16_t y,int16_t theta, Rotation rotationFirst, Direction direction, Rotation rotationSecond){
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
            Direction::NONE,    //useless
            Rotation::NONE,     //useless
        });
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_THETA,
            0,                  //useless
            0,                  //useless
            theta,
            Direction::NONE,    //useless
            rotationSecond
        });
    }
    else{
        return -1;
    }
    return 0;
}

bool movement::setConsigneAngulaire(int16_t angle,Rotation rotation){
    if(!commandBuffer.isFull()){
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_THETA,
            0,                  //useless
            0,                  //useless
            angle,
            Direction::NONE,    //useless
            rotation
        });
    }
    else{
        return -1;
    }
    return 0;
}

bool movement::setConsigneLookAt(int16_t x,int16_t y,Rotation rotation, Direction direction){
    if(!commandBuffer.isFull()){
        commandBuffer.push(Command{
            BaseCommand::ANGULAR_LOOKAT,
            x,
            y,
            0,                  //useless
            direction,
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
            Direction::NONE,        //usless
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
            Direction::NONE,        //usless
            Rotation::NONE          //usless
        });
    }
    else{
        return -1;
    }
    return 0;
}

bool movement::setPosition(int x, int y, int theta){
    if(!commandBuffer.isFull()){
        commandBuffer.push(Command{
            BaseCommand::SET_POSITION,
            x,
            y,
            theta,
            Direction::NONE,        //usless
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

void movement::printStatistic(void){
    if(currentCommand.baseCommand == BaseCommand::LINEAR){
        usartprintf("LINEAR STATISTIC :\n");
        usartprintf("Time : %lf\n",statisticLinear.getTime());
        usartprintf("Min Error : %lf\n",statisticLinear.getMinError());
        usartprintf("Max Error : %lf\n",statisticLinear.getMaxError());
        usartprintf("Integral : %lf\n",statisticLinear.getIntegral());
        usartprintf("Quadratic Integra : %lf\n",statisticLinear.getQuadraticIntegral());
        usartprintf("Derivate Integral : %lf\n\n\n",statisticLinear.getDerivateIntegral());
    }
    else if(currentCommand.baseCommand == BaseCommand::ANGULAR_THETA || currentCommand.baseCommand == BaseCommand::ANGULAR_LOOKAT){
        usartprintf("ANGULAR STATISTIC :\n");
        usartprintf("Time : %lf\n",statisticAngular.getTime());
        usartprintf("Min Error : %lf\n",statisticAngular.getMinError());
        usartprintf("Max Error : %lf\n",statisticAngular.getMaxError());
        usartprintf("Integral : %lf\n",statisticAngular.getIntegral());
        usartprintf("Quadratic Integra : %lf\n",statisticAngular.getQuadraticIntegral());
        usartprintf("Derivate Integral : %lf\n\n\n",statisticAngular.getDerivateIntegral());
    }
}

void movement::preComputeBuffer(void){
    int i = 0;
    position_t preComputePos = posRobot->getPosition();
    while (!commandBuffer.isEmpty(i)){
        Command* command = commandBuffer.read(i);
        switch (command->baseCommand)
        {
        case BaseCommand::LINEAR:
            preComputePos.x = currentCommand.x;
            preComputePos.y = currentCommand.y;
            break;

        case BaseCommand::ANGULAR_LOOKAT:
            preComputePos.teta = getLookAtAngle(preComputePos,currentCommand.x,currentCommand.y,currentCommand.direction,currentCommand.rotation);
            break;

        case BaseCommand::ANGULAR_THETA:
            preComputePos.teta = currentCommand.theta;
            break;


        case BaseCommand::SET_POSITION:
            preComputePos.x = currentCommand.x;
            preComputePos.y = currentCommand.y;
            preComputePos.teta = currentCommand.theta;
            break;

        default:
            break;
        }
    }
}

void movement::launchCommande(void){
    currentCommand = commandBuffer.pop();

    usartprintf("\nbaseCommand %s\n",baseCommandToString(currentCommand.baseCommand));
    usartprintf("x %d\n",currentCommand.x);
    usartprintf("y %d\n",currentCommand.y);
    usartprintf("theta %d\n",currentCommand.theta);
    usartprintf("direction %s\n",directionToChar(currentCommand.direction));
    usartprintf("rotation %s\n\n\n",rotationToChar(currentCommand.rotation));

    statisticAngular.reset();
    statisticLinear.reset();

    switch (currentCommand.baseCommand)
    {
    case BaseCommand::LINEAR:
        Asservissement::setConsigneLineaire(currentCommand.x,currentCommand.y);
        break;

    case BaseCommand::ANGULAR_LOOKAT:
        Asservissement::setConsigneAngulaire(currentCommand.x,currentCommand.y,currentCommand.direction,currentCommand.rotation);
        break;

    case BaseCommand::ANGULAR_THETA:
        Asservissement::setConsigneAngulaire(currentCommand.theta,currentCommand.rotation);
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

    case BaseCommand::SET_POSITION:{
        posRobot->setPosition(currentCommand.x,currentCommand.y,currentCommand.theta);
        position_t offsetCons = {(double)currentCommand.x,(double)currentCommand.y,(double)currentCommand.theta,0};
        setConsigne(offsetCons);
        break;
    }

    default:
        break;
    }
}

bool movement::currentCommandRun(void){
//slower chaining of actions with statistics to have a more consistent time
#ifdef ENABLE_STATISTIC
    if(currentCommand.baseCommand == BaseCommand::LINEAR){
        return robotRunning();
    }
    else{
        return robotTurning();
    }
#else
    if(currentCommand.baseCommand == BaseCommand::LINEAR){
        return getLinearError()!=0;
    }
    else if(currentCommand.baseCommand == BaseCommand::ANGULAR_LOOKAT){
        return getAngularError()!=0;
    }
    else if(currentCommand.baseCommand == BaseCommand::ANGULAR_THETA){
        return getAngularError()!=0;
    }
    return false;
#endif
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
            launchCommande();
            run = true;
        }
        else if(run && !currentCommandRun()){
#ifdef ENABLE_STATISTIC
            printStatistic();
#endif
            if(!commandBuffer.isEmpty()){
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