#!/bin/bash

function print_usage()
{
   printf 'Usage: %s [config_name]\n' "$0"
   exit 1
}

config_name=base

if [ $# -eq 1 ]; then
   config_name=$1

elif [ $# -gt 1 ]; then
   print_usage
fi

./build.sh "$config_name" > /dev/null

build_exit_code=$?
if [ $build_exit_code -ne 0 ]
then
   exit $build_exit_code
fi

executable="example"

echo "Running $executable in $config_name configuration"
echo

../build/"$config_name"/"$executable"/"$executable"