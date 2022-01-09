#!/bin/bash
DIRECTORIES="1_IO 2_Variables 3_Operations 4_Control_Structures"
FILES=`find $DIRECTORIES -name "*.decaf" | sort`
SPIM="spim"
DECAF="../../decaf"
REGEX="^([0-9]+|"true"|"false")=([0-9]+|"true"|"false")$"

for x in $FILES
do
	curr_file="${x/.decaf/}"
	$DECAF $x -o "$curr_file.mips" > /dev/null 2>&1
	$SPIM -file "$curr_file.mips" 2>&1 \
        | sed -e 1,5d > "$curr_file.xout" 2>&1
	working="\033[1;32mOK\033[0m"
	while read line 
	do
   		var1=$(echo $line | cut -f1 -d=)
		var2=$(echo $line | cut -f2 -d=)
		if [ "$var1" != "$var2" ] || [[ ! $line =~ $REGEX ]]
		then
			working="\033[1;31mKO\033[0m"
		fi
	done <<< "$(cat $curr_file.xout)"
	echo -e "$curr_file : $working"
	rm $curr_file.xout $curr_file.mips
done