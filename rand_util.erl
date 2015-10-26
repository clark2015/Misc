%%%-------------------------------------------------------------------
%%% @author clark
%%% @copyright (C) 2015, <COMPANY>
%%% @doc
%%%
%%% @end
%%% Created : 26. 十月 2015 下午6:36
%%%-------------------------------------------------------------------
-module(rand_util).
-author("clark").

%% API
-export
([
    get_random_list/2
]).



get_random_list(NeedNum, SrcList) ->
    Top =
        lists:foldl
        (
            fun({_ID, Pre}, Acc) ->
                if
                    Pre > Acc -> Pre;
                    true -> Acc
                end
            end,
            0,
            SrcList
        ),
    get_random_list(NeedNum, Top, SrcList).
get_random_list(_NeedNum, _TopPre, []) -> [];
get_random_list(NeedNum, TopPre, SrcList) ->
    PreLine = com_util:random(0, TopPre),
    {LL, RL} =
        lists:foldl
        (
            fun({ID, Pre}, {LList, RList}) ->
                if
                    Pre >= PreLine ->
                        {[ID|LList], RList};
                    true ->
                        {LList, [ID|RList]}
                end
            end,
            {[], []},
            SrcList
        ),
    Len = erlang:length(LL),
    if
        Len > NeedNum ->
            lists:sublist(LL, NeedNum);
        Len < NeedNum ->
            PreLine1 = com_util:random(0, PreLine),
            LL ++ get_random_list(NeedNum-Len, PreLine1, RL);
        true ->
            LL
    end.