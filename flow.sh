#!/bin/bash

g++ -c flow.cpp -o flow.o `pkg-config --cflags --libs opencv`
g++ flow.o main.cpp -o flow.bin `pkg-config --cflags --libs opencv`

d="fotos/"
j=0
for i in `ls $d`
do
    if [ -f "$d$j" ]; then
        echo "$d$j" "$d$i"
        time ./flow.bin "$d$j" "$d$i"
    fi
    j=$i
done
