#!/bin/bash

filesdir=$1
searchstr=$2

if [ -z "$filesdir" ] || [ -z "$searchstr" ]; then
	echo "Usage: ./finder.sh <filesdir> <searchstr>"
	exit 1
fi

if [ ! -d "$filesdir" ]; then
	echo "Directory $filesdir does not exist!"
	exit 1
fi

count=`find $filesdir/* | wc -l`
matches=`grep -r $searchstr $filesdir/* | wc -l`

echo "The number of files are $count and the number of matching lines are $matches"
