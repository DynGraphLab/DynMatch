#/bin/bash


LISTALGO=`echo "$@"| sed 's/ /,/g'`
echo $LISTALGO > cols
./dump_columns_time.sh $@ 

LISTALGO=$(for var in $@; do echo $var.tmp; done)
paste -d , $LISTALGO >> cols
rm -rf *.tmp
./generate_performance_plot_time.sh cols
rm cols 
