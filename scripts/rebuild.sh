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

./clean.sh "$config_name"
./build.sh "$config_name"