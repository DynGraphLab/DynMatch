#/bin/bash


LISTALGO=`echo "$@"| sed 's/ /,/g'`
echo $LISTALGO > cols
./dump_columns.sh $@ 

LISTALGO=$(for var in $@; do echo $var.tmp; done)
paste -d , $LISTALGO >> cols
rm -rf *.tmp
./generate_performance_plot.sh cols
rm cols
