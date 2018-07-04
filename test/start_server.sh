#!/bin/bash

server=$1
dir=$2
nodenum=$3

if [[ x$server == "x" ]];then
    exit -1
fi

ps -ef | grep $server | grep -v grep | grep -v $0 | awk '{print $2}' | xargs kill -15

if [[ $nodenum == "1" ]];then
    $server -c $dir/config/pfconfig$nodenum.json -b 127.0.0.1:11264 &
else
    for i in `seq $nodenum`;do
        $server -c $dir/config/phxpf$i.json &
    done
fi

if [[ x$(ps -ef | grep $server | grep -v grep | grep -v $0 | wc -l | sed 's/ //g') != "x$nodenum" ]];then
    exit -1
fi