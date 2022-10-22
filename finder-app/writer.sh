#!/bin/bash

# read parameters
writefile=$1
writestr=$2

# check parameters
if [ -z "$writefile" ] || [ -z "$writestr" ]; then
	echo "Usage: ./writer.sh <writefile> <writestr>"
	exit 1
fi

# pipe the string to the file
echo $writestr > $writefile
