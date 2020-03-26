#!/bin/bash

echo measure 
TIME=`less results_social_measure | grep takes | awk 'BEGIN {G=1} {G*=($4)^(1/22)} END {print G}'`
echo $TIME
echo blossom
SIZE=`less results_social_blossom | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/22)} END {print G}'`
TIME=`less results_social_blossom | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/22)} END {print G}'`
echo $SIZE $TIME
echo dyn blossom
SIZE=`less results_social_dynblossom | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/22)} END {print G}'`
TIME=`less results_social_dynblossom | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/22)} END {print G}'`
echo $SIZE $TIME
for eps in 0.1 0.25 0.5 1 2; do 
echo dyn blossom $eps
SIZE=`less results_social_dynblossom$eps | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/22)} END {print G}'`
TIME=`less results_social_dynblossom$eps | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/22)} END {print G}'`
echo $SIZE $TIME
done
for eps in 0.1 0.25 0.5 1 2; do 
echo random walk $eps
SIZE=`less results_social_randomwalk$eps | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/22)} END {print G}'`
TIME=`less results_social_randomwalk$eps | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/22)} END {print G}'`
echo $SIZE $TIME
done
echo neiman $eps
SIZE=`less results_social_neiman | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/22)} END {print G}'`
TIME=`less results_social_neiman | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/22)} END {print G}'`
echo $SIZE $TIME
echo baswana $eps
SIZE=`less results_social_baswana | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/22)} END {print G}'`
TIME=`less results_social_baswana | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/22)} END {print G}'`
echo $SIZE $TIME

echo measure 
TIME=`less results_walshaw_measure | grep takes | awk 'BEGIN {G=1} {G*=($4)^(1/22)} END {print G}'`
echo $TIME
echo blossom
SIZE=`less results_walshaw_blossom | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/35)} END {print G}'`
TIME=`less results_walshaw_blossom | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/35)} END {print G}'`
echo $SIZE $TIME
echo dyn blossom
SIZE=`less results_walshaw_dynblossom | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/35)} END {print G}'`
TIME=`less results_walshaw_dynblossom | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/35)} END {print G}'`
echo $SIZE $TIME
for eps in 0.1 0.25 0.5 1 2; do 
echo dyn blossom $eps
SIZE=`less results_walshaw_dynblossom$eps | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/35)} END {print G}'`
TIME=`less results_walshaw_dynblossom$eps | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/35)} END {print G}'`
echo $SIZE $TIME
done
for eps in 0.1 0.25 0.5 1 2; do 
echo random walk $eps
SIZE=`less results_walshaw_randomwalk$eps | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/35)} END {print G}'`
TIME=`less results_walshaw_randomwalk$eps | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/35)} END {print G}'`
echo $SIZE $TIME
done
echo neiman $eps
SIZE=`less results_walshaw_neiman | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/35)} END {print G}'`
TIME=`less results_walshaw_neiman | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/35)} END {print G}'`
echo $SIZE $TIME
echo baswana $eps
SIZE=`less results_walshaw_baswana | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($1)^(1/35)} END {print G}'`
TIME=`less results_walshaw_baswana | grep -v matching | grep -v took | grep -v home | awk 'BEGIN {G=1} {G*=($2)^(1/35)} END {print G}'`
echo $SIZE $TIME


