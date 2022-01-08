#!/bin/bash
DIRECTORIES="1_IO 2_Variables 3_Operations 4_Control_Structures"
FILES=`find $DIRECTORIES -name "*.decaf" | sort`

for x in $FILES
do
	echo $x
	../decaf $x -o "${x/.decaf/.asm}" > /tmp/null
	echo "Done !"
done