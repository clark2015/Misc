#! /bin/sh

HOSTNAME=`uname -n`
GAME=ffsj
NODENAME=hand
ERL_FLAGS='-smp disable +K true '

ERL="erl -config priv/etc/sys"






show_help()
{
    echo "Usage: 
    -h | -help show help
    st
    t
    default show info"
}

show_info()
{
    ps aux | grep $NODENAME@$HOSTNAME | grep -v grep
}


close()
{
    local PID=$(ps aux | grep $NODENAME@$HOSTNAME | grep -v grep | awk '{print($2)}')
    kill $PID
    echo $PID closed
}


GAME_PAR="+pc unicode +P 50000"
GAME_EBIN="-pa ebin"
GAME_BASE_EBIN="-pa ../apps/*/ebin"
GAME_UNKNOWN="+K true"
GAME_NAME="-s ffsj"
GAME_CFG="-config priv/etc/sys.config"
GAME_COOKIE="-setcookie jjsj"

case $1 in 
    '-h'|'--help')
        show_help ;;
    'st' | 'start')
        erl $GAME_PAR  $GAME_EBIN $GAME_BASE_EBIN $GAME_UNKNOWN $NODENAME@$HOSTNAME $GAME_NAME $GAME_CFG $GAME_COOKIE ;;
    'bg') 
        erl  -noshell -noinput -detached +pc unicode +P 50000  -pa ebin -pa ../apps/*/ebin +K true $NODENAME@$HOSTNAME -s $GAME -config priv/etc/sys.config -setcookie jjsj ;;
    'tt' )
        erl -pa ebin -pa ../apps/*/ebin -pa ;;
    'cl'| 'top')
        close ;;

    *)
        show_info ;;
esac
