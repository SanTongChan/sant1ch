#ifndef __LOGIC_H__
#define __LOGIC_H__

#include "N76E003.h"
#include "typedef.h"

sbit RELAY1 = P0^7;
typedef enum
{
	DEV_SELFLOCK = 0,
	DEV_JOGGING = 1,
	DEV_REMOTE = 2,
	DEV_TIME = 3,
}DevMode;

typedef struct
{
	uint16_t timer_cnt;
	DevMode dev_mode;
	uint32_t remote_val;
	uint8_t lock;
	bool update_flag;
}DevDef;

extern xdata DevDef dev_def;
extern bool deal_remote_flag;
extern bool key_scan_flag;
extern bool update_local_flag;
extern bool syn_app_flag;
void logicInit(void);
void dealLogic(void);
#endif
