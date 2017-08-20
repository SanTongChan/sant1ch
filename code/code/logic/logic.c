#include <intrins.h>
#include "logic.h"
#include "N76E003.h"
#include "key.h"
#include "led.h"
#include "timer.h"
#include "key_driver.h"
#include "flash.h"
#include "remote.h"

static uint16_t jogging_time = 700;

void incJoggingTime(void)
{
	if(jogging_time == 700)
	{
		jogging_time = 1000;
	}
	else if(jogging_time == 60000)
	{
		jogging_time = 60000;
	}
	else
	{
		jogging_time += 1000;
	}
}
void decJoggingTime(void)
{
	if(jogging_time == 700)
	{
		jogging_time = 700;
	}
	else if(jogging_time == 1000)
	{
		jogging_time = 700;
	}
	else
	{
		jogging_time -= 1000;
	}
}
