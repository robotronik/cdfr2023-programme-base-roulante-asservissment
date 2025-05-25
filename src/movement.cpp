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

double getDeltaAngle(double angleErreur, Rotation rotation){
    if(angleErreur>0 && rotation == Rotation::CLOCKWISE){
        angleErreur -= 360;
    }
    else if(angleErreur<0 && rotation == Rotation::ANTICLOCKWISE){
        angleErreur += 360;
    }
    return angleErreur;
}

double movement::findLinearMaxSpeedOut(void){
    if(currentCommand.baseCommand != BaseCommand::LINEAR){
        return 0;
    }

    position_t preComputePos = posRobot->getPosition();
    position_t prevPos;
    double deltaAngle[COMMANDE_BUFFER_SIZE];
    double deltaSize[COMMANDE_BUFFER_SIZE];
    double maxDeccel[COMMANDE_BUFFER_SIZE];
    double maxSpeed[COMMANDE_BUFFER_SIZE];
    double maxSpeedCurve[COMMANDE_BUFFER_SIZE];
    int vectorCount = -1;
    stateMovement currentStateVector = NONE;
    bool stop = false;
    double currentMaxSpeed = positionControlLineaire.vitesseMaxAv;
    double currentMaxDeccel = positionControlLineaire.decelerationMaxAv;
    Command* command = &currentCommand;
    int i = 0;

    do{
        prevPos = preComputePos;

        //compute MAX
        switch (command->baseCommand)
        {
        case BaseCommand::LINEAR:
            preComputePos.x = command->x;
            preComputePos.y = command->y;
            vectorCount ++;
            maxSpeed[vectorCount] = currentMaxSpeed;
            maxDeccel[vectorCount] = currentMaxDeccel;
            deltaSize[vectorCount] = hypot(prevPos.x - preComputePos.x, prevPos.y - preComputePos.y);
            if(vectorCount != 0){
                double dist = std::min(std::min(deltaSize[vectorCount-1],deltaSize[vectorCount]),50.0)/50.0;
                double angle = map(std::min(abs(deltaAngle[vectorCount-1]),20.0), 0, 20,300,0);
                maxSpeedCurve[vectorCount-1] = dist * angle;
            }
            currentStateVector = LINEAR;
            break;

        case BaseCommand::ANGULAR_LOOKAT:
            preComputePos.teta = getLookAtAngle(preComputePos,command->x,command->y,command->direction,command->rotation);
            deltaAngle[vectorCount] = getDeltaAngle(mod_angle(prevPos.teta - preComputePos.teta), command->rotation);
            currentStateVector = ANGULAR;
            break;

        case BaseCommand::ANGULAR_THETA:
            preComputePos.teta = command->theta;
            deltaAngle[vectorCount] = getDeltaAngle(mod_angle(prevPos.teta - preComputePos.teta), command->rotation);
            currentStateVector = ANGULAR;
            break;

        case BaseCommand::MAX_SPEED_LINEAR:
            currentMaxSpeed = command->x;
            currentMaxDeccel = command->theta;
            break;


        case BaseCommand::SET_POSITION:
            stop = true;
            break;

        default:
            break;
        }

        //get next command and check validity
        if(!commandBuffer.isEmpty(i))
            command = commandBuffer.read(i);
        else
            stop = true;
        if((command->baseCommand == BaseCommand::ANGULAR_LOOKAT || command->baseCommand == BaseCommand::ANGULAR_THETA) && currentStateVector == ANGULAR)
            stop = true;
        i++;
    }while(!stop);

    double currentSpeed = 0;
    for(int j = vectorCount; j >= 1; j--){
        currentSpeed = sqrt(currentSpeed*currentSpeed + 2*deltaSize[j]*maxDeccel[j]);
        currentSpeed = std::min(currentSpeed,maxSpeed[j]);
        //acceptable angle : 10deg per meter
        currentSpeed = abs(deltaSize[j] / deltaAngle[j-1])>100 ? currentSpeed : 0;
    }
    return currentSpeed;
}

void movement::launchCommande(void){
    currentCommand = commandBuffer.pop();
    bool chainedCommand = false;

    usartprintf("\nbaseCommand %s\n",baseCommandToString(currentCommand.baseCommand));
    usartprintf("x %d\n",currentCommand.x);
    usartprintf("y %d\n",currentCommand.y);
    usartprintf("theta %d\n",currentCommand.theta);
    usartprintf("direction %s\n",directionToChar(currentCommand.direction));
    usartprintf("rotation %s\n",rotationToChar(currentCommand.rotation));

    statisticAngular.reset();
    statisticLinear.reset();

    switch (currentCommand.baseCommand)
    {
    case BaseCommand::LINEAR:{
            double maxSpeedOutTmp = findLinearMaxSpeedOut();
            usartprintf("maxSpeedOut %lf\n",maxSpeedOutTmp);
            nextCommandIschained = maxSpeedOutTmp > 0 ? true : false;
            Asservissement::setConsigneLineaire(currentCommand.x,currentCommand.y,maxSpeedOutTmp);
        }
        break;

    case BaseCommand::ANGULAR_LOOKAT:
        chainedCommand = nextCommandIschained;
        nextCommandIschained = false;
        Asservissement::setConsigneAngulaire(currentCommand.x,currentCommand.y,currentCommand.direction,currentCommand.rotation);
        break;

    case BaseCommand::ANGULAR_THETA:
        chainedCommand = nextCommandIschained;
        nextCommandIschained = false;
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
    usartprintf("\n\n");
    if(!currentCommandRun() || chainedCommand){
        launchCommande();
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