#include "config.h"

const char* directionToChar(Direction dir) {
    switch (dir) {
        case Direction::NONE:
            return "NONE";
        case Direction::SHORTEST:
            return "SHORTEST";
        case Direction::FORWARD:
            return "FORWARD";
        case Direction::BACKWARD:
            return "BACKWARD";
        default:
            return "UNKNOWN";
    }
}


const char* rotationToChar(Rotation rot) {
    switch (rot) {
        case Rotation::NONE:
            return "NONE";
        case Rotation::SHORTEST:
            return "SHORTEST";
        case Rotation::ANTICLOCKWISE:
            return "ANTICLOCKWISE";
        case Rotation::CLOCKWISE:
            return "CLOCKWISE";
        default:
            return "UNKNOWN";
    }
}

const char* baseCommandToString(BaseCommand cmd) {
    switch (cmd) {
        case BaseCommand::ANGULAR_THETA:
            return "ANGULAR_THETA";
        case BaseCommand::ANGULAR_LOOKAT:
            return "ANGULAR_LOOKAT";
        case BaseCommand::LINEAR:
            return "LINEAR";
        case BaseCommand::MAX_SPEED_LINEAR:
            return "MAX_SPEED_LINEAR";
        case BaseCommand::MAX_SPEED_ANGULAR:
            return "MAX_SPEED_ANGULAR";
        case BaseCommand::SET_POSITION:
            return "SET_POSITION";
        default:
            return "UNKNOWN";
    }
}
