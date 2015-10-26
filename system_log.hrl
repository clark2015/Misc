%%%-------------------------------------------------------------------
%%% @author clark
%%% @copyright (C) 2015, <COMPANY>
%%% @doc
%%%
%%% @end
%%% Created : 24. 十月 2015 下午12:43
%%%-------------------------------------------------------------------
-author("clark").

-record(test_log,
{
    iNum = 5,
    vString = "十月"
}).

-record(role_login,
{
    iEventId,           %%   游戏事件ID" size="60" type="string"/>
    iWorldId,           %%   游戏大区ID" type="uint" index="1"/>
    iUin,               %%   用户ID" size="60" type="string"/>
    dtEventTime,        %%   记录时间, 格式 YYYY-MM-DD HH:MM:SS" size="" type="datetime"/>
    iRoleId,            %%   角色ID" type="uint" index="1"/>
    vClientIp,          %%   客户端所在ip" size="16" type="string"/>
    vRoleName,          %%   角色名" size="20" type="string"/>
    iJobId,             %%   角色职业" size="" type="utinyint"/>
    iRoleLevel,         %%   角色等级" size="" type="uint"/>
    iRoleExp,           %%   角色经验值" size="" type="uint"/>
    iMoney,             %%   角色金币数" size="" type="uint"/>
    iLijin,             %%   角色绑钻数量" size="" type="uint"/>
    iGamePoints,        %%   角色钻石数量" size="" type="uint"/>
    dtCreateTime,       %%   角色创建的时间" size="" type="datetime"/>
    iOnlineTotalTime,   %%   角色总在线时长" size="" type="uint"/>
    iLoginWay,          %%   登录渠道" size="" type="uint"/>
    vMac,               %%   客户端mac地址" size="60" type="string"/>
    vDeviceID,          %%   客户端设备唯一标识" size="60" type="string"/>
    vDeviceInfo         %%   客户端设备型号信息" size="60" type="string"/>
}).