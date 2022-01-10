#!/bin/bash
SPIM="spim"
DECAF="../../decaf"
REGEX="^\*\*\*\*.*"
OK="\033[1;32mOK\033[0m"
KO="\033[1;31mKO\033[0m"
GLOBALOK=0
GLOBALKO=0
TOTALCOUNT=0

cd $(dirname $0)

if [[ ! -f "$DECAF" ]]											# Verify if executable exists
then
	echo "Couldn't find executable, please compile project before launching tests"
	exit 1
fi

FILES=`find -name "*.decaf"`

printf '%50s\n' | tr ' ' -
printf '%26s%-24s\n' "Runtime_error"
printf '%50s\n' | tr ' ' -
for x in $FILES
do
	curr_file="${x/.decaf/}"
	$DECAF $x -o "$curr_file.mips" > /dev/null 2>&1
	$SPIM -file "$curr_file.mips" 2>&1 \
				| sed -e 1,5d > "$curr_file.xout" 2>&1
	working=$OK
	if [[ "$(cat $curr_file.xout)" =~ $REGEX ]]
	then
		working=$OK
	else
		working=$KO
	fi
	echo -e "\t$(basename -- $curr_file) : $working"
	if [ $working == $OK ]
	then
		GLOBALOK=$((GLOBALOK+1))
	else
		GLOBALKO=$((GLOBALKO+1))
	fi
	rm $curr_file.xout $curr_file.mips
	TOTALCOUNT=$((TOTALCOUNT+1))
done
if [ $GLOBALKO == 0 ]
then
	GLOBALKO="\033[1;37m$GLOBALKO\033[0m"
else
	GLOBALKO="\033[1;31m$GLOBALKO\033[0m"
fi
GLOBALOK="\033[1;32m$GLOBALOK\033[0m"
printf '%50s\n' | tr ' ' -
echo -e "TOTAL\tPASSED: $GLOBALOK\tFAILED: $GLOBALKO"
printf '%50s\n' | tr ' ' -