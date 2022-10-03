#pragma once
#ifndef Tool_h
#define Tool_h

#include "time.h"
#include <stdlib.h>

void CPURandomInit() {
	srand(time(NULL));
}

float GetCPURandom() {
	return (float)rand() / (RAND_MAX + 1.0);
}

#endif


