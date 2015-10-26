#pragma once

#include <../include/mmorpg/typedef.h>

struct IVividInstance
{
    virtual bool isDumbAI( ) const = 0;
    virtual void dumbAI(bool dumb) = 0;
};
