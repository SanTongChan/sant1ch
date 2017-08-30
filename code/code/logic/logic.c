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
bool deal_remote_flag = false;
bool key_scan_flag = false;
bool update_local_flag = false;
bool syn_app_flag = false;
bool led_flag = false;

#define  JOGGING_TIME   (7)

void incJoggingTime(void)
{
	if(dev_def.jogging_time == 7)
	{
		dev_def.jogging_time = 100;
	}
	else if(dev_def.jogging_time == 600)
	{
		dev_def.jogging_time = 600;
		return;
	}
	else
	{
		dev_def.jogging_time += 100;
	}
	RfLedOnXt(10,10,1);
}
void decJoggingTime(void)
{
	if(dev_def.jogging_time == 7)
	{
		dev_def.jogging_time = 7;
		return;
	}
	else if(dev_def.jogging_time == 100)
	{
		dev_def.jogging_time = 7;
	}
	else
	{
		dev_def.jogging_time -= 100;
	}
	RfLedOnXt(10,10,1);
}
static void modeInit(void)
{
	dev_def.dev_mode = DEV_SELFLOCK;
	dev_def.last_dev_mode = DEV_SELFLOCK;
	dev_def.jogging_time = 7;
	RfLedOff();
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
			if(dev_def.timer_cnt >= dev_def.jogging_time)
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
	if(led_flag)
	{
		led_flag = false;
		ledScan();
	}
}
void logicInit(void)
{
	modeInit();
	ledInit();
	keyInit();
	relayInit();
	RfLedOffXt(50,50,5);
#if 0
	captureInit();
	timer1Init();
#endif
    timer0Init();
}

