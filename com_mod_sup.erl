-module(com_mod_sup).

-behaviour(supervisor).

%% API
-export([start_link/0]).


%% Supervisor callbacks
-export([init/1]).


start_link() ->
    supervisor:start_link({local, ?MODULE}, ?MODULE, []).


-define(CHILD(Mod, Type, Args), {Mod, {Mod, start_link, Args},
    permanent, 2000, Type, [Mod]}).

-define(CHILD_WORKER(ProcessMod, Name, Mod, Args), {Name, {ProcessMod, start_link, [{Name, Mod, Args}]},
    permanent, 2000, worker, [Mod]}).



%% @doc 开启各个功能模块
init([]) ->
    RestarStrategy = {one_for_one, 1, 2},
    {
        ok,
        {
            RestarStrategy,
            [
                ?CHILD(safe_ets, worker, [])
                , ?CHILD(com_prog, worker, [])
                , ?CHILD(world, worker, []) %% depend prog
                , ?CHILD(timer_manager, worker, [])      %活动管理器
                , ?CHILD(main_ins_team_mod, worker, [])
                , ?CHILD(auction_svr, worker, [])
                , ?CHILD(friend_gift_svr, worker, [])
                , ?CHILD(team_svr, worker, [])
                , ?CHILD(arena_svr, worker, [])
                , ?CHILD(ranking_lib, worker, [])
                , ?CHILD(card_svr, worker, [])
                , ?CHILD(guild_service, worker, [])         %公会公共数据
                , ?CHILD(pet_global_server, worker, [])     %宠物公共模块，维护变成宠物蛋的数据
                , ?CHILD(camp_service, worker, [])          %神魔系统公共数据
                , ?CHILD(sky_service, worker, [])           %天空之城公共数据
                , ?CHILD(title_service, worker, [])         %维护公共称号
                , ?CHILD(player_log_service, worker, [])    %玩家进程crash日志存储
                , ?CHILD_WORKER(event_server, system_log, system_log, [])            %玩家进程crash日志存储
                , ?CHILD_WORKER(event_server, info_log, info_log, [])            %玩家进程crash日志存储
            ]
        }
    }.
