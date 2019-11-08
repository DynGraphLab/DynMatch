#!/bin/bash

cd examples/
#cp uk.graph.seq start_point.seq
STOP="0"
while [ "$STOP" -eq "0" ]; do 
STOP="1"
wc -l start_point.seq
less start_point.seq > tmp.graph.seq
LINES=`wc -l < tmp.graph.seq`
RANDOMLINE=`echo "1 + ($RANDOM % ($LINES-1))" | bc`
echo $RANDOMLINE
sed -e "$RANDOMLINE"'d' tmp.graph.seq > tmp2.graph.seq

RESULT=`../deploy/dynmatch tmp2.graph.seq --algorithm=myblossomdyn | grep NOMATE | wc -l`
if [ "$RESULT" -eq "1" ]; then
        mv tmp2.graph.seq start_point.seq
        STOP="0"
fi
done
cd ..
