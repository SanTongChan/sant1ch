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
	if(dev_def.dev_mode == DEV_SELFLOCK || (dev_def.dev_mode == DEV_JOGGING))
	{
		KEY = 0;
	}
}
static void key1_short_up_press(void)
{
	if(dev_def.dev_mode == DEV_REMOTE)//如果是remote模式
	{
		/*remote模式 key1短按无效，仅key2退出*/
	}
	else if(dev_def.dev_mode == DEV_TIME)//如果为时间模式
	{
		incJoggingTime();	//增加点动时间
	}
	else
	{
		CON_RELAY = !RELAY1;
		KEY = 1;
		dev_def.update_flag = true;
	}
}
static void key2_short_up_press(void)
{
	if(dev_def.dev_mode == DEV_REMOTE)
	{
		dev_def.dev_mode = dev_def.last_dev_mode;
		if(dev_def.dev_mode == DEV_SELFLOCK)//如果是自锁
		{
			RfLedOffXt(20,20,2);//200ms闪3次后熄灭
		}
		else if(dev_def.dev_mode == DEV_JOGGING)//如果是点动
		{
			RfLedOnXt(20,20,2);//200ms闪3次后亮
		}
		save_mode_flag = true;//置位保存按钮
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
	if(dev_def.dev_mode == DEV_TIME)//如果是设置时间模式,长按退出
	{
		dev_def.dev_mode = dev_def.last_dev_mode;//恢复到原来模式
		if(dev_def.dev_mode == DEV_SELFLOCK)//如果是自锁
		{
			RfLedOffXt(20,20,2);//200ms闪3次后熄灭
		}
		else if(dev_def.dev_mode == DEV_JOGGING)//如果是点动
		{
			RfLedOnXt(20,20,2);//200ms闪3次后亮
		}
		save_mode_flag = true;//置位保存按钮
	}
	else if(dev_def.dev_mode != DEV_REMOTE)//不是remote模式
	{
		dev_def.dev_mode = DEV_REMOTE;//置为remote模式
		//清除码值
		dev_def.remote_val = 0;//清空remote_val
		RfLedBlink(10,10);//灯100ms快闪
	}
}

static void key1_twice_press(void)
{
	if(dev_def.dev_mode == DEV_JOGGING)//如果原先为点动
	{
		dev_def.dev_mode = DEV_SELFLOCK;//切换为自锁
		dev_def.last_dev_mode = DEV_SELFLOCK;
		CON_RELAY = 0;//关闭继电器
		dev_def.update_flag = true;//允许更新
		RfLedOff();//熄灭
		save_mode_flag = true;//置位保存按钮
	}
	else if(dev_def.dev_mode == DEV_SELFLOCK)//如果原先为自锁
	{
		dev_def.dev_mode = DEV_JOGGING;//更新为点动
		dev_def.last_dev_mode = DEV_JOGGING;
		CON_RELAY = 0;//关闭继电器
		dev_def.update_flag = true;//允许更新
		RfLedOn();//亮
		save_mode_flag = true;//置位保存按钮
	}
	else
	{
		/*do nothing，key1双击只用于切换点动/自锁*/
	}
}
static void key2_twice_press(void)
{
	if(dev_def.dev_mode != DEV_REMOTE)//如果不是设置时间模式
	{
		dev_def.dev_mode = DEV_TIME;//置为设置设置时间模式
		RfLedOnXt(20,20,2);//200ms闪3次后灯亮
	}
	else
	{
		/*do nothing,目前只用在设置时间*/
	}
}

static void registerKeys(void)
{
    keyRegisterSingle(key1_is_press,key1_short_press,key1_short_up_press,key1_long_press,NULL,key1_twice_press);
    keyRegisterSingle(key2_is_press,NULL,key2_short_up_press,key2_long_press,NULL,key2_twice_press);
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
