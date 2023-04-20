#!bin/sh

file=$1
sed -i 's/,/\n/g' $file
