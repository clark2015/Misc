REBAR = ./rebar
DIALYZER = dialyzer

DIALYZER_WARNINGS = -Werror_handling \
                    -Wrace_conditions -Wunderspecs 

DATE=`date +%F_%H_%M`
FILE = "mobile_game_${DATE}.tar.gz"
FILE_DATA = "mobile_game_data_${DATE}.tar.gz"
FILE2 = "mobile_game_server2_${DATE}.tar.gz"

.PHONY: all compile test qc clean dialyze sync gmsg dataup

ALL_MODS_NAME = src/auto/defs/mod_name_def.hrl

all: $(ALL_MODS_NAME) compile

rebuild: gen_sys_log
	@rm -rf src/auto/defs/*
	@rm -rf src/auto/proto/*
	make clean
	@priv/tools/gen_defs && priv/tools/gen_proto && make


def:
	@priv/tools/gen_defs && make

msg:
	@priv/tools/gen_proto && make

gen_sys_log:
	@if [ ! -e src/include/sys_debug_log_enable.hrl ] ; then touch src/include/sys_debug_log_enable.hrl; fi
	@priv/tools/gen_log

upmsg:
	cd src/proto; svn ci -m'ci'; cd -

$(ALL_MODS_NAME): src/wk_player
	@priv/tools/gen_module_define

compile: 
	@$(REBAR) compile

clean:
	@$(REBAR) clean


rar:
	tar -czf  hand_game.tar.gz ebin win.bat priv ../apps/*/ebin data
	cd _msg; svn up; mv ../hand_game.tar.gz  ./ ; svn ci -m'ci' ; cd -

upotp:
	rm -f git_version
	echo ${DATE} > git_version
	tar -czf $(FILE) ../apps/*/ebin ../apps/esqlite/priv ../apps/esqlite/c_src ../mobile_game/ebin ../mobile_game/priv/etc ../mobile_game/data ../mobile_game/git_version ../mobile_game/startup.sh
	scp otp_src_17.0.tar.gz one@120.24.100.174:~/bak/

upalione:
	rm -f git_version
	echo ${DATE} > git_version
	tar -czf $(FILE) ../apps/*/ebin ../apps/esqlite/priv ../apps/esqlite/c_src ../mobile_game/ebin ../mobile_game/priv/etc ../mobile_game/data ../mobile_game/git_version ../mobile_game/startup.sh
	scp $(FILE) one@120.24.100.174:~/game/version/

upali:
	rm -f git_version
	echo ${DATE} > git_version
	tar -czf $(FILE) ../apps/*/ebin ../apps/esqlite/priv ../apps/esqlite/c_src ../mobile_game/ebin ../mobile_game/priv/etc ../mobile_game/data ../mobile_game/git_version ../mobile_game/startup.sh
	scp $(FILE) ali:

upaliserver2:
	rm -f git_version
	echo ${DATE} > git_version
	tar -czf $(FILE2) ../apps/*/ebin ../apps/esqlite/priv ../apps/esqlite/c_src ../mobile_game/ebin ../mobile_game/priv/etc ../mobile_game/data ../mobile_game/git_version ../mobile_game/startup.sh
	scp $(FILE2) ali:

upali2:
	rm -f git_version
	echo ${DATE} > git_version
	tar -czf $(FILE_DATA) ../mobile_game/data
	scp $(FILE_DATA) ali:

up142:
	rm -f git_version
	echo ${DATE} > git_version
	tar -czf $(FILE) ../apps/*/ebin ../apps/esqlite/priv ../apps/esqlite/c_src ../mobile_game/ebin ../mobile_game/priv/etc ../mobile_game/data ../mobile_game/git_version ../mobile_game/startup.sh
	scp $(FILE) santi_server@192.168.0.142:~/version/

up142new:
	rm -f git_version
	echo ${DATE} > git_version
	tar -czf $(FILE) ../apps/*/ebin ../apps/esqlite/priv ../apps/esqlite/c_src ../mobile_game_new/ebin ../mobile_game_new/priv/etc ../mobile_game_new/data ../mobile_game_new/git_version ../mobile_game_new/startup.sh
	scp $(FILE) santi_server@192.168.0.142:~/version/

up:
	svn up;
	cd data; svn up;
	cd priv/proto/; svn up; cd -

svn:
	cd data; svn up; cd ../

rmorig:
	find . -name '*.orig' | xargs rm

rmswp:
	find . -name '.*.swp' | xargs rm

updata:
	cd  data; svn up; cd -

dialyze: 
	@$(DIALYZER)   -pa ebin -I apps -I src  -I src/proto -r src  -r ebin -r -r apps/common apps/config -r apps/pangzi --fullpath $(DIALYZER_WARNINGS) 
