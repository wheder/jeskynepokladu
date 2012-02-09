#!/bin/bash


for i in 4 10 15 20 22 25 27 30 32 35 37 40; do


	g++ -Wall -pedantic -O3 overkill_bb.cpp -o overkill_bb; ./overkill_bb  ../in/knap_${i}.inst.dat 50 | sed -e 's/.* --- //' | awk 'BEGIN{sum=0;go=0;}{v=$0;  if ( (v+0) == v  ) {sum+=$0; go+=1; }}END{printf "%.12f\n",sum/go}' ;



done





