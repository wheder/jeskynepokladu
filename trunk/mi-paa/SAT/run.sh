#!/bin/bash


#for j in 150 200 250 300; do
#	for i in `seq 0 4`; do
#		./sat in/malo${i} ${j} 10 5000 0.8
#		./sat in/dvestepade${i} ${j} 10 5000 0.8
#		./sat in/trista${i} ${j} 10 5000 0.8
#	done;
#done;


#for j in 2 4 6 8; do
#	for i in `seq 0 4`; do
#		./sat in/malo${i} 200 10 5000 0.${j}
#		./sat in/dvestepade${i} 200 10 5000 0.${j}
#		./sat in/trista${i} 200 10 5000 0.${j}
#	done;
#done;

for j in 5000 500 50; do
	for i in `seq 0 4`; do
#		./sat in/malo${i} 200 10 $j 0.8
#		./sat in/dvestepade${i} 200 10 $j 0.8
		./sat in/trista${i} 200 10 $j 0.8
	done;
done;







