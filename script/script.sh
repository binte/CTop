#!/bin/bash

# Define application's name
TARGET="prog"

# Define output file
OUT="out.txt"

# Delete output file (if it exists), so that data obtained from previous runs is deleted
rm -f $OUT

# Define the number of times to run the programme for each instance as the first parameter passed to the script
RUNS=$1

# Declare an array with the names of the instances
INSTANCES=("Cp4.3m.txt" "Cp5.4.z.txt" "Cp7.3.s.txt" "dados.dat")


# get array length
len=${#INSTANCES[@]}

for (( i=0; i<$len; i++ ));
	do
		instance=${INSTANCES[$i]}
		
		echo "----------------- INSTANCE: $instance -----------------" >> $OUT

		for (( j=0; j<$RUNS; j++ ));
			do
				./$TARGET	$instance true >> $OUT
			done

		echo "-------------------------------------------------------" >> $OUT
			
	done

