#ifndef IVIVIDINSTANCEDATA_H_INCLUDE_20120425__
#define IVIVIDINSTANCEDATA_H_INCLUDE_20120425__

#include "functor.h"
#include "mmorpg\typedef.h"

struct IVividPlayer;
struct NPCPrototype;

////////////////////////////////////////////////////////////////
//class Instance
//{
//    bool KillMmonsterCredit(IVividPlayer* killer, const NPCPrototype* proto)
//    {
//        return false;
//    }
//}
//调用方法
//Instance instance
//SubKillMonsterCredit(&instance, &Instance::KillMmonsterCredit);

struct __declspec(novtable) IVividInstanceData
{
    //订阅玩家打死NPC时的奖励，处理成功返回true
    virtual void SubKillMonsterCredit(const mmo_server::Functor2<bool, IVividPlayer*, const NPCPrototype*>&);
    virtual void UnSubKillMonsterCredit(const mmo_server::Functor2<bool, IVividPlayer*, const NPCPrototype*>&);

    virtual void SubPlayerLevelUp(const mmo_server::Functor1<void, IVividPlayer*>&);
    virtual void UnSubPlayerLevelUp(const mmo_server::Functor1<void, IVividPlayer*>&);

    virtual void SubPlayerHealthModified(const mmo_server::Functor1<void, IVividPlayer*>&);
    virtual void UnSubPlayerHealthModified(const mmo_server::Functor1<void, IVividPlayer*>&);

    virtual void SubAddedItem(const mmo_server::Functor2<void, IVividPlayer*, uint32>&);
    virtual void UnSubAddedItem(const mmo_server::Functor2<void, IVividPlayer*, uint32>&);

    virtual void SubAddedSpell(const mmo_server::Functor2<void, IVividPlayer*, uint32>&);
    virtual void UnSubAddedSpell(const mmo_server::Functor2<void, IVividPlayer*, uint32>&);

    virtual void SubTakeQuest(const mmo_server::Functor2<void, IVividPlayer*, uint32>&);
    virtual void UnSubTakeQuest(const mmo_server::Functor2<void, IVividPlayer*, uint32>&);

    virtual void SubCompeleteQuest(const mmo_server::Functor2<void, IVividPlayer*, uint32>&);
    virtual void UnSubCompeleteQuest(const mmo_server::Functor2<void, IVividPlayer*, uint32>&);

    template <class T>
    inline  void SubKillMonsterCredit(T* _p, bool (T::*_f)(IVividPlayer*, const NPCPrototype*), const char* desc = NULL)
    {
        SubKillMonsterCredit(mmo_server::CreateFunctor(_p, _f, desc));
    }
    template <class T>
    inline  void UnSubKillMonsterCredit(T* _p, bool (T::*_f)(IVividPlayer*, const NPCPrototype*), const char* desc = NULL)
    {
        UnSubKillMonsterCredit(mmo_server::CreateFunctor(_p, _f, desc));
    }
};

#endif //IVIVIDINSTANCEDATA_H_INCLUDE_20120425__
