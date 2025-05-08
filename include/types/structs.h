#pragma once

#include "type.h"
#include "stdint.h"

#define PI 3.14159265359
#define DEG_TO_RAD PI/180
#define RAD_TO_DEG 180/PI

enum class BaseCommand {
    ANGULAR_THETA,
    ANGULAR_LOOKAT,
    LINEAR,
    MAX_SPEED_LINEAR,
    MAX_SPEED_ANGULAR
};

const char* directionToChar(Direction dir);
const char* rotationToChar(Rotation rot);
const char* baseCommandToString(BaseCommand cmd);

typedef struct{
	double x = 0;
    double y = 0;
	double a = 0;
	uint64_t time = 0;
}position_t;

typedef enum {
    /// @brief Success
    ret_OK = 0,

    /// @brief Fail
    ret_FAIL = -1,
} return_t;

typedef struct{
	int L = 0;
    int R = 0;
}motorSpeed_t;

typedef struct{
	int16_t x;
    int16_t y;
	int16_t a;
}positionint_t;

union position_u
{
	positionint_t position;
	uint8_t tab[6];
};

union uintConv
{
	int16_t num;
	uint8_t tab[2];
};