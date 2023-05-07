#!/bin/bash

gcc ../seq.c ../stack_t.c -lm -o seq
SEQ_OUTPUT=$(./seq)
SEQ_TIME_STR=($SEQ_OUTPUT)
SEQ_TIME_STR=${SEQ_TIME_STR[2]}
SEQ_TIME=${SEQ_TIME_STR:5}

THR_COUNT=6

for ((i = 1; i < $THR_COUNT+1; i++))
do
	gcc ../parallel.c ../stack_t.c -lm -DTHREAD_COUNT=${i} -o parallel
	TEMP=($(./parallel))
	TEMP=${TEMP[2]}
	TEMP=${TEMP:5}
	TEMP=$SEQ_TIME/$TEMP
	TEMP=$(echo $TEMP | bc -l)
	PAR_S_ARR[$i-1]=$TEMP
	INDEXES[$i-1]=$i
	TEMP=$TEMP/$i
	TEMP=$(echo $TEMP | bc -l)
	PAR_E_ARR[$i-1]=$TEMP
done

GNUPLOTOUT="perf.png"

gnuplot -persis <<-EOFMAKER
s_str="${PAR_S_ARR[*]}"
e_str="${PAR_E_ARR[*]}"

array s[$THR_COUNT]
array e[$THR_COUNT]
ind = 1
do for [j in s_str] {
	s[ind] = sprintf("%s", j)
	s[ind] = s[ind]+0
	ind = ind + 1
}
ind = 1
do for [j in e_str] {
	e[ind] = sprintf("%s", j)
	e[ind] = e[ind]+0
	ind = ind + 1
}

set terminal pngcairo
set output "$GNUPLOTOUT"
set grid
set multiplot layout 1,2
plot s title "S(n_{threads})" with linespoints
plot e title "E(n_{threads})" with linespoints
EOFMAKER
