#!/bin/bash
DIRECTORIES="1_IO 2_Variables 3_Operations 4_Control_Structures 5_Functions"	# Test files directories
FILES=`find $DIRECTORIES -name "*.decaf" | sort` 								# Test files search and order
SPIM="spim" 																	# Interpreter for the MIPS files
DECAF="../../decaf"																# Path to compiler executable
REGEX="^([0-9]+|"true"|"false")=([0-9]+|"true"|"false")$"
OK="\033[1;32mOK\033[0m"
KO="\033[1;31mKO\033[0m"
#NB_FILES=0

if [[ ! -f "$DECAF" ]]											# Verify if executable exists
then
	echo "Couldn't find executable, please compile project before launching tests"
	exit 1
fi
for x in $FILES													# Iterate over all test files
do
	#NB_FILES=$((NB_FILES+1))
	curr_file="${x/.decaf/}" 									# Current test file name
	$DECAF $x -o "$curr_file.mips" > /dev/null 2>&1				# Compilation
	$SPIM -file "$curr_file.mips" 2>&1 \
        | sed -e 1,5d > "$curr_file.xout" 2>&1					# Execution
	working=$OK
	if [ "$(cat $curr_file.xout | wc -l)" -lt 1 ]				# If output is empty, test did not run properly
	then
		working=$KO
	fi
	while read line 
	do
   		var1=$(echo $line | cut -f1 -d=)
		var2=$(echo $line | cut -f2 -d=)
		if [ "$var1" != "$var2" ] || [[ ! $line =~ $REGEX ]]	# If output values are wrong or output line does
		then													#	not match pattern, test did not run properly
			working=$KO
		fi
	done <<< "$(cat $curr_file.xout)"
	echo -e "$curr_file : $working"
	rm $curr_file.xout $curr_file.mips
done
#echo $NB_FILES