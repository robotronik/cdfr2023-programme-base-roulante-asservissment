#pragma once
#include "types/structs.h"

extern position_t pos;
extern position_t vel;
extern position_t acc;

void updatePositionData();

void setPosition(position_t inCommingposition);
void setPosition(double x, double y, double a);
bool getPositionChanged();