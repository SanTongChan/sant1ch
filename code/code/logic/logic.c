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
	if(dev_def.jogging_time == 7)//���Ϊ700ms������Ϊ10s
	{
		dev_def.jogging_time = 100;
	}
	else if(dev_def.jogging_time == 600)//���Ϊ60s���Ѿ�Ϊ��ߣ�ֱ�ӷ���
	{
		return;
	}
	else//��һ�Σ�����10S
	{
		dev_def.jogging_time += 100;
	}
	RfLedOnXt(10,10,1);//��һ�Σ�����һ��
}
void decJoggingTime(void)
{
	if(dev_def.jogging_time == 7)//���ֱ�ӷ���
	{
		return;
	}
	else if(dev_def.jogging_time == 100)//���Ϊ10s������Ϊ700ms
	{
		dev_def.jogging_time = 7;
	}
	else//����ÿ�μ���10S
	{
		dev_def.jogging_time -= 100;
	}
	RfLedOnXt(10,10,1);//����һ��
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
        updateLocal();//Ҫ��ͬ��APP֮��
    }
	if(led_flag)
	{
		led_flag = false;
		ledScan();
	}
}
void dealRemoteStudy(void)
{
    if(dev_def.dev_mode == DEV_REMOTE)//��remoteѧϰģʽ��
    {
        if(ir_data.cnt == 10)//�Ѿ��������յ�����ͬ����ֵ10��
        {
			if(dev_def.last_dev_mode == DEV_SELFLOCK)//���������
			{
				RfLedOffXt(30,30,4);//��ʾѧϰ�ɹ�����300ms��˸4��,�����
			}
			else if(dev_def.last_dev_mode == DEV_JOGGING)//����ǵ㶯
			{
				RfLedOnXt(30,30,4);//��ʾѧϰ�ɹ�����300ms��˸4��,�����
			}
        }
        if(ir_data.cnt >= 10 && ir_data.timer_cnt >= 150)//���ֳ���300ms
        {
            dev_def.remote_val = ir_data.ir_data;//����ѧϰ���ļ�ֵ
            clearIrData();//���ң������
            dev_def.dev_mode = dev_def.last_dev_mode;//�˳�remoteģʽ���ص�ԭ��ģʽ��jogging��selflock��
			save_mode_flag = true;//��λ���水ť
        }
    }
}
void dealRemoteNormal(void)
{
    static uint16_t cnt = 0;
    cnt++;
    if(dev_def.dev_mode != DEV_REMOTE && dev_def.dev_mode != DEV_TIME)//�ж�����������㶯ģʽ��
    {
		if(ir_data.ir_data == dev_def.remote_val && ir_data.cnt > 0)//���յ��ļ�ֵ����Ҽ�������0
		{
			if(dev_def.dev_mode == DEV_SELFLOCK)//���������
			{
				RfLedOffXt(5,5,1);//50ms��˸1�Σ�������
			}
			else if(dev_def.dev_mode == DEV_JOGGING)//����ǵ㶯
			{
				RfLedOnXt(5,5,1);//50ms��˸1�Σ�������
			}
		}
        if(ir_data.cnt == 1)//����Ѿ����յ���һ����Чң��ֵ
        {

			if(ir_data.ir_data == dev_def.remote_val)//�ж��Ƿ���Ѿ�ѧϰ�����
			{
				KEY = 0;//���Ͱ���
				cnt = 0;//��ռ�����
			}
        }
        if(ir_data.timer_cnt >= 150 && ir_data.cnt >= 1
				&& ir_data.ir_data == dev_def.remote_val)//����Ѿ�����300ms��������ȵ�
        {
			KEY = 1;   //���߰���
            if(cnt <= 2500)//5s�ڣ�֤��������
            {      
				CON_RELAY = !RELAY1;//���Ƽ̵���
				dev_def.update_flag = true;//��λ������±�־
            }
            clearIrData();//���ң�����Ĳ���
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

