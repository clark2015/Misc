#ifndef GameGoodsInfo_H_INCLUDE_20111109__
#define GameGoodsInfo_H_INCLUDE_20111109__

#include "typedef.h"
#include <string>

enum GameGoodsTypes
{
    GAMEOBJECT_TYPE_TRANSPORT              = 0,
    GAMEOBJECT_TYPE_TRAP                   = 1,
    GAMEOBJECT_TYPE_BLOCKER                = 2,
    GAMEOBJECT_TYPE_ITEM                   = 3,
    GAMEOBJECT_TYPE_COUNT,
};

struct GameObjectPrototype
{
    uint32  GetID() const { return id; }
    uint32  id;
    uint32  type;
    uint32  displayId;
    const char*  name;
    union                                                   // different game object types have different data field
    {
        //0 GAMEOBJECT_TYPE_TRANSPORT
        struct
        {
            uint32           mapID;
            const char*      mapname;
            Position_t       x;
            Position_t       y;
        } transport;
        //1 GAMEOBJECT_TYPE_TRAP
        struct
        {
            uint32 spellId;                                 //1
            uint32 radius;                                  //2 radius for trap activation
            uint32 cooldown;                                //3 time in millsecs
        } trap;
        //2 GAMEOBJECT_TYPE_BLOCKER
        struct
        {
            Size_t   blocker;
            int32   health;                                 //
        } destructible_blocker;
        //3 GAMEOBJECT_TYPE_ITEM
        struct
        {
            uint32  itemid;                                 //
        } item;

        struct
        {
            char data[20];
        } raw;
    };
};

#endif //GameGoodsInfo_H_INCLUDE_20111109__
