#/bin/bash
#head of file algorithm names
#input lines of A1,A2, ... AN 
#this is for maximization problems

tail -n +2 $1 > columns.raw.tmp
less columns.raw.tmp | awk 'BEGIN {FS=","} {min=100000; for(i=1; i<=NF; i++) {if( $i < min ) {min=$i}}  for(i=1; i<=NF; i++) {printf "%.5f ",min/$i} print ""}' > columns.frac.tmp
COLUMNS=`less columns.frac.tmp | awk '{L=NF} END {print L}'`


#now sort each for the columns
for i in `seq 1 $COLUMNS`; do 
        cut -f $i -d " " columns.frac.tmp | sort -n -r > $i.tmp
done

LISTTMP=$(for i in `seq 1 $COLUMNS`; do 
        echo -ne "$i.tmp "
done)

paste $LISTTMP > sortedquotients.tmp
LINES=`less 1.tmp | wc -l`
for i in `seq 1 $COLUMNS`; do 
echo "1,0," > staircase.$i.tmp
less $i.tmp | awk -v x=$LINES 'BEGIN {COUNT=0; FILL=0} {if(COUNT==0) prev=$1; if($1 != prev) {printf "%.5f,%.5f,\n",prev,COUNT/x; prev=$1; } else {FILL+=1} COUNT+=1;} END {for( i=1; i <= FILL; i++) {printf "%.5f,%.5f,\n",prev,COUNT/x;}}' >> staircase.$i.tmp
#echo "0,1," >> staircase.$i.tmp
done
LISTTMP=$(for i in `seq 1 $COLUMNS`; do 
        echo -ne "staircase.$i.tmp "
done)
paste $LISTTMP > final_output_time
NUMALGO=`head -n 1 $1 | sed 's/,/\n/g' | wc -l`
head -n 1 $1 | sed 's/,/\n/g' > algonames.tmp
cat algonames.tmp

for i in `seq 1 $NUMALGO`; do
    TWO=`echo "2*$i-1" | bc`
    echo $TWO >> startcolumn.tmp
done 
paste startcolumn.tmp algonames.tmp


cp plot_head_time.gnuplot plot_current_time.gnuplot
COUNT="1"
while IFS= read -r line;
do 
        if [ $COUNT -eq "1" ]; then
            echo "plot \"final_output_time\" using 1:2 with steps lw 4 lt $COUNT title '$line', \\" >> plot_current_time.gnuplot
     else 
            RHS=` echo "$COUNT+1" | bc`
            echo "\"final_output_time\" using $COUNT:$RHS with steps lw 4 lt $COUNT title '$line', \\" >> plot_current_time.gnuplot

        fi

        COUNT=` echo "$COUNT+2" | bc`
done < "algonames.tmp"

rm *.tmp
gnuplot < plot_current_time.gnuplot > output_time.png
