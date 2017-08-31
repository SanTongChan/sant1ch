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
	if(dev_def.dev_mode == DEV_REMOTE)//�����remoteģʽ
	{
		/*remoteģʽ key1�̰���Ч����key2�˳�*/
	}
	else if(dev_def.dev_mode == DEV_TIME)//���Ϊʱ��ģʽ
	{
		incJoggingTime();	//���ӵ㶯ʱ��
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
		if(dev_def.dev_mode == DEV_SELFLOCK)//���������
		{
			RfLedOffXt(20,20,2);//200ms��3�κ�Ϩ��
		}
		else if(dev_def.dev_mode == DEV_JOGGING)//����ǵ㶯
		{
			RfLedOnXt(20,20,2);//200ms��3�κ���
		}
		save_mode_flag = true;//��λ���水ť
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
	if(dev_def.dev_mode == DEV_TIME)//���������ʱ��ģʽ,�����˳�
	{
		dev_def.dev_mode = dev_def.last_dev_mode;//�ָ���ԭ��ģʽ
		if(dev_def.dev_mode == DEV_SELFLOCK)//���������
		{
			RfLedOffXt(20,20,2);//200ms��3�κ�Ϩ��
		}
		else if(dev_def.dev_mode == DEV_JOGGING)//����ǵ㶯
		{
			RfLedOnXt(20,20,2);//200ms��3�κ���
		}
		save_mode_flag = true;//��λ���水ť
	}
	else if(dev_def.dev_mode != DEV_REMOTE)//����remoteģʽ
	{
		dev_def.dev_mode = DEV_REMOTE;//��Ϊremoteģʽ
		//�����ֵ
		dev_def.remote_val = 0;//���remote_val
		RfLedBlink(10,10);//��100ms����
	}
}

static void key1_twice_press(void)
{
	if(dev_def.dev_mode == DEV_JOGGING)//���ԭ��Ϊ�㶯
	{
		dev_def.dev_mode = DEV_SELFLOCK;//�л�Ϊ����
		dev_def.last_dev_mode = DEV_SELFLOCK;
		CON_RELAY = 0;//�رռ̵���
		dev_def.update_flag = true;//�������
		RfLedOff();//Ϩ��
		save_mode_flag = true;//��λ���水ť
	}
	else if(dev_def.dev_mode == DEV_SELFLOCK)//���ԭ��Ϊ����
	{
		dev_def.dev_mode = DEV_JOGGING;//����Ϊ�㶯
		dev_def.last_dev_mode = DEV_JOGGING;
		CON_RELAY = 0;//�رռ̵���
		dev_def.update_flag = true;//�������
		RfLedOn();//��
		save_mode_flag = true;//��λ���水ť
	}
	else
	{
		/*do nothing��key1˫��ֻ�����л��㶯/����*/
	}
}
static void key2_twice_press(void)
{
	if(dev_def.dev_mode != DEV_REMOTE)//�����������ʱ��ģʽ
	{
		dev_def.dev_mode = DEV_TIME;//��Ϊ��������ʱ��ģʽ
		RfLedOnXt(20,20,2);//200ms��3�κ����
	}
	else
	{
		/*do nothing,Ŀǰֻ��������ʱ��*/
	}
}

static void registerKeys(void)
{
    keyRegisterSingle(key1_is_press,key1_short_press,key1_short_up_press,key1_long_press,NULL,key1_twice_press);
    keyRegisterSingle(key2_is_press,NULL,key2_short_up_press,key2_long_press,NULL,key2_twice_press);
}
void keyInit(void)
{
	P1M1 |= 0xA0;//��ʼ��Ϊ����
   	P1M2 &= 0X5f;
	P0M1 &= 0xE7;
   	P0M2 |= 0X18; 
	CON_RELAY = 0;
   	registerKeys();
}
