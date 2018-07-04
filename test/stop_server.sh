#!/bin/bash

server=$1
dir=$2

ps -ef | grep $server | grep -v grep | grep -v $0 | awk '{print $2}' | xargs kill -15

if [[ x$(ps -ef | grep $server | grep -v grep | grep -v $0) != "x" ]];then
    exit -1
fi
