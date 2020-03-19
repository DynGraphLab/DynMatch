#/bin/bash
#input lines of A1,A2, ... AN 

./generate_names.sh > columns
tail -n +2 columns > columns.raw.tmp

less columns.raw.tmp | awk 'BEGIN {FS=","} {max=0; for(i=1; i<=NF; i++) {if( $i > max ) {max=$i}}  for(i=1; i<=NF; i++) {printf "%.5f ",$i/max} print ""}' > columns.frac.tmp
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
echo "0,1," >> staircase.$i.tmp
done
LISTTMP=$(for i in `seq 1 $COLUMNS`; do 
        echo -ne "staircase.$i.tmp "
done)
paste $LISTTMP > final_output
rm *.tmp
head -n 1 columns
gnuplot < plot.gnuplot > output.png
rm columns
