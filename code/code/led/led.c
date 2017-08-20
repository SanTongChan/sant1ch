#include "led.h"

void ledInit(void)
{
    P0M1 &= 0xDf;
   	P0M2 |= 0X20; 
   	RF_LED = 0;
}



