#include "led.h"

LedDef led_def = {0};

void ledInit(void)
{
    P0M1 &= 0xDf;
   	P0M2 |= 0X20; 
	RF_LED = 0;
}
void ledScan(void)
{
	if(led_def.led_status == LED_BLINK)
	{
		led_def.cnt++;
		if(led_def.cnt >= led_def.led_blink_cnt)
		{
			if(RF_LED == 1)
			{
				led_def.led_blink_cnt = led_def.led_off_cnt;
				if(led_def.blink_off_stop)
				{
					led_def.blink_off_cnt++;
				}
			}
			else
			{
				led_def.led_blink_cnt = led_def.led_on_cnt;
				if(led_def.blink_on_stop)
				{
					led_def.blink_on_cnt++;
				}
			}
			RF_LED = !RF_LED;
			led_def.cnt = 0;
			if(led_def.blink_off_stop && led_def.blink_off_cnt >= led_def.led_blink_off_cnt)
			{
				led_def.led_status = LED_OFF;
			}
			if(led_def.blink_on_stop && led_def.blink_on_cnt >= led_def.led_blink_on_cnt)
			{
				led_def.led_status = LED_ON;
			}
		}
	}
}

void RfLedBlink(uint16_t blink_on_cnt,uint16_t blink_off_cnt)
{
	led_def.led_status = LED_BLINK;
	led_def.led_on_cnt = blink_on_cnt;
	led_def.led_off_cnt = blink_off_cnt;
	led_def.led_blink_on_cnt = 0;
	led_def.led_blink_off_cnt = 0;	
	led_def.blink_on_cnt = 0;
	led_def.blink_off_cnt = 0;
	led_def.blink_on_stop = 0;
	led_def.blink_off_stop = 0;
}
void RfLedOnXt(uint16_t blink_on_cnt,uint16_t blink_off_cnt,uint8_t time)
{
	led_def.led_status = LED_BLINK;
	led_def.led_on_cnt = blink_on_cnt;
	led_def.led_off_cnt = blink_off_cnt;
	led_def.led_blink_on_cnt = time;
	led_def.led_blink_off_cnt = 0;	
	led_def.blink_on_cnt = 0;
	led_def.blink_off_cnt = 0;
	led_def.blink_on_stop = 1;
	led_def.blink_off_stop = 0;
}
void RfLedOffXt(uint16_t blink_on_cnt,uint16_t blink_off_cnt,uint8_t time)
{
	led_def.led_status = LED_BLINK;
	led_def.led_on_cnt = blink_on_cnt;
	led_def.led_off_cnt = blink_off_cnt;
	led_def.led_blink_on_cnt = 0;
	led_def.led_blink_off_cnt = time;	
	led_def.blink_on_cnt = 0;
	led_def.blink_off_cnt = 0;
	led_def.blink_on_stop = 0;
	led_def.blink_off_stop = 1;
}
void RfLedOn(void)
{
	RF_LED = 1;
	led_def.led_status = LED_ON;
}

void RfLedOff(void)
{
	RF_LED = 0;
	led_def.led_status = LED_OFF;
}
