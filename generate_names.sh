#/bin/bash


rm -rf *.tmp
LISTALGO=$(for g in `ls results_social_* | grep -v tmp | grep -v measure`; do
        echo -ne "$g "
done)

./dump_columns.sh $LISTALGO

LISTALGOCOLUMNS=$(for g in `ls results_social_* | grep -v tmp | grep -v measure`; do
        echo -ne "$g.tmp "
done)
paste -d , $LISTALGOCOLUMNS
rm -rf *.tmp
