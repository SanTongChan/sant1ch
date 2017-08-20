#ifndef __LED_H__
#define __LED_H__

#include "N76E003.h"

sbit RF_LED = P0^5;

void ledInit(void);

#endif
