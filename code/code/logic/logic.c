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
bool save_mode_flag = false;

#define  JOGGING_TIME   (7)

void incJoggingTime(void)
{
	if(dev_def.jogging_time == 7)//如果为700ms，增加为10s
	{
		dev_def.jogging_time = 100;
	}
	else if(dev_def.jogging_time == 600)//如果为60s，已经为最高，直接返回
	{
		return;
	}
	else//按一次，增加10S
	{
		dev_def.jogging_time += 100;
	}
	RfLedOnXt(10,10,1);//按一次，灯闪一次
}
void decJoggingTime(void)
{
	if(dev_def.jogging_time == 7)//最低直接返回
	{
		return;
	}
	else if(dev_def.jogging_time == 100)//如果为10s，减少为700ms
	{
		dev_def.jogging_time = 7;
	}
	else//其他每次减少10S
	{
		dev_def.jogging_time -= 100;
	}
	RfLedOnXt(10,10,1);//灯闪一次
}
void saveModeToFlash(void)
{
    if(save_mode_flag)
    {
		flash_data[0] = (uint8_t)dev_def.last_dev_mode;
		flash_data[1] = (uint8_t)(dev_def.remote_val >> 16);
		flash_data[2] = (uint8_t)(dev_def.remote_val >> 8);
		flash_data[3] = (uint8_t)dev_def.remote_val;
		flash_data[4] = (uint8_t)(dev_def.jogging_time >> 8);
		flash_data[5] = (uint8_t)(dev_def.jogging_time);
		flashWrite();
		save_mode_flag = false;
    }
}
static void modeInit(void)
{
	flashRead();
	dev_def.dev_mode = flash_data[0];
	dev_def.last_dev_mode = flash_data[0];
	dev_def.remote_val = 
			(uint32_t)flash_data[1] << 16
	        | (uint32_t)flash_data[2] << 8 
	        | flash_data[3];
	dev_def.jogging_time = 
			(uint16_t)flash_data[4] << 8
	        | flash_data[5];
	if(0xff == dev_def.dev_mode
		|| 0xffffff == dev_def.remote_val
		|| 0xffff == dev_def.jogging_time)
	{
		dev_def.dev_mode = DEV_SELFLOCK;
		dev_def.last_dev_mode = DEV_SELFLOCK;
		dev_def.jogging_time = 7;
		dev_def.remote_val = 0;
	}
	if(dev_def.dev_mode == DEV_SELFLOCK)
	{
		RfLedOff();
	}
	else if(dev_def.dev_mode == DEV_JOGGING)
	{
		RfLedOn();
	}
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
void dealRemoteStudy(void)
{
    if(dev_def.dev_mode == DEV_REMOTE)//在remote学习模式下
    {
        if(ir_data.cnt == 10)//已经连续接收到了相同的码值10次
        {
			if(dev_def.last_dev_mode == DEV_SELFLOCK)//如果是自锁
			{
				RfLedOffXt(30,30,4);//表示学习成功，灯300ms闪烁4次,最后灭
			}
			else if(dev_def.last_dev_mode == DEV_JOGGING)//如果是点动
			{
				RfLedOnXt(30,30,4);//表示学习成功，灯300ms闪烁4次,最后亮
			}
        }
        if(ir_data.cnt >= 10 && ir_data.timer_cnt >= 150)//松手超过300ms
        {
            dev_def.remote_val = ir_data.ir_data;//保存学习到的键值
            clearIrData();//清空遥控数据
            dev_def.dev_mode = dev_def.last_dev_mode;//退出remote模式，回到原来模式（jogging或selflock）
			save_mode_flag = true;//置位保存按钮
        }
    }
}
void dealRemoteNormal(void)
{
    static uint16_t cnt = 0;
    cnt++;
    if(dev_def.dev_mode != DEV_REMOTE && dev_def.dev_mode != DEV_TIME)//判断是在自锁或点动模式下
    {
		if(ir_data.ir_data == dev_def.remote_val && ir_data.cnt > 0)//接收到的键值相等且计数大于0
		{
			if(dev_def.dev_mode == DEV_SELFLOCK)//如果是自锁
			{
				RfLedOffXt(5,5,1);//50ms闪烁1次，最终灭
			}
			else if(dev_def.dev_mode == DEV_JOGGING)//如果是点动
			{
				RfLedOnXt(5,5,1);//50ms闪烁1次，最终亮
			}
		}
        if(ir_data.cnt == 1)//如果已经接收到了一个有效遥控值
        {

			if(ir_data.ir_data == dev_def.remote_val)//判断是否跟已经学习的相等
			{
				KEY = 0;//拉低按键
				cnt = 0;//清空计数器
			}
        }
        if(ir_data.timer_cnt >= 150 && ir_data.cnt >= 1
				&& ir_data.ir_data == dev_def.remote_val)//如果已经松手300ms，且是相等的
        {
			KEY = 1;   //拉高按键
            if(cnt <= 2500)//5s内，证明非配置
            {      
				CON_RELAY = !RELAY1;//控制继电器
				dev_def.update_flag = true;//置位允许更新标志
            }
            clearIrData();//清空遥控器的参数
        }
    }
}
void dealRemote(void)
{
	if(deal_remote_flag)
	{
		deal_remote_flag = false;
		analyzeRfData();
		dealRemoteStudy();
		dealRemoteNormal();
	}
}
void logicInit(void)
{
	ledInit();
	keyInit();
	relayInit();
	modeInit();
	captureInit();
	timer1Init();
    timer0Init();
}

