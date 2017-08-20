#include "key.h"
#include "key_driver.h"
#include "led.h"
#include "logic.h"
#include "remote.h"

sbit KEY1 = P1^5;	//模式键
sbit KEY2 = P1^7;  //通道键

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
#if 0
    if(dev_def.remote)
    {
        clearIrData();
        RF_LED = 1;
        dev_def.remote_channel = 1;
    }
    else
    {
        if(h595_val | KEY1_595)
        {
            h595_val = h595_val & (~KEY1_595);
    	    SendTo595(h595_val);
        }
    }
#endif
}
static void key2_short_press(void)
{
#if 0
    if(dev_def.remote)
    {
        clearIrData();
        RF_LED = 1;
        dev_def.remote_channel = 2;
    }
    else
    {
        if(h595_val | KEY2_595)
        {
            h595_val = h595_val & (~KEY2_595);
        	SendTo595(h595_val);	
    	}
	}
#endif
}
static void key1_short_up_press(void)
{
#if 0
    if(!dev_def.remote)
    {
        if(RELAY1)
        {
            h595_val &= (~RELAY1_595);
        }
        else
        {
            if(dev_def.lock)
            {
                h595_val &= 0x0f;
            }
            h595_val |= RELAY1_595;
        }
        h595_val |= KEY1_595;
        SendTo595(h595_val);
        if(dev_def.setting == true)
        {
            dev_def.setting = false;
            update_time = 10;
        }
        else
        {
            update_time = 5;
        }
        dev_def.dev_channel[0].update_flag = true;
        dev_def.dev_channel[1].update_flag = true;
        dev_def.dev_channel[2].update_flag = true;
        dev_def.dev_channel[3].update_flag = true;
        dev_def.update_local_cnt = 0;
    }
    else
    {
        RF_LED = 0;
    }
#endif
}
static void key2_short_up_press(void)
{
#if 0
    if(!dev_def.remote)
    {
      	if(RELAY2)
        {
            h595_val &= (~RELAY2_595);
        }
        else
        {
            if(dev_def.lock)
            {
                h595_val &= 0x0f;
            }
            h595_val |= RELAY2_595; 
        }
        h595_val |= KEY2_595;
        SendTo595(h595_val);
        if(dev_def.setting == true)
        {
            dev_def.setting = false;
            update_time = 10;
        }
        else
        {
            update_time = 5;
        }
        dev_def.dev_channel[0].update_flag = true;
        dev_def.dev_channel[1].update_flag = true;
        dev_def.dev_channel[2].update_flag = true;
        dev_def.dev_channel[3].update_flag = true;
        dev_def.update_local_cnt = 0;
    }
    else
    {
        RF_LED = 0;
    }
#endif
}


static void key1_long_press(void)
{
#if 0
    if(dev_def.remote)
    {
        dev_def.dev_channel[0].remote_val = 0;
        RF_LED = 0;
    }
#endif
}
static void key2_long_press(void)
{
#if 0
    if(dev_def.remote)
    {
        dev_def.dev_channel[1].remote_val = 0;
        RF_LED = 0;
    }
#endif
}

static void key1_long_up_press(void)
{
#if 0
    if(!dev_def.remote)
    {
        h595_val |= KEY1_595;
        update_time = 10;
        dev_def.setting = true;
        SendTo595(h595_val);
    }
#endif
}
static void key2_long_up_press(void)
{
#if 0
    if(!dev_def.remote)
    {
    	h595_val |= KEY2_595;
    	update_time = 10;
    	dev_def.setting = true;
    	SendTo595(h595_val);
	}
#endif
}
static void key1_twice_press(void)
{
#if 0
    if(dev_def.dev_channel[0].channel_mode == DEV_JOGGING)
    {
        dev_def.dev_channel[0].channel_mode = DEV_SELFLOCK;
    }
    else
    {
        dev_def.dev_channel[0].channel_mode = DEV_JOGGING;
    }
    dev_def.dev_channel[0].update_flag = true; 
    dev_def.update_local_cnt = 0;
    save_mode_flag = true;
#endif
}
static void key2_twice_press(void)
{
#if 0
    if(dev_def.dev_channel[1].channel_mode == DEV_JOGGING)
    {
        dev_def.dev_channel[1].channel_mode = DEV_SELFLOCK;
    }
    else
    {
        dev_def.dev_channel[1].channel_mode = DEV_JOGGING;
    }
    dev_def.dev_channel[1].update_flag = true; 
    dev_def.update_local_cnt = 0;
    save_mode_flag = true;
#endif
}

static void registerKeys(void)
{
    keyRegisterSingle(key1_is_press,key1_short_press,key1_short_up_press,key1_long_press,key1_long_up_press,key1_twice_press);
    keyRegisterSingle(key2_is_press,key2_short_press,key2_short_up_press,key2_long_press,key2_long_up_press,key2_twice_press);
}
void keyInit(void)
{
	P1M1 |= 0xA0;//初始化P10 -> P14为输入
   	P1M2 &= 0X5f;
   	registerKeys();
}
