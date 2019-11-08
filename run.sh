#!/bin/bash


#for ALGO in blossomblossom
echo "######################## graph only"
./deploy/dynmatch examples/tmp.seq --algorithm=blossom --measure_graph_only

echo "######################## blossom"
./deploy/dynmatch examples/tmp.seq --algorithm=blossom

echo "######################## dyn blossom (speed heuristic) "
./deploy/dynmatch examples/tmp.seq --algorithm=dynblossom

echo "######################## bounded dyn blossom 2/eps - 1, eps=0.1"
./deploy/dynmatch examples/tmp.seq --algorithm=dynblossom --eps=0.1

echo "######################## bounded dyn blossom 2/eps - 1, eps=.25"
./deploy/dynmatch examples/tmp.seq --algorithm=dynblossom --eps=.25

echo "######################## bounded dyn blossom 2/eps - 1, eps=.5"
./deploy/dynmatch examples/tmp.seq --algorithm=dynblossom --eps=.5

echo "######################## bounded dyn blossom 2/eps - 1, eps=1"
./deploy/dynmatch examples/tmp.seq --algorithm=dynblossom --eps=1

echo "######################## bounded dyn blossom 2/eps - 1, eps=2"
./deploy/dynmatch examples/tmp.seq --algorithm=dynblossom --eps=2

echo "######################## random walk 2/eps - 1, eps=0.1"
./deploy/dynmatch examples/tmp.seq --algorithm=randomwalk --eps=0.1

echo "######################## random walk 2/eps - 1, eps=2"
./deploy/dynmatch examples/tmp.seq --algorithm=randomwalk --eps=2

echo "######################## neiman solomon"
./deploy/dynmatch examples/tmp.seq --algorithm=neimansolomon 

echo "######################## baswana gupta seng "
./deploy/dynmatch examples/tmp.seq --algorithm=baswanaguptaseng 

echo "AUTO"
echo "######################## graph only"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=blossom --measure_graph_only

echo "######################## blossom"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=blossom

echo "######################## dyn blossom (speed heuristic) "
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=dynblossom

echo "######################## bounded dyn blossom 2/eps - 1, eps=0.1"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=dynblossom --eps=0.1
echo "######################## bounded dyn blossom 2/eps - 1, eps=.25"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=dynblossom --eps=.25

echo "######################## bounded dyn blossom 2/eps - 1, eps=.5"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=dynblossom --eps=.5

echo "######################## bounded dyn blossom 2/eps - 1, eps=1"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=dynblossom --eps=1

echo "######################## bounded dyn blossom 2/eps - 1, eps=2"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=dynblossom --eps=2

echo "######################## random walk 2/eps - 1, eps=0.1"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=randomwalk --eps=0.1

echo "######################## random walk 2/eps - 1, eps=2"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=randomwalk --eps=2

echo "######################## neiman solomon"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=neimansolomon 

echo "######################## baswana gupta seng "
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq --algorithm=baswanaguptaseng 
