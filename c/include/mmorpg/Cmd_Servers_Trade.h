#ifndef CMD_SERVERS_TRADE_H_INCLUDE_20121007
#define CMD_SERVERS_TRADE_H_INCLUDE_20121007
//数据库服务器的消息结构

#include "TradeItemInfo.h"

enum
{
    MSGSUBID_G2T_BUY_ITEM               = 50,
    MSGSUBID_G2T_BUY_ITEM_RESEND        = 51,     //玩家登录的时候发送上次未处理完成的消息
    MSGSUBID_G2T_SELL_ITEM              = 52,
    MSGSUBID_G2T_SELL_ITEM_RESEND       = 53,
    MSGSUBID_G2T_TAKE_STORAGE_ITEM      = 55,
    MSGSUBID_G2T_CANCEL_SELL_ITEM       = 56,

    MSGSUBID_T2G_BUY_ITEM_RESULT        = 50,
    MSGSUBID_T2G_SELL_ITEM_RESULT       = 52,
    MSGSUBID_T2G_TAKE_STORAGE_ITEM      = 55,
    MSGSUBID_T2G_TAKE_ALL_STORAGE_ITEMS = 56,
    MSGSUBID_T2G_TAKE_STORAGE_MONEY     = 57,        //取出的物品可能是金币和银币
};

#pragma pack(push,1)
struct MessageG2TBuyItem
{
    enum{
        MessageID = MSGSUBID_G2T_BUY_ITEM,
    };

    ObjectGuid  playerGuid;
    uint64      log_id;
    ObjectGuid  itemGuid;
    uint32      count;
    uint32      gold_money;
    uint32      silver_money;
};

struct MessageG2TSellItem
{
    enum{
        MessageID = MSGSUBID_G2T_SELL_ITEM,
    };

    ObjectGuid  player_guid;
    uint64      log_id;
    ObjectGuid  item_guid;
    ObjectGuid  item_static_guid;
    uint32      unit_gold_price;
    uint32      unit_silver_price;
    uint32      online_time;

    //物品详情
    TradeItemInfo   item_info;
};

struct MessageG2TCancelSellItem
{
    enum{
        MessageID = MSGSUBID_G2T_CANCEL_SELL_ITEM,
    };

    ObjectGuid  player_guid;
    uint64      log_id;
    ObjectGuid  item_guid;
};

struct MessageG2TTakeStorageItem
{
    enum{
        MessageID = MSGSUBID_G2T_TAKE_STORAGE_ITEM,
    };

    ObjectGuid  playerGuid;
    uint64      log_id;
    ObjectGuid  itemGuid;
};

struct MessageG2TTakeAllStorageItems
{
    ObjectGuid  playerGuid;
    uint32      slot_count;
};

struct MessageT2GBuyItemResult
{
    enum{
        MessageID = MSGSUBID_T2G_BUY_ITEM_RESULT,
    };

    ObjectGuid  player_guid;
    uint64      log_id;
    ObjectGuid  item_guid;
    uint32      result;
    uint32      glod_money;
    uint32      silver_money;
};
struct MessageT2GSellItemResult
{
    enum{
        MessageID = MSGSUBID_T2G_SELL_ITEM_RESULT,
    };
    ObjectGuid  player_guid;
    uint64      log_id;
    ObjectGuid  item_guid;
    uint32      result;
};
struct MessageT2GTakeItem
{
    enum{
        MessageID = MSGSUBID_T2G_TAKE_STORAGE_ITEM,
    };

    ObjectGuid  player_guid;
    uint64      log_id;
    ObjectGuid  item_guid;       //用这个guid创建物品
    ObjectGuid  item_static_guid;

    uint32      unit_gold_price;
    uint32      unit_silver_price;
    
    //物品详情
    TradeItemInfo   item_info;
};
struct MessageT2GTakeTradeMoney
{
    enum{
        MessageID = MSGSUBID_T2G_TAKE_STORAGE_MONEY,
    };
    ObjectGuid  player_guid;
    uint64      log_id;
    ObjectGuid  item_guid;       //出售的这个guid的物品
    ObjectGuid  item_static_guid;

    uint32      count;
    uint32      unit_gold_price;
    uint32      unit_silver_price;

    //物品详情
    TradeItemInfo   item_info;
};


struct MSGGS2TS_Header
{
    MSGGS2TS_Header(uint8 id):messageID(id){}
private:
    uint8  messageID;
};

template< class Message >
struct FullServerMessageImpl : private MSGGS2TS_Header, public Message
{
    FullServerMessageImpl() : MSGGS2TS_Header(Message::MessageID) {}
    template< class P1 >
    FullServerMessageImpl(P1 p1) : MSGGS2TS_Header(Message::MessageID), Message(p1) {}
    template< class P1, class P2 >
    FullServerMessageImpl(P1 p1, P2 p2) : MSGGS2TS_Header(Message::MessageID), Message(p1, p2) {}
    template< class P1, class P2, class P3 >
    FullServerMessageImpl(P1 p1, P2 p2, P3 p3) : MSGGS2TS_Header(Message::MessageID), Message(p1, p2, p3) {}
    template< class P1, class P2, class P3, class P4 >
    FullServerMessageImpl(P1 p1, P2 p2, P3 p3, P4 p4) : MSGGS2TS_Header(Message::MessageID), Message(p1, p2, p3, p4) {}

    const   uint8*      contents() const { return reinterpret_cast<const uint8*>(this); }
};

typedef FullServerMessageImpl<MessageG2TBuyItem>  MessageG2TBuyItem_full;
typedef FullServerMessageImpl<MessageG2TSellItem>  MessageG2TSellItem_full;
typedef FullServerMessageImpl<MessageG2TTakeStorageItem>  MessageG2TTakeStorageItem_full;
typedef FullServerMessageImpl<MessageG2TTakeStorageItem>  MessageG2TTakeStorageItem_full;
typedef FullServerMessageImpl<MessageG2TCancelSellItem>   MessageG2TCancelSellItem_full;

typedef FullServerMessageImpl<MessageT2GBuyItemResult>  MessageT2GBuyItemResult_full;
typedef FullServerMessageImpl<MessageT2GSellItemResult>  MessageT2GSellItemResult_full;
typedef FullServerMessageImpl<MessageT2GTakeItem>  MessageT2GTakeItem_full;
typedef FullServerMessageImpl<MessageT2GTakeTradeMoney>  MessageT2GTakeTradeMoney_full;
#pragma pack(pop)
#endif
