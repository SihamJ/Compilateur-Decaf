#!/bin/bash

echo "" > test.log

if [ ! "$1" == "-v" ]
then
	printf '%50s\n' | tr ' ' -
	printf '%30s%-20s\n' "Should_succeed"
	printf '%50s\n' | tr ' ' -
	./Should_succeed/test.sh | grep "TOTAL"
	echo -e "\n"
	printf '%50s\n' | tr ' ' -
	printf '%28s%-22s\n' "Should_fail"
	printf '%50s\n' | tr ' ' -
	./Should_fail/test.sh | grep "TOTAL"
	echo -e "\n"
	printf '%50s\n' | tr ' ' -
	printf '%29s%-21s\n' "Runtime_error"
	printf '%50s\n' | tr ' ' -
	./Runtime_error/test.sh | grep "TOTAL"
	echo -e "\n"
else
	./Should_succeed/test.sh
	./Should_fail/test.sh
	./Runtime_error/test.sh
fi