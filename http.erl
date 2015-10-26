%%%-------------------------------------------------------------------
%%% @author dsl
%%% @copyright (C) 2015, <COMPANY>
%%% @doc
%%% 充值活动
%%% @end
%%% Created : 07. 三月 2015 下午2:43
%%%-------------------------------------------------------------------
-module(activities_recharge_mng).
-author("dsl").

-include_lib("config/include/config.hrl").
-include_lib("common/src/com_log.hrl").
-include("inc_all.hrl").
-include("player.hrl").
-include("player_mod.hrl").
-include("log.hrl").
-include("../mysql/include/common.hrl").

%% API
-export([cumsum_gold/1, do_log/4, init_status/1,add_prize_by_id/1,
  get_activity_end_time/0,get_ssa_type_by_id/1,get_prize_by_id/1]).

-define(CANT_GET, 0).     %% 不可领取
-define(CAN_GET, 1).      %% 可以领取
-define(HAD_GOT, 2).      %% 已领取
-define(TYPE_OF_RECHARGE_PERDAY, 4).    %% 每日充值的类型
-define(TYPE_OF_RECHARGE_RETURN, 5).    %% 充值返利的类型

-record(player_ar_tab, {
    id,
    num=0,                               %% 最大的单次充值元宝
    total=0,                             %% 充值总数
    ar_list=[],                          %% [{活动id， get_statue}] %% 领取状态，0 不可领取，1可领取，2领取过
    today_recharge = {0, 0},             %% 今天充值总数{Timestamp, Num}
    recharge_perday_id = 2,              %% 每日充值活动id
    recharge_prize_state = ?CANT_GET,    %% 每次充值奖励状态
    perday_recharge = [],                %% 每日充值奖励状态 [{Id, Timestamp}]
    recharge_return_get_prize_list = []  %% 充值返利领取奖励列表
}).


-record(activities_recharge_cfg, {
      id,
      type,
      time,
      hour,
      value,
      prize
    }).


open_ar() ->
    Ar = get(?pd_ar_list),
    Bin = %%
    lists:foldl(fun({Id, Statue}, Acc) ->
        <<Acc/bytes, Id, 0:64, Statue>>
    end,
    <<(length(Ar))>>,
    Ar),
    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_TOTAL, (get(?pd_ar_total)):32>>),
    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_OPEN, Bin/bytes>>).

%% 累加元宝
cumsum_gold(Num) ->
    N = get(?pd_ar_total) + Num,
    put(?pd_ar_total, N),
    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_TOTAL, N:32>>),
    Max = get(?pd_ar_one_max),
    if
        Num > Max ->
            put(?pd_ar_one_max, Num);
        true ->
            pass
    end,
    List = get(?pd_ar_list),
    %%today_chongzhi(Num),%% todo
    lists:foreach(fun({Id, _}) ->
        case get_ar_cfg(Id) of
            ?none ->
                pass;
            Ar ->
                Type = Ar#activities_recharge_cfg.type,
                _Time = Ar#activities_recharge_cfg.time,
                {T, V} = Ar#activities_recharge_cfg.value,
                if
                    Type =:= 1 ->
                        judge_ar(T, V, Max, Id),
                        todo;
                    Type =:= 2 ->
                        todo;
                    Type =:= 3 ->
                        judge_ar(T, V, N, Id);
                    true ->
                        pass
                end
        end
    end,
    List).

%% 判断活动是否能达到可领取的条件
%% 第一个参数  是type
%% 第2个参数   是需要达到的数值
%% 第3个参数   是当前的数值
%% 第4个参数   是当前活动的id
judge_ar(1, _Value, _CurrentV, _Id) ->
    todo;
judge_ar(2,Value, CurrentV, Id) ->
    if
        CurrentV >= Value ->
            chage_get_statue(Id);
        true ->
            pass
    end;
judge_ar(3, _Value, _CurrentV, _Id) ->
    todo;
judge_ar(4, _Value, _CurrentV, _Id) ->
    todo;
judge_ar(5, Value, CurrentV, Id) ->
    if
        CurrentV > Value ->
            chage_get_statue(Id);
        true ->
            pass
    end.

chage_get_statue(Id) ->
    List = get(?pd_ar_list),
    case lists:keysearch(Id, 1, List) of
        ?false ->
            pass;
        {value, {_, 0}} ->
            put(?pd_ar_list, lists:keyreplace(Id, 1, List, {Id, 1}));
        _ ->
            pass
    end.

get_activity_end_time() ->
    CfgIdList = lookup_all_activities_recharge_cfg(#activities_recharge_cfg.id),
    TypeList = lists:seq(7,13),
    lists:foldl(
        fun(Id, List) ->
            case  lookup_activities_recharge_cfg(Id) of
                ?none ->
                    List;
                Cfg ->
                    Time = Cfg#activities_recharge_cfg.time,
                    {H, F, M} = Cfg#activities_recharge_cfg.hour,
                    Type = Cfg#activities_recharge_cfg.type,
                    case lists:member(Type, TypeList) of
                        ?true ->
                            EndTime = (Time - 2) * 86400 + com_time:get_seconds_to_next_day() + (H * 3600 + F* 60 + M),
                            case lists:keyfind(Type, 1, List) of
                                ?false ->
                                    [{Type, EndTime+com_time:now()}|List];
                                _ ->
                                    List
                            end;
                        ?false ->
                            List
                    end
            end
    end,
    [],
    CfgIdList).

get_ssa_type_by_id(Id) ->
    case  lookup_activities_recharge_cfg(Id, #activities_recharge_cfg.type) of
        ?none ->
            ?none;
        Type ->
            Type
    end.

init_status(Type) ->
    CfgIdList = lookup_all_activities_recharge_cfg(#activities_recharge_cfg.id),
    lists:foldl(
        fun(Id, List) ->
            case  lookup_activities_recharge_cfg(Id) of
                ?none ->
                    List;
                Cfg ->
                    case Cfg#activities_recharge_cfg.type of
                        Type when Type > 0 ->
                            {_, Gold} = Cfg#activities_recharge_cfg.value,
                            [{Id, Gold, 0} | List];
                        _ ->
                            List
                    end
            end
        end,
    [],
    CfgIdList).

init_ar() ->
    CfgIdList = lookup_all_activities_recharge_cfg(#activities_recharge_cfg.id),
    lists:foldl(
        fun(Id, List) ->
                case get_ar_cfg_type_of_today(Id) of
                    ?true ->
                        %?DEBUG_LOG("Id-------------------------:~p",[Id]),
                        [{Id, 0} | List];
                    ?false ->
                        List
                end
        end,
        [],
        CfgIdList
    ).
    % lists:foldl(fun({Key, _Ar}, List) ->
    %       [{Key,0}|List]
    % end,
    % [],
    % ets:tab2list(activities_recharge_cfg)).
%% 获取是不是类型是3和6
get_ar_cfg_type_of_today(Id) ->
    case lookup_activities_recharge_cfg(Id, #activities_recharge_cfg.type) of
        ?none ->
            ?false;
        3 ->
            ?true;
        6 ->
            ?true;
        _ ->
            ?false
    end.

%% 获取是不是类型是6类型
get_ar_cfg_type_of_six(Id) ->
    case lookup_activities_recharge_cfg(Id, #activities_recharge_cfg.type) of
        ?none ->
            ?false;
        6 ->
            ?true;
        _ ->
            ?false
    end.


get_ar_cfg(Id) ->
    case lookup_activities_recharge_cfg(Id) of
        ?none ->
            ?none;
        Cfg ->
            Cfg
    end.

get_prize_by_id(Id) ->
    case get_ar_cfg(Id) of
        ?none ->
            ?none;
        Cfg ->
            Prize = Cfg#activities_recharge_cfg.prize,
            if
                is_integer(Prize) ->
                   Prize;
                true ->
                    element(1, Prize)
            end
    end.


add_prize_by_id(Id) ->
    case get_ar_cfg(Id) of
        ?none ->
            pass;
        Cfg ->
            Prize = Cfg#activities_recharge_cfg.prize,
            PrizeId =
                if
                    is_integer(Prize) ->
                        Prize;
                    true ->
                        element(get(?pd_job), Prize)
                end,
            prize:get_prize(PrizeId, ?SSA_PRIZE)
    end.

%% 每日单笔充值大返利--------------------------------------------------------------------------------------------------start
today_chongzhi(Num) ->
    %?DEBUG_LOG("Num---------------------------:~p",[Num]),
    List = get(?pd_ar_list),
    %?DEBUG_LOG("List-----------------------------:~p",[List]),
    CfgIdList = lookup_all_activities_recharge_cfg(#activities_recharge_cfg.id),
    NewValueList =
    lists:foldl(
        fun(Id, List2) ->
            case get_ar_cfg_type_of_six(Id) of
                ?true ->
                    case lookup_activities_recharge_cfg(Id, #activities_recharge_cfg.value) of
                        ?none ->
                            List2;
                        {3, Value} ->
                            [{Value, Id}|List2];
                        _ ->
                            List2
                    end;
                ?false ->
                    List2
            end
        end,
    [],
    CfgIdList),
    %?DEBUG_LOG("NewValueList--------------------------:~p",[NewValueList]),
    case lists:keysearch(Num, 1, NewValueList) of
        ?false ->
            pass;
        {?value, {_Value2, Id2}} ->
            case lists:keysearch(Id2, 1 , List) of
                ?false ->
                    todo;
                {?value, {Id3, Count}} ->
                    NewCount = Count + 1,
                    L = lists:keyreplace(Id3, 1, List, {Id3, NewCount}),
                    %?DEBUG_LOG("L------------------------------:~p",[L]),
                    put(?pd_ar_list, L),
                    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_TODAY_CHONGZHI, Id3, NewCount>>)
            end
    end.

open_today_chongzhi() ->
    List = get(?pd_ar_list),
    %?DEBUG_LOG("open List-------------:~p",[List]),
    CfgIdList = lookup_all_activities_recharge_cfg(#activities_recharge_cfg.id),
    NewCfgList =
        lists:foldl(
            fun(Id, List2) ->
                case get_ar_cfg_type_of_six(Id) of
                    ?true ->
                        [Id|List2];
                    ?false ->
                        List2
                end
            end,
        [],
        CfgIdList),
    {Bin, Size} =
    lists:foldl(
        fun({Id, Count}, {Binary, Num}) ->
            case lists:member(Id, NewCfgList) of
                ?true ->
                    %?DEBUG_LOG("Id-------------------:~p----Count--:~p",[Id, Count]),
                    {<<Binary/bytes, Id, Count>>, Num+1};
                ?false ->
                    {Binary, Num}
            end
        end,
    {<<>>,0},
    List),
    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_TODAY_CHONGZHI_OPEN, Size, Bin/bytes>>).

get_today_chongzhi_prize(Id) ->
    case get_ar_cfg(Id) of
        ?none ->
            pass;
        Cfg ->
            List = get(?pd_ar_list),
            case lists:keysearch(Id, 1, List) of
                ?false ->
                    pass;
                {value, {_, 0}} ->
                    pass;
                {value, {_, Count}} ->
                    Prize = Cfg#activities_recharge_cfg.prize,
                    PrizeId =
                        if
                            is_integer(Prize) ->
                                Prize;
                            true ->
                                element(get(?pd_job), Prize)
                        end,
                    prize:get_prize(PrizeId, ?GET_RECHARGE_PRIZE),
                    NewCount = Count - 1,
                    L = lists:keyreplace(Id, 1, List, {Id, NewCount}),
                    put(?pd_ar_list, L),
                    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_TODAY_CHONGZHI_GET_PRIZE_OK, Id, NewCount>>);
                _ ->
                    pass
            end
    end.



%% today_chongzhi_(ArList , [], _Gold) ->
%%     ArList;
%%
%% today_chongzhi_(ArList , [{Value, Id}|T2], Gold) ->
%%     if
%%         Gold > Value ->
%%             NewArlist = lists:keyreplace(Id, 1, ArList, {Id, 1}),
%%             today_chongzhi_(NewArlist, T2, Gold);
%%         true ->
%%             today_chongzhi_(NewArlist, T2, Gold)
%%     end.

%% 每日单笔充值大返利--------------------------------------------------------------------------------------------------end


get_ar_prize(Id) ->
    case get_ar_cfg(Id) of
        ?none ->
            pass;
        Cfg ->
            List = get(?pd_ar_list),
            %?DEBUG_LOG("List--------------------:~p",[List]),
            case lists:keysearch(Id, 1, List) of
                ?false ->
                    %?DEBUG_LOG("false---------------------------"),
                    pass;
                {value, {_, 0}} ->
                    %?DEBUG_LOG("-----------------------------------0"),
                    pass;
                {value, {_, 1}} ->
                    %?DEBUG_LOG("--------------------------------1"),
                    Prize = Cfg#activities_recharge_cfg.prize,
                    PrizeId =
                    if
                        is_integer(Prize) ->
                           Prize;
                        true ->
                           element(get(?pd_job), Prize)
                    end,
                    prize:get_prize(PrizeId, ?GET_RECHARGE_PRIZE),
                    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_GET_PRIZE_OK, Id>>),
                    world:broadcast(?to_client_msg(<<?MSG_S2C_SCENE_BROADCAST_ACHIEVEMENT, 126, ?pkg_sstr(get(?pd_name)), PrizeId:32>>)),
                    L = lists:keyreplace(Id, 1, List, {Id, 2}),
                    put(?pd_ar_list, L);
                {value, {_, 2}} ->
                    %?DEBUG_LOG("---------------------------------2"),
                    pass
            end
    end.

get_prize_status(Id) ->
    List = get(?pd_ar_list),
    case lists:keysearch(Id, 1, List) of
        ?false ->
            0;
        {value, {_, Statue}} ->
            Statue
    end.

open_shouchong(Id) ->
    Statue = get_prize_status(Id),
    PrizeId =
    case get_ar_cfg(Id) of
        ?none ->
            0;
        Cfg ->
            Type = Cfg#activities_recharge_cfg.type,
            if
                Type =:= 3 -> %% 3 表示是首冲
                    PrizeTuple = Cfg#activities_recharge_cfg.prize,
                    element(get(?pd_job), PrizeTuple);
                true ->
                    0
            end
    end,
    %%?DEBUG_LOG("PRizeId------------------------:~p",[PrizeId]),
    if
        PrizeId =/= 0 ->
            ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_SHOUCHONG_OPEN, Statue, PrizeId:32>>);
        true ->
            pass
    end.

mod_id() -> ?PM_ACTIVITIES_RECHARGE_MNG.

%% 玩家第一次登陆是调用
create_mod_data(SelfId) ->
    case quickdb:insert_new(?player_ar_tab,SelfId, #player_ar_tab{id=SelfId, ar_list=init_ar()},?FILE,?MODULE,?LINE) of
        ?true ->
            ok;
        ?false ->
            ?ERROR_LOG("player ~p create new player_ar_tab not alread exists ", [SelfId])
    end,
    ok.

load_mod_data(PlayerId) ->
    case quickdb:load_data(?player_ar_tab, PlayerId,?FILE,?MODULE,?LINE) of
        [] ->
            create_mod_data(PlayerId),
            load_mod_data(PlayerId);
        [#player_ar_tab{total=Total, ar_list=List, num=Num, today_recharge=Tuple, recharge_perday_id=RechargePerdayId, recharge_prize_state=State, perday_recharge=RechargeList, recharge_return_get_prize_list=GetPrizeList}] ->
            ?pd_new(?pd_ar_total, Total),
            ?pd_new(?pd_ar_list, List),
            ?pd_new(?pd_ar_one_max, Num),
            ?pd_new(?pd_today_recharge, Tuple),
            ?pd_new(?pd_recharge_perday_id, RechargePerdayId),
            ?pd_new(?pd_recharge_prize_state, State),
            ?pd_new(?pd_perday_recharge, RechargeList),
            ?pd_new(?pd_recharge_return_get_prize_list, GetPrizeList),
            set_activities_recharge_endtime()
    end,
    ok.

init_client(Acc) ->
    Acc.

view_data(Acc) ->
    Acc.

online() ->
    Id = get(?pd_recharge_perday_id),
    State = get(?pd_recharge_prize_state),
    case player:is_daliy_first_online() of
        true ->
            put(?pd_today_recharge, {com_time:now(), 0}),
            put(?pd_recharge_return_get_prize_list, []),
            case State =:= ?HAD_GOT of
                true ->
                    put(?pd_recharge_perday_id, Id + 1),
                    put(?pd_recharge_prize_state, ?CANT_GET);
                _ ->
                    pass
            end;
        _ ->
            pass
    end,
    send_recharge_perday_info(),
    send_recharge_return_info(),
    ok.

offline(PlayerId) ->
    save_data(PlayerId).

save_data(PlayerId) ->
    %?DEBUG_LOG("offline ar list------------------------:~p",[get(?pd_ar_list)]),
    quickdb:update(?player_ar_tab, PlayerId,#player_ar_tab{
            id=PlayerId,
            num=get(?pd_ar_one_max),
            total=get(?pd_ar_total),
            ar_list=get(?pd_ar_list),
            today_recharge=get(?pd_today_recharge),
            recharge_perday_id=get(?pd_recharge_perday_id),
            recharge_prize_state=get(?pd_recharge_prize_state),
            perday_recharge=get(?pd_perday_recharge),
            recharge_return_get_prize_list=get(?pd_recharge_return_get_prize_list)
    },?FILE,?MODULE,?LINE),
    ok.

handle_frame(?frame_flush_data) ->
    PlayerId = get(?pd_id),
    save_data(PlayerId),
    ok;


handle_frame(_) ->
  ok.

handle_client(?MSG_C2S_ACTIVITIES_RECHARGE_OPEN, <<>>) ->
    ?debughuangzuan("MSG_C2S_ACTIVITIES_RECHARGE_OPEN "),
    open_ar();

handle_client(?MSG_C2S_ACTIVITIES_RECHARGE_GET_PRIZE, <<Id>>) ->
    get_ar_prize(Id);

handle_client(?MSG_C2S_ACTIVITIES_RECHARGE_SHOUCHONG_OPEN, <<>>) ->
    ?debughuangzuan("MSG_C2S_ACTIVITIES_RECHARGE_SHOUCHONG_OPEN"),
    open_shouchong(1);

handle_client(?MSG_C2S_ACTIVITIES_RECHARGE_TOTAL, <<>>) ->
    ?debughuangzuan("MSG_C2S_ACTIVITIES_RECHARGE_TOTAL"),
    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_TOTAL, (get(?pd_ar_total)):32>>);

handle_client(?MSG_C2S_ACTIVITIES_RECHARGE_SEND_MONEY_TO_QQ, <<BuygoldId:16>>) ->   %向腾讯充值的消息
    ?debughuangzuan("MSG_C2S_ACTIVITIES_RECHARGE_SEND_MONEY_TO_QQ"),
    platformaward_mng:send_qq_money(BuygoldId);

%% 领取每日充值奖励
handle_client(?MSG_C2S_ACTIVITIES_RECHARGE_GET_RECHARGE_PERDAY_PRIZE, <<>>) ->
    get_recharge_perday_prize();

%% 领取充值返利奖励
handle_client(?MSG_C2S_ACTIVITIES_RECHARGE_GET_RECHARGE_RETURN_PRIZE, <<Id:8>>) ->
    get_recharge_return_prize(Id);

handle_client(?MSG_C2S_ACTIVITIES_RECHARGE_TODAY_CHONGZHI_OPEN, <<>>) ->
    open_today_chongzhi();

handle_client(?MSG_C2S_ACTIVITIES_RECHARGE_TODAY_CHONGZHI_GET_PRIZE, <<Id>>) ->
    ?DEBUG_LOG("Id--------------------------------:~p",[Id]),
    get_today_chongzhi_prize(Id);

handle_client(Mod, Msg) ->
  ?ERROR_LOG("no known msg Mod:~p Msg:~p", [Mod, Msg]).

%充值添加元宝的消息 todo 做服务器的检测等。。。
handle_msg(_FromMod, {?recharge_add_yuanbao_msg,_Serverid,_Appkey,Payitem,_Token,_Ts,Billno,Amt,_Sign}) ->
    ?debughuangzuan("recharge_add_yuanbao_msg suc"),
    ListPayItem=Payitem,    % Payitem 包含的内容是 物品id*物品q价格*物品数量
    ItemList=string:tokens(ListPayItem,"*"),
    ?INFO_LOG("recharge_add_yuanbao_msg PlayerId=~p ListPayItem=~p",[get(?pd_id),ListPayItem]),
    ?debughuangzuan("recharge_add_yuanbao_msg suc ItemList=~p",[ItemList]),
    LengthCur=length(ItemList),
    if
        LengthCur =:= 3 ->
            ListYuanBao=lists:nth(2,ItemList),
            ListGeShu=lists:nth(3,ItemList),
            GeShuNum=list_to_integer(ListGeShu),
            YuanBaoNum=list_to_integer(ListYuanBao),
            TotalNum=round(YuanBaoNum*GeShuNum),
            ?INFO_LOG("recharge_add_yuanbao_msg 1 PlayerId=~p (?pd_gold_ingot)=~p",[get(?pd_id),erlang:get(?pd_gold_ingot)]),
            player:add_gold_ingot(TotalNum, ?RECHARGE),
            ?INFO_LOG("recharge_add_yuanbao_msg 2 PlayerId=~p TotalNum=~p get(?pd_gold_ingot)=~p",[get(?pd_id),TotalNum,erlang:get(?pd_gold_ingot)]),
            do_log(get(?pd_id), TotalNum, list_to_binary(Billno), list_to_integer(Amt)),
            recharge_perday_operate(TotalNum),
            ?INFO_LOG("recharge_add_yuanbao_msg 3 PlayerId=~p TotalNum=~p get(?pd_gold_ingot)=~p",[get(?pd_id),TotalNum,erlang:get(?pd_gold_ingot)]),
            ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_SEND_MONEY_SUC,TotalNum:16>>);
        ?true ->
            ?ERROR_LOG("ItemList=~p",[ItemList])
    end;

handle_msg(_FromMod, {?time_to_update}) ->
    Id = get(?pd_recharge_perday_id),
    State = get(?pd_recharge_prize_state),
    put(?pd_today_recharge, {com_time:now(), 0}),
    put(?pd_recharge_return_get_prize_list, []),
    case State =:= ?HAD_GOT of
        true ->
            put(?pd_recharge_perday_id, Id + 1),
            put(?pd_recharge_prize_state, ?CANT_GET);
        _ ->
            pass
    end,
    send_recharge_perday_info(),
    send_recharge_return_info();

handle_msg(FromMod, Msg) ->
  ?ERROR_LOG("~p recv a unknown msg ~p from ~p", [?pname(), Msg, FromMod]).

load_config_meta() ->
    [
        #config_meta{record=#activities_recharge_cfg{},
            fields = record_info(fields, activities_recharge_cfg),
            file="activities_recharge.txt",
            keypos=#activities_recharge_cfg.id,
            all= [#activities_recharge_cfg.id],
            verify = fun verify/1}
    ].

verify(#activities_recharge_cfg{id=Id, type=Type, time=Time, hour = _Hour, value=Value, prize=P}) ->
    ?check(erlang:is_integer(Id), "activities_recharge_cfg[~p] id 不是数字! ", [Id]),
    ?check(erlang:is_tuple(Value), "activities_recharge_cfg[~p] value 不是tuple ",[Value]),
    if
        Type =:= 1 orelse Type =:= 3 ->
            ?check(Time >= 0, "activities_recharge_cfg[~p] Time 要大于0! ", [Time]);
        Type =:= 2 ->
            ?check(erlang:is_list(Time), "line id=~p activities_recharge_cfg[~p] Time 不是tuple ",[Id,Time]);
        true ->
            ok
    end,
    case is_integer(P) of
        ?true ->
            ?check(prize:is_exist_prize_cfg(P),"activities_recharge_cfg[~p] prize :~p 没有找到! ", [Id, P]);
        ?false ->
            lists:foreach(fun(Prize) ->
                ?check(prize:is_exist_prize_cfg(Prize),"activities_recharge_cfg[~p] prize :~p 没有找到! ", [Id, Prize])
            end,
            tuple_to_list(P))
    end,
    ok;

verify(_R) ->
    ?ERROR_LOG("mount ~p 无效格式", [_R]).

set_activities_recharge_endtime() ->
    put(?pd_recharge_perday_endtime, com_time:now() - 1),
    put(?pd_recharge_return_endtime, com_time:now() - 1),
    BeginTime = com_time:localtime_to_sec(shop_server:get_start_server_time()), %% 开服时间
    CfgIdList = lookup_all_activities_recharge_cfg(#activities_recharge_cfg.id),
    lists:foreach(
        fun(Id) ->
                case lookup_activities_recharge_cfg(Id) of
                    Cfg when erlang:is_record(Cfg, activities_recharge_cfg) ->
                        DayNum = Cfg#activities_recharge_cfg.time,
                        EndTime = BeginTime + ?SECONDS_PER_DAY * DayNum,
                        case Cfg#activities_recharge_cfg.type of
                            ?TYPE_OF_RECHARGE_PERDAY ->
                                case DayNum =:= 0 of
                                    true ->
                                        put(?pd_recharge_perday_endtime, 0);
                                    _ ->
                                        put(?pd_recharge_perday_endtime, EndTime)
                                end;
                            ?TYPE_OF_RECHARGE_RETURN ->
                                case DayNum =:= 0 of
                                    true ->
                                        put(?pd_recharge_return_endtime, 0);
                                    _ ->
                                        put(?pd_recharge_return_endtime, EndTime)
                                end;
                            _ ->
                                pass
                        end;
                    _ ->
                        pass
                end
        end,
        CfgIdList
    ).

do_log(PlayerId, TotalNum, Billon, Amt) ->
    Time = com_time:now(),
    case db_mysql:select_row(?DB_POOL, order_info, "add_time", [{player_id, PlayerId}, {first_recharge, 1}]) of
        [] ->
            db_mysql:insert(?DB_POOL, order_info, [player_id, add_time, ingot, first_recharge, order_sn, money], [PlayerId, Time, TotalNum, 1, Billon, Amt]);
        _ ->
            db_mysql:insert(?DB_POOL, order_info, [player_id, add_time, ingot, first_recharge, order_sn, money], [PlayerId, Time, TotalNum, 0, Billon, Amt])
    end,
    ok.

%% 登录发送每日充值信息
send_recharge_perday_info() ->
    RechargeList = get(?pd_perday_recharge),
    case RechargeList =/= [] of
        true ->
            {Id, _} = lists:nth(1, RechargeList),
            ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_SEND_RECHARGE_PERDAY_INFO, Id:8, 1:8>>);
        _ ->
            Id = get(?pd_recharge_perday_id),
            State = get(?pd_recharge_prize_state),
            % BeginTime = com_time:localtime_to_sec(shop_server:get_start_server_time()), %% 开服时间
            case lookup_activities_recharge_cfg(Id) of
                Cfg when is_record(Cfg, activities_recharge_cfg) ->
                    case Cfg#activities_recharge_cfg.type =:= ?TYPE_OF_RECHARGE_PERDAY of
                        true ->
                            case com_time:now() =< get(?pd_recharge_perday_endtime) of
                                true ->
                                    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_SEND_RECHARGE_PERDAY_INFO, Id:8, State:8>>);
                                _ ->
                                    ok
                            end;
                        _ ->
                            ok
                    end;
                _ ->
                    ok
            end
    end.

%% 每日充值处理
recharge_perday_operate(RechargeNum) ->
    {LastTime, TodayRecharge} = get(?pd_today_recharge),
    {LastRecharge, TodayRechargeTotal} = case com_time:is_same_day(LastTime, com_time:now()) of
        true ->
            put(?pd_today_recharge, {com_time:now(), TodayRecharge + RechargeNum}),
            {TodayRecharge, TodayRecharge + RechargeNum};
        _ ->
            put(?pd_today_recharge, {com_time:now(), RechargeNum}),
            {0, RechargeNum}
    end,
    RechargePerdayId = get(?pd_recharge_perday_id),
    State = get(?pd_recharge_prize_state),
    RechargeList = get(?pd_perday_recharge),
    % BeginTime = com_time:localtime_to_sec(shop_server:get_start_server_time()), %% 开服时间
    case lookup_activities_recharge_cfg(RechargePerdayId) of
        Cfg when is_record(Cfg, activities_recharge_cfg) ->
            case Cfg#activities_recharge_cfg.type =:= ?TYPE_OF_RECHARGE_PERDAY of
                true ->
                    case com_time:now() =< get(?pd_recharge_perday_endtime) of
                        true ->
                            {_, PrizeRecharge} = Cfg#activities_recharge_cfg.value,
                            case LastRecharge =< PrizeRecharge andalso TodayRechargeTotal >= PrizeRecharge andalso State =:= ?CANT_GET of
                                true ->
                                    NewList = RechargeList ++ [{RechargePerdayId, com_time:now()}],
                                    put(?pd_perday_recharge, NewList),
                                    put(?pd_recharge_perday_id, RechargePerdayId + 1),
                                    put(pd_recharge_prize_state, ?CANT_GET),
                                    send_recharge_perday_info();
                                _ ->
                                    pass
                            end;
                        _ ->
                            pass
                    end;
                _ ->
                    pass
            end;
        _ ->
            pass
    end,
    send_recharge_return_info().

get_recharge_perday_prize() ->
    RechargeList = get(?pd_perday_recharge),
    case RechargeList =:= [] of
        true ->
            pass;
        _ ->
            [{RechargePerdayId, Timestamp} | ResList] = RechargeList,
            case lookup_activities_recharge_cfg(RechargePerdayId) of
                Cfg when is_record(Cfg, activities_recharge_cfg) ->
                    Job = get(?pd_job),
                    Prize = erlang:element(Job, Cfg#activities_recharge_cfg.prize),
                    prize:get_prize(Prize, ?GET_RECHARGE_PRIZE),
                    world:broadcast(?to_client_msg(<<?MSG_S2C_SCENE_BROADCAST_ACHIEVEMENT, 127, ?pkg_sstr(get(?pd_name)), Prize:32>>)),
                    put(?pd_perday_recharge, ResList),
                    case com_time:is_same_day(Timestamp, com_time:now()) of
                        true ->
                            put(?pd_recharge_perday_id, RechargePerdayId),
                            put(?pd_recharge_prize_state, ?HAD_GOT);
                        _ ->
                            pass
                    end,
                    send_recharge_perday_info();
                _ ->
                    pass
            end
    end.

send_recharge_return_info() ->
    {_LastTime, TotalRecharge} = get(?pd_today_recharge),
    {_, {H, M, S}} = erlang:localtime(),
    RestTime = ?SECONDS_PER_DAY - H * 3600 - M * 60 - S,
    EndTime = get(?pd_recharge_return_endtime),
    Num = erlang:length(get(?pd_recharge_return_get_prize_list)),
    SendData = lists:foldl(
        fun(Id, Acc) ->
                <<Acc/binary, Id:8>>
        end,
        <<?MSG_S2C_ACTIVITIES_RECHARGE_RECHARGE_RETURN_INFO, TotalRecharge:32, RestTime:32, EndTime:32, Num:8>>,
        get(?pd_recharge_return_get_prize_list)
    ),
    ?player_send(<<SendData/binary>>).

get_recharge_return_prize(Id) ->
    GetPrizeList = get(?pd_recharge_return_get_prize_list),
    EndTime = get(?pd_recharge_return_endtime),
    case EndTime =< com_time:now() andalso EndTime =/= 0 of
        true ->
            ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_GET_RECHARGE_RETURN_PRIZE_RESPONSE, Id:8, 2:8>>);
        _ ->
            case lookup_activities_recharge_cfg(Id) of
                Cfg when erlang:is_record(Cfg, activities_recharge_cfg) ->
                    case Cfg#activities_recharge_cfg.type =:= ?TYPE_OF_RECHARGE_RETURN of
                        true ->
                            {_, PrizeRecharge} = Cfg#activities_recharge_cfg.value,
                            {_LastTime, TodayRecharge} = get(?pd_today_recharge),
                            case TodayRecharge >= PrizeRecharge andalso not lists:member(Id, GetPrizeList) of
                                true ->
                                    Job = get(?pd_job),
                                    Prize = erlang:element(Job, Cfg#activities_recharge_cfg.prize),
                                    prize:get_prize(Prize, ?GET_RECHARGE_PRIZE),
                                    put(?pd_recharge_return_get_prize_list, [Id | GetPrizeList]),
                                    world:broadcast(?to_client_msg(<<?MSG_S2C_SCENE_BROADCAST_ACHIEVEMENT, 132, ?pkg_sstr(get(?pd_name)), Prize:32>>)),
                                    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_GET_RECHARGE_RETURN_PRIZE_RESPONSE, Id:8, 1:8>>);
                                _ ->
                                    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_GET_RECHARGE_RETURN_PRIZE_RESPONSE, Id:8, 0:8>>)
                            end;
                        _ ->
                            ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_GET_RECHARGE_RETURN_PRIZE_RESPONSE, Id:8, 0:8>>)
                    end;
                _ ->
                    ?player_send(<<?MSG_S2C_ACTIVITIES_RECHARGE_GET_RECHARGE_RETURN_PRIZE_RESPONSE, Id:8, 0:8>>)
            end
    end.
