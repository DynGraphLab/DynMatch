#!/bin/bash


#for ALGO in blossomblossom
echo "######################## blossom"
./deploy/dynmatch ~/projects/graph_collection/evo_clustering/kron_g500-simple-logn16.graph.seq  --algorithm=blossom

echo "######################## blossom"
./deploy/dynmatch ~/projects/graph_collection/evo_clustering/kron_g500-simple-logn16.graph.seq  --algorithm=boostblossom

echo "######################## blossom"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq  --algorithm=blossom

echo "######################## blossom"
./deploy/dynmatch ~/projects/graph_collection/all_walschaw_graphs/auto.graph.seq  --algorithm=boostblossom

