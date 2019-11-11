#!/bin/bash

#a=$1
#for (( ; a<=; a+=4 ))
for a in 1 2 4 5 8 10 16 20 25 32 40 50 64 80 100 #125 128 160 200 250 256 320 400 500 512 625 640 800 1000 1024 1250 1280 1600 2000 2048 2500 2560 3125 3200 4000 4096 5000 5120 6250 6400 8000 8192
do
	#echo "CheckNum: "$a
	./main $a 5 case_PreNum_$a.csv Y Y
	for (( c=10; c<=60; c+=5 ))
	do  
		./main $a $c case_PreNum_$a.csv N Y
	done
done

echo

for a in 1 2 4 5 8 10 16 20 25 32 40 50 64 80 100 #125 128 160 200 250 256 320 400 500 512 625 640 800 1000 1024 1250 1280 1600 2000 2048 2500 2560 3125 3200 4000 4096 5000 5120 6250 6400 8000 8192
do
	#echo "CheckNum: "$a
	./test.2 $a 5 case_PreNum_$a.rev1.csv Y Y
	for (( c=10; c<=60; c+=5 ))
	do  
		./test.2 $a $c case_PreNum_$a.rev1.csv N Y
	done
done



