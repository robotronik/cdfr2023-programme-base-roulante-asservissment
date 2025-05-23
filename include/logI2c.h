#pragma once
#include "config.h"
#include <string.h>

void logWrite(char* message, int size);
int getLogBufferSize(void);
void getLogBuffer(char** outBuffer, int& size);