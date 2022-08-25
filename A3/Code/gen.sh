#!/bin/bash
# Define a string variable with a value
ki="1 4"
km="1 1.5"
kn="0.5 1"
kf="0.1 0.3"
ps="0.99 0.9999"

COUNTER=0

# Iterate the string variable using for loop
for i in $ki; do
    for m in $km; do
        for n in $kn; do
            for f in $kf; do
                for p in $ps; do
                    python3 mytcp.py -i $i -m $m -n $n -f $f -s $p -T "1024" -o out.txt -a $COUNTER
                    COUNTER=$(( COUNTER + 1))
                    echo $COUNTER
                done
            done
        done
    done
done