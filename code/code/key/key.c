#include "key.h"
#include "key_driver.h"
#include "led.h"
#include "logic.h"
#include "remote.h"


static bool key1_is_press(void)
{
    if(0 == KEY1)
    {
        return true;
    }
    return false;
}
static bool key2_is_press(void)
{
    if(0 == KEY2)
    {
        return true;
    }
    return false;
}

static void key1_short_press(void)
{
	if(dev_def.dev_mode == DEV_REMOTE)
	{
		
	}
	else if(dev_def.dev_mode == DEV_TIME)
	{

	}
	else
	{
		KEY = 0;
	}
}
static void key1_short_up_press(void)
{
	if(dev_def.dev_mode == DEV_REMOTE)
	{
		dev_def.dev_mode = dev_def.last_dev_mode;
		RfLedOff();
	}
	else if(dev_def.dev_mode == DEV_TIME)
	{
		incJoggingTime();	
	}
	else
	{
		CON_RELAY = !RELAY1;
		KEY = 1;
		dev_def.update_flag = true;
	}
}
static void key2_short_press(void)
{
	//RF_LED = !RF_LED;
}
static void key2_short_up_press(void)
{
	if(dev_def.dev_mode == DEV_REMOTE)
	{
		dev_def.dev_mode = dev_def.last_dev_mode;
		RfLedOff();
	}
	else if(dev_def.dev_mode == DEV_TIME)
	{
		decJoggingTime();
	}
}
static void key1_long_press(void)
{
	if(dev_def.dev_mode == DEV_REMOTE)
	{
		
	}
	else if(dev_def.dev_mode == DEV_TIME)
	{
		dev_def.dev_mode = dev_def.last_dev_mode;
		RfLedOff();
	}
	else
	{
		KEY = 1;
	}
}
static void key2_long_press(void)
{
	if(dev_def.dev_mode == DEV_TIME)
	{
		dev_def.dev_mode = dev_def.last_dev_mode;
		RfLedOff();
	}
	else if(dev_def.dev_mode != DEV_REMOTE)
	{
		dev_def.dev_mode = DEV_REMOTE;
		//清除码值
		RfLedBlink(10,10);
	}
}

static void key1_long_up_press(void)
{
	
}
static void key2_long_up_press(void)
{
	
}
static void key1_twice_press(void)
{
	if(dev_def.dev_mode == DEV_JOGGING)
	{
		dev_def.dev_mode = DEV_SELFLOCK;
		dev_def.last_dev_mode = DEV_SELFLOCK;
		CON_RELAY = 0;
		dev_def.update_flag = true;
	}
	else if(dev_def.dev_mode == DEV_SELFLOCK)
	{
		dev_def.dev_mode = DEV_JOGGING;
		dev_def.last_dev_mode = DEV_JOGGING;
		CON_RELAY = 0;
		dev_def.update_flag = true;
	}
	else
	{
		/*do nothing*/
	}
}
static void key2_twice_press(void)
{
	if(dev_def.dev_mode != DEV_REMOTE)
	{
		dev_def.dev_mode = DEV_TIME;
		RfLedOnXt(10,10,1);
	}
}

static void registerKeys(void)
{
    keyRegisterSingle(key1_is_press,key1_short_press,key1_short_up_press,key1_long_press,key1_long_up_press,key1_twice_press);
    keyRegisterSingle(key2_is_press,key2_short_press,key2_short_up_press,key2_long_press,key2_long_up_press,key2_twice_press);
}
void keyInit(void)
{
	P1M1 |= 0xA0;//初始化为输入
   	P1M2 &= 0X5f;
	P0M1 &= 0xE7;
   	P0M2 |= 0X18; 
	CON_RELAY = 0;
   	registerKeys();
}
