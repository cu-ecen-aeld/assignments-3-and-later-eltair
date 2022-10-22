#!/bin/bash

# save parameters
filesdir=$1
searchstr=$2

# validate parameters
if [ -z "$filesdir" ] || [ -z "$searchstr" ]; then
	echo "Usage: ./finder.sh <filesdir> <searchstr>"
	exit 1
fi

# check for valid directory
if [ ! -d "$filesdir" ]; then
	echo "Directory $filesdir does not exist!"
	exit 1
fi

count=`find $filesdir/* | wc -l`
matches=`grep -r $searchstr $filesdir/* | wc -l`

echo "The number of files are $count and the number of matching lines are $matches"
