#!/bin/bash

writefile=$1
writestr=$2

if [ -z "$writefile" ] || [ -z "$writestr" ]; then
	echo "Usage: ./writer.sh <writefile> <writestr>"
	exit 1
fi

echo $writestr > $writefile
