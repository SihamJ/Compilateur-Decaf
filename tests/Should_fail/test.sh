#!/bin/bash
DIRECTORIES="1_IO 2_Variables 3_Operations 4_Control_Structures 5_Functions 6_Context"	# Test files directories
SPIM="spim" 																			# Interpreter for the MIPS files
DECAF="../../decaf"																		# Path to compiler executable
OK="\033[1;32mOK\033[0m"
KO="\033[1;31mKO\033[0m"
LOCALOK=0
LOCALKO=0
GLOBALOK=0
GLOBALKO=0
TOTALCOUNT=0

cd $(dirname $0)

printf '%50s\n' | tr ' ' -
printf '%29s%-21s\n' "Should_fail"
printf '%50s\n' | tr ' ' -

if [[ ! -f "$DECAF" ]]											# Verify if executable exists
then
	echo "Couldn't find executable, please compile project before launching tests"
	exit 1
fi
for d in $DIRECTORIES
do
	FILES=`find $d -name "*.decaf" | sort`
	printf '%50s\n' | tr ' ' -
	printf '%26s%-24s\n' $d
	printf '%50s\n' | tr ' ' -
	for x in $FILES
	do
		curr_file="${x/.decaf/}"
		$DECAF $x -o "$curr_file.mips" > /dev/null 2>&1
		RET=$?
		if [ $RET == 1 ]
		then
			working=$OK
		else
			working=$KO
		fi
		if [ $working == $OK ]
		then
			LOCALOK=$((LOCALOK+1))
		else
			LOCALKO=$((LOCALKO+1))
		fi
		echo -e "\t$(basename -- $curr_file) : $working"
		rm $curr_file.mips
		TOTALCOUNT=$((TOTALCOUNT+1))
	done
	GLOBALOK=$((GLOBALOK+LOCALOK))
	GLOBALKO=$((GLOBALKO+LOCALKO))
	if [ $LOCALKO == 0 ]
	then
		LOCALKO="\033[1;37m$LOCALKO\033[0m"
	else
		LOCALKO="\033[1;31m$LOCALKO\033[0m"
	fi
	LOCALOK="\033[1;32m$LOCALOK\033[0m"
	echo -e "\n"
	echo -e "PASSED: \033[1;32m$LOCALOK\033[0m\tFAILED: \033[1;31m$LOCALKO\033[0m"
	LOCALOK=0
	LOCALKO=0
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