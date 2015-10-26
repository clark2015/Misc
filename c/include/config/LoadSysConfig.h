#ifndef LOADSYSCONFIG_H_INCLUDE_20121008__
#define LOADSYSCONFIG_H_INCLUDE_20121008__

#include <vector>
#include "config\LoadConfig.h"
#include "SysConfig.h"

namespace mmo_server
{

static bool LoadData(std::map<std::string, field>& csv, SysConfig* config)
{
    config->id                              = csv["ID"];
    config->value                           = csv["Value"];

    return true;
}

};
#endif //LOADSYSCONFIG_H_INCLUDE_20121008__