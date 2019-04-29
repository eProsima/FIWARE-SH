#!/bin/sh
# Script to erase all subscriptions in Orion context broker

curl 192.168.1.59:1026/v2/subscriptions -s -S -H 'Accept: application/json' | python -mjson.tool | grep 5cc | while read line;

do
    ID=$(echo $line| cut -d':' -f 2)
    ID=$(echo $ID| cut -d'"' -f 2)
    echo $ID

    curl --include --request DELETE 192.168.1.59:1026/v2/subscriptions/$ID

done
