#!/bin/bash
PREFIX="dynprelim"
FOLDER="prelim"
for g in `ls ~/projects/graph_collection/$FOLDER/*.seq`; do 
        echo $g;
done
rm results_$PREFIX\_measure
rm results_$PREFIX\_blossom
rm results_$PREFIX\_dynblossom*
rm results_$PREFIX\_randomwalk*
rm results_$PREFIX\_neiman
rm results_$PREFIX\_baswana

for seed in `seq 1 25`; do 
CURDIR=`pwd`
cd ~/projects/graph_collection/$FOLDER/
for g in `ls *.graph`; do
	for g in `ls *.graph`; do convert_metis_seq $g --shuffle --seed=$seed; done
done
cd $CURDIR


for g in `ls ~/projects/graph_collection/$FOLDER/*.seq`; do 
        echo $g;
        echo "######################## graph only"
        echo $g >> results_$PREFIX\_measure
        ./deploy/dynmatch $g --algorithm=blossom --measure_graph_only >> results_$PREFIX\_measure
        echo "######################## blossom"
        echo $g >> results_$PREFIX\_blossom
        ./deploy/dynmatch $g --algorithm=blossom >> results_$PREFIX\_blossom
        echo "######################## dyn blossom (speed heuristic) "
        echo $g >> results_$PREFIX\_dynblossom
        ./deploy/dynmatch $g --algorithm=dynblossom >> results_$PREFIX\_dynblossom

        for eps in 0.1 0.25 0.5 1 2; do 
        echo "######################## bounded dyn blossom 2/eps - 1, eps=0.1"
        echo $g >> results_$PREFIX\_dynblossom$eps
        ./deploy/dynmatch $g --algorithm=dynblossom --eps=$eps >> results_$PREFIX\_dynblossom$eps
        done

        echo "######################## dyn blossom (speed heuristic) "
        echo $g >> results_$PREFIX\_dynblossom_speed
        ./deploy/dynmatch $g --algorithm=dynblossom --dynblossom_speedheuristic >> results_$PREFIX\_dynblossomspeed

        for eps in 0.1 0.25 0.5 1 2; do 
	echo "######################## bounded dyn blossom (speed heuristic2) /eps - 1, eps=0.1"
        echo $g >> results_$PREFIX\_dynblossom_speed_$eps
        ./deploy/dynmatch $g --algorithm=dynblossom --eps=$eps --dynblossom_speedheuristic >> results_$PREFIX\_dynblossom_speed_$eps
        done

        for eps in 0.1 0.25 0.5 1 2; do 
        echo "######################## bounded random walk 2/eps - 1, eps=0.1"
        echo $g >> results_$PREFIX\_randomwalk$eps
        ./deploy/dynmatch $g --algorithm=randomwalk --eps=$eps >> results_$PREFIX\_randomwalk$eps
        done

        for eps in 0.1 0.25 0.5 1 2; do 
        echo "######################## bounded random walk 2/eps - 1, eps=0.1"
        echo $g >> results_$PREFIX\_randomwalkdegreesettle$eps
        ./deploy/dynmatch $g --algorithm=randomwalk --rw_ending_additional_settle --eps=$eps >> results_$PREFIX\_randomwalkdegreesettle$eps
        done

        for eps in 0.1 0.25 0.5 1 2; do 
        echo "######################## bounded random walk 2/eps - 1, eps=0.1"
        echo $g >> results_$PREFIX\_randomwalkdegreesettle_10lowdegree$eps
        ./deploy/dynmatch $g --algorithm=randomwalk --rw_low_degree_value=10 --rw_ending_additional_settle --eps=$eps >> results_$PREFIX\_randomwalkdegreesettle_10lowdegree$eps
        done

        echo "######################## neiman solomon"
        echo $g >> results_$PREFIX\_neiman
        ./deploy/dynmatch $g --algorithm=neimansolomon >> results_$PREFIX\_neiman

        echo "######################## baswana gupta seng "
        echo $g >> results_$PREFIX\_baswana
        ./deploy/dynmatch $g --algorithm=baswanaguptaseng >> results_$PREFIX\_baswana
done
done


