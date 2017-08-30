#ifndef __LED_H__
#define __LED_H__

#include "N76E003.h"
#include "typedef.h"
#define LED_NUM   (1)
#define LED_MAX_NUM   (1)

sbit RF_LED = P0^5;
typedef void (*LedProc)(void);
typedef enum
{
	LED_OFF = 0,
	LED_ON = 1,
	LED_BLINK = 2,
}LedStatus;

typedef struct
{
	//属于led的属性
	uint16_t led_on_cnt;
	uint16_t led_off_cnt;
	uint8_t led_blink_on_cnt;//闪烁次数控制
	uint8_t led_blink_off_cnt;//闪烁次数控制
	LedStatus led_status;
	LedProc led_proc;
	//属于led控制的属性
	uint16_t cnt;
	uint16_t led_blink_cnt;
	uint8_t blink_on_cnt;//闪烁次数控制
	uint8_t blink_off_cnt;//闪烁次数控制
	uint8_t blink_on_stop;
	uint8_t blink_off_stop;
}LedDef;

void ledInit(void);
void ledScan(void);
void RfLedBlink(uint16_t blink_on_cnt,uint16_t blink_off_cnt);
void RfLedOnXt(uint16_t blink_on_cnt,uint16_t blink_off_cnt,uint8_t time);
void RfLedOffXt(uint16_t blink_on_cnt,uint16_t blink_off_cnt,uint8_t time);
void RfLedOn(void);
void RfLedOff(void);
void setLedProc(LedProc led_proc);
#endif
