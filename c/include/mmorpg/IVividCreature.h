#ifndef IVIVIDCREATURE_H_INCLUDE_20120425__
#define IVIVIDCREATURE_H_INCLUDE_20120425__

#include "typedef.h"

//enum WeaponAttackType;

///////////////////////////////////////////////////////////////////////////
//
// 这里面的接口是给其他模块以及lua用的
//
///////////////////////////////////////////////////////////////////////////
class VividPlayer;
class VividNPC;
class VividInstance;
struct IGameCreature;
struct  IVividCreature
{
    virtual  float           GetPosX( ) const = 0;
    virtual  float           GetPosY( ) const = 0;
    virtual  float           GetOrientation( ) const = 0;
    virtual  uint32          GetLevel( ) const = 0;
    virtual  bool            isAlive( ) const = 0;
    virtual  void            TeleportTo(float x, float y) = 0;
    virtual  void            ReserveC1( ) { }
    virtual  void            ReserveC2( ) { }
    virtual  void            ReserveC3( ) { }
    virtual  void            ReserveC4( ) { }
};

struct IVividItem;
class Item;
class ByteBuffer;

struct  IVividPlayer : public IVividCreature
{
    virtual uint32      ChargeGoldMoney(uint32 value) = 0;
    virtual uint32      DestroyItemByID(uint32 id, uint32 count, uint32 reason, uint32 group) = 0;
    virtual Item*       AddBindItem(uint32 itemid, uint32 count, uint32 reason, uint32 group) = 0;
    
    virtual bool        IsBagFull( ) const = 0;
    virtual uint32      GetVipLevel( ) const = 0;
    virtual uint32      GetDailyDBData(uint32 index) const = 0;
    virtual void        SetDailyDBData(uint32 index, uint32 data) = 0;
};

struct NPCPrototype;
struct  IVividNPC : public IVividCreature
{
    virtual const NPCPrototype*   GetNPCPrototype() const = 0;
};

#endif //IVIVIDCREATURE_H_INCLUDE_20120425__
