#!/bin/bash
# $1 seq filename
# [0,1] amount of ops to undo (1 being all)
echo $1 $2
NUMOP=`less $1 | grep -v \# | wc -l`
REVERTOP=`echo $NUMOP*$2 | bc`
REVERTOP=`echo $REVERTOP | awk '{printf "%d",$1}'`

tail -n $REVERTOP $1 | awk '{if($1 == 1) {printf "0 %d %d\n",$2,$3} else {printf "1 %d %d\n",$2,$3}}' > tmp
NEWFILENAME=`echo $1 | sed s/\.seq//g`
cp $1 $NEWFILENAME.undo.$2.seq
tac tmp >> $NEWFILENAME.undo.$2.seq
rm tmp
