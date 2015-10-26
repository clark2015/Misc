#ifndef SYSCONFIG_H_INCLUDE_20121008__
#define SYSCONFIG_H_INCLUDE_20121008__

#include "../mmorpg/typedef.h"

struct SysConfig
{
    uint32 id;
    uint32 value;
};

enum
{
	SysConfig_TradeKeepingFee = 1,
	SysConfig_TradeTax        = 2,
};
#endif //LOADSPELLENTRY_H_INCLUDE_20120524__
