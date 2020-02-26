#!/bin/bash
PREFIX="walshaw"
for g in `ls ~/projects/graph_collection/all_walschaw_graphs/*.seq`; do 
        echo $g;
done
rm results_$PREFIX\_measure
rm results_$PREFIX\_blossom
rm results_$PREFIX\_dynblossom*
rm results_$PREFIX\_randomwalk*
rm results_$PREFIX\_neiman
rm results_$PREFIX\_baswana

for g in `ls ~/projects/graph_collection/all_walschaw_graphs/*.seq`; do 
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

        for eps in 0.1 0.25 0.5 1 2; do 
        echo "######################## bounded dyn blossom 2/eps - 1, eps=0.1"
        echo $g >> results_$PREFIX\_randomwalk$eps
        ./deploy/dynmatch $g --algorithm=randomwalk --eps=$eps >> results_$PREFIX\_randomwalk$eps
        done

        echo "######################## neiman solomon"
        echo $g >> results_$PREFIX\_neiman
        ./deploy/dynmatch $g --algorithm=neimansolomon >> results_$PREFIX\_neiman

        echo "######################## baswana gupta seng "
        echo $g >> results_$PREFIX\_baswana
        ./deploy/dynmatch $g --algorithm=baswanaguptaseng >> results_$PREFIX\_baswana
done


PREFIX="social"
for g in `ls ~/projects/graph_collection/snw_paper/*.seq`; do 
        echo $g;
done
rm results_$PREFIX\_measure
rm results_$PREFIX\_blossom
rm results_$PREFIX\_dynblossom*
rm results_$PREFIX\_randomwalk*
rm results_$PREFIX\_neiman
rm results_$PREFIX\_baswana

for g in `ls ~/projects/graph_collection/snw_paper/*.seq`; do 
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

        for eps in 0.1 0.25 0.5 1 2; do 
        echo "######################## bounded dyn blossom 2/eps - 1, eps=0.1"
        echo $g >> results_$PREFIX\_randomwalk$eps
        ./deploy/dynmatch $g --algorithm=randomwalk --eps=$eps >> results_$PREFIX\_randomwalk$eps
        done

        echo "######################## neiman solomon"
        echo $g >> results_$PREFIX\_neiman
        ./deploy/dynmatch $g --algorithm=neimansolomon >> results_$PREFIX\_neiman

        echo "######################## baswana gupta seng "
        echo $g >> results_$PREFIX\_baswana
        ./deploy/dynmatch $g --algorithm=baswanaguptaseng >> results_$PREFIX\_baswana
done



