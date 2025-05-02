#pragma once

void usartSetup(void);

//Send information on uart2
//1000 char max
void usartprintf(const char* format,...);