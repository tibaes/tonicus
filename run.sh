#!/bin/bash

g++ cvfarneback.cpp `pkg-config --cflags --libs opencv`

d="fotos/"
j=0
for i in `ls $d`
do
    if [ -f "$d$j" ]; then
        echo "$d$j" "$d$i"
        mkdir "$j-$i"
        time ./a.out "$d$j" "$d$i" >> "$j-$i/res.txt"
        mv i0.png i1.png flow-1.jpg "$j-$i"
        mv "$j-$i" res/
    fi
    j=$i
done
