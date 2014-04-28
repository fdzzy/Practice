#!/bin/bash

add()
{
    for i in {1..4095}
    do
        vconfig add eth1 $i
    done
}

rem()
{
    for i in {1..4095}
    do
        vconfig rem eth1.$i
    done
}

if [ $1 == "add" ]; then
    add
elif [ $1 == "rem" ]; then
    rem
fi 
