#!/bin/bash
make clean
make
for i in 10 20 30
do
    for j in 1 2 4 8 16
    do
        echo "Thread number: $j   Row range: $i"
        time ./Lab4 adaptec1.aux -r $i -t $j | grep cost 
    done
done
