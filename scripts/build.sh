#!/bin/bash

function print_usage()
{
   printf 'Usage: %s [config_name]\n' "$0"
   exit 1
}

function build_config()
{
   local config_name=$1
   printf 'Building %s configuration...\n' "$config_name"

   (
      cd ..
      cmake --preset "$config_name"
      cmake --build --preset "$config_name" --parallel
   )

   if [ $? -ne 0 ]; then
      >&2 echo "Failed to build $config_name configuration"
      exit 1
   fi
}

if [ $# -eq 1 ]; then
   build_config $1

elif [ $# -eq 0 ]; then
   build_config base

else
   print_usage
fi
