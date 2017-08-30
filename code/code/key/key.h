#ifndef _KEY_H_
#define _KEY_H_

#include "N76E003.h"

sbit KEY1 = P1^5;	//通道键
sbit KEY2 = P1^7;  //模式键
sbit KEY = P0^3;
sbit CON_RELAY = P0^4;

#define KEY3_595 (0X01)
#define KEY2_595 (0X02)
#define KEY1_595 (0X04)
#define KEY4_595 (0X08)

#define RELAY4_595	 (0X10)
#define RELAY3_595   (0X20)
#define RELAY2_595   (0X40)
#define RELAY1_595   (0X80)


void keyInit(void);

#endif

