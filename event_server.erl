%%%-------------------------------------------------------------------
%%% @author clark
%%% @copyright (C) 2015, <COMPANY>
%%% @doc
%%%
%%% @end
%%% Created : 23. 十月 2015 下午3:01
%%%-------------------------------------------------------------------
-module(event_server).
-author("clark").


-behaviour(gen_server).

%% API
-export([init/1, handle_call/3, handle_cast/2, handle_info/2, terminate/2, code_change/3]).
-export
([
    start_link/1
    ,sub_info/3
    ,unsub_info/1
    ,sub_call/3
    ,unsub_call/1
    ,sub_terminate/2
    ,unsub_terminate/0
]).


-include_lib("common/src/com_log.hrl").
-include("game.hrl").


-define(call_evt(EvtID),        {'@call_evt@', EvtID}).
-define(cast_evt(EvtID),        {'@cast_evt@', EvtID}).
-define(info_evt(EvtID),        {'@info_evt@', EvtID}).
-define(terminate_evt(EvtID),   {'@terminate_evt@', EvtID}).
-define(code_change_evt(EvtID), {'@code_change_evt@', EvtID}).

start_link({Name, Mod, Args}) ->
    gen_server:start_link({local, Name}, ?MODULE, [Mod, Args], []).

init([Mod, Args]) ->
%%     ?INFO_LOG("event_server ~p",[self()]),
    Mod:init(Args).


sub_info(EvtID, Mod, Callback) ->
    case erlang:get(?info_evt(EvtID)) of
        undefined ->
            erlang:put(?info_evt(EvtID), {Mod, Callback});
        _ ->
            ?ERROR_LOG("repeat sub_info ~p", [{EvtID, Mod, Callback}]),
            erlang:put(?info_evt(EvtID), {Mod, Callback})
    end.
unsub_info(EvtID) ->
    erlang:put(?info_evt(EvtID), undefined).

sub_call(EvtID, Mod, Callback) ->
    case erlang:get(?call_evt(EvtID)) of
        undefined ->
            erlang:put(?call_evt(EvtID), {Mod, Callback});
        _ ->
            ?ERROR_LOG("repeat sub_call ~p", [{EvtID, Mod, Callback}]),
            erlang:put(?call_evt(EvtID), {Mod, Callback})
    end.
unsub_call(EvtID) ->
    erlang:put(?call_evt(EvtID), undefined).


sub_terminate(Mod, Callback) ->
    case erlang:get(?terminate_evt(1)) of
        undefined ->
            erlang:put(?terminate_evt(1), {Mod, Callback});
        _ ->
            ?ERROR_LOG("repeat sub_terminate ~p", [{1, Mod, Callback}]),
            erlang:put(?terminate_evt(1), {Mod, Callback})
    end.
unsub_terminate() ->
    erlang:put(?terminate_evt(1), undefined).




%% ---------------------------------------------
%% protected
%% ---------------------------------------------
handle_call({EvtID, Args}, From, State) ->
    Ret =
        case erlang:get(?call_evt(EvtID)) of
            {Mod, Call} ->
                Mod:Call(Args, From);
            _ ->
                ?ERROR_LOG("Error in handle_call evt_id ~p",[{EvtID, Args, From}]),
                nil
        end,
    {reply, {ok, Ret}, State};
handle_call(Request, From, State) ->
    ?ERROR_LOG("Error in handle_call ~p",[{Request, From}]),
    {reply, ok, State}.



handle_info({EvtID, Args}, State) ->
    case erlang:get(?info_evt(EvtID)) of
        {Mod, Call} ->
            Mod:Call(Args);
        _ ->
            ?ERROR_LOG("Error in handle_info evt_id ~p",[{EvtID, Args}]),
            {ok, nil}
    end,
    {noreply, State};
handle_info(Info, State) ->
    ?ERROR_LOG("Error in handle_info ~p",[Info]),
    {noreply, State}.

terminate(Reason, State) ->
    case erlang:get(?terminate_evt(1)) of
        {Mod, Call} ->
            Mod:Call(Reason, State);
        _ ->
            {ok, nil}
    end,
    ok.

code_change(OldVsn, State, Extra) ->
    ?INFO_LOG("Error in code_change ~p",[{OldVsn, State, Extra}]),
    {ok, State}.

handle_cast(_Request, State) ->
    {noreply, State}.


