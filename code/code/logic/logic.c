#include <intrins.h>
#include "logic.h"
#include "N76E003.h"
#include "key.h"
#include "led.h"
#include "timer.h"
#include "key_driver.h"
#include "flash.h"
#include "remote.h"

DevDef xdata dev_def = {0};
static uint16_t jogging_time = 700;
bool deal_remote_flag = false;
bool key_scan_flag = false;
bool update_local_flag = false;
bool syn_app_flag = false;

#define  JOGGING_TIME   (7)

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
static void modeInit(void)
{
	dev_def.dev_mode = DEV_SELFLOCK;
}
static void relayInit(void)
{
	P0M1 |= 0X80;
	P0M2 &= 0x7f;
}
static void syncApp(void)
{
	static bool flag = false;
	if(dev_def.dev_mode == DEV_JOGGING)
	{
		dev_def.timer_cnt++;
		if(RELAY1 == 1)
		{
			if(false == flag)
			{
				flag = true;
				dev_def.timer_cnt = 0;
			}
			if(dev_def.timer_cnt >= JOGGING_TIME)
			{
				dev_def.timer_cnt = 0;
				flag = false;
				CON_RELAY = 0;
				dev_def.update_flag = true;
			}
		}
		else
		{
			flag = false;
		}
	}
	if(!dev_def.update_flag)
	{
		CON_RELAY = RELAY1;
	}
}
static void updateLocal(void)
{
	static bool high_flag = false;
	if(dev_def.update_flag)
	{
		if(RELAY1 != CON_RELAY && high_flag == false)
		{
			high_flag = true;
			KEY = 0;
		}
		else if(high_flag == true)
		{
			high_flag = false;
			KEY = 1;
		}
		else
		{
			dev_def.update_flag = false;
		}
	}
}
void dealLogic(void)
{
	if(key_scan_flag)
    {
        key_scan_flag = false;
        keyScan();
    }
	if(syn_app_flag)
	{
		syn_app_flag = false;
		syncApp();
	}
	if(update_local_flag)
    {
        update_local_flag = false;
        updateLocal();//要在同步APP之后
    }
}
void logicInit(void)
{
	modeInit();
	ledInit();
	keyInit();
	relayInit();
#if 0
	captureInit();
	timer1Init();
#endif
    timer0Init();
}

