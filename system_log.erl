%%%-------------------------------------------------------------------
%%% @author clark
%%% @copyright (C) 2015, <COMPANY>
%%% @doc
%%%
%%% @end
%%% Created : 23. 十月 2015 下午2:42
%%%-------------------------------------------------------------------
-module(system_log).
-author("clark").
-compile(export_all).
-extends(gen_server_ex).


%% API
-export
([
    init/1
    ,push_log/1
    ,push_test_log/0
]).



-include_lib("common/src/com_log.hrl").
-include_lib("system_log.hrl").


-define(io_device, io_device).
-define(GAME_LOG_FILE, "game_log_file.log").
-define(push_log, push_log).


-record(state, {io_device}).

init(Args) ->
    ?INFO_LOG("event_server ~p",[Args]),
    event_server:sub_info(?push_log, system_log, info_push_log),
    event_server:sub_terminate(system_log, terminate),
    {ok, S} = file:open( ?GAME_LOG_FILE, [append, write,  {encoding, utf8}] ),
    erlang:put(?io_device, S),
    {ok, nil}.


push_test_log() ->
    ?MODULE ! {?push_log, #test_log{}}.


push_log(Log) ->
    ?MODULE ! {?push_log, Log}.


info_push_log(#test_log{} = Log) ->
%%     List = erlang:tuple_to_list(Log),
    ?INFO_LOG("~p|~ts",
        [
            Log#test_log.iNum,
            Log#test_log.vString
        ]),
    S = erlang:get(?io_device),
    io:format(S, "~p|~ts",
        [
            Log#test_log.iNum,
            unicode:characters_to_binary(Log#test_log.vString)
        ]),
    ok;

info_push_log(#role_login{} = Log) ->
%%     ?INFO_LOG("~p|~ws",
%%         [
%%             Log#test_log.iNum,
%%             Log#test_log.vString
%%         ]),
    ok.

terminate(_Reason, _State) ->
    S = erlang:get(?io_device),
    file:close(S).


%% add_log(Text, List) ->
%%     ?INFO_LOG(Text,[List]),
%%     S = erlang:get(?io_device),
%%     io:format(S, Text, [List]),
%%     ok.

%%     iEventId,           %%   游戏事件ID" size="60" type="string"/>
%%     iWorldId,           %%   游戏大区ID" type="uint" index="1"/>
%%     iUin,               %%   用户ID" size="60" type="string"/>
%%     dtEventTime,        %%   记录时间, 格式 YYYY-MM-DD HH:MM:SS" size="" type="datetime"/>
%%     iRoleId,            %%   角色ID" type="uint" index="1"/>
%%     vClientIp,          %%   客户端所在ip" size="16" type="string"/>
%%     vRoleName,          %%   角色名" size="20" type="string"/>
%%     iJobId,             %%   角色职业" size="" type="utinyint"/>
%%     iRoleLevel,         %%   角色等级" size="" type="uint"/>
%%     iRoleExp,           %%   角色经验值" size="" type="uint"/>
%%     iMoney,             %%   角色金币数" size="" type="uint"/>
%%     iLijin,             %%   角色绑钻数量" size="" type="uint"/>
%%     iGamePoints,        %%   角色钻石数量" size="" type="uint"/>
%%     dtCreateTime,       %%   角色创建的时间" size="" type="datetime"/>
%%     iOnlineTotalTime,   %%   角色总在线时长" size="" type="uint"/>
%%     iLoginWay,          %%   登录渠道" size="" type="uint"/>
%%     vMac,               %%   客户端mac地址" size="60" type="string"/>
%%     vDeviceID,          %%   客户端设备唯一标识" size="60" type="string"/>
%%     vDeviceInfo         %%   客户端设备型号信息" size="60" type="string"/>