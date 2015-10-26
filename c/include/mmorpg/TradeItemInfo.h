#pragma once

#pragma pack(push,1)
struct TradeItemInfo
{
    enum
    {
        SIZEOF_creator_name     = 17,
        NumOfSlot   = 3,
        NumOfEnchantment   = 3,
    };
    TradeItemInfo()
    {
        memset(this, 0, sizeof(TradeItemInfo));
    }

    unsigned int        item_id;
    unsigned int        flags;
    int                 stack_count;
    int                 durability;
    unsigned char       slot_color[NumOfSlot];
    unsigned char       pad;
    unsigned int        gem_id[NumOfSlot];
    unsigned long long  gem_guid[NumOfSlot];
    unsigned int        cram;
    unsigned int        enhancement;
    unsigned int        enchantment[NumOfEnchantment];
    char                creator_name[SIZEOF_creator_name];
};
#pragma pack(pop)