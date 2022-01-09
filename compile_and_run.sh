#!/bin/bash
DECAF="./decaf"
SPIM="spim"
SCRIPTDIR=$(dirname $0)

$SCRIPTDIR/$DECAF $1 -o "${1/.decaf/.mips}"
$SPIM -file "${1/.decaf/.mips}"
rm "${1/.decaf/.mips}"