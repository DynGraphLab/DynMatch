#!/bin/bash

for g in `ls ~/projects/graph_collection/all_walschaw_graphs/*.graph  `; do echo $g; echo "my dyn match =============="; G=`echo $g | awk 'BEGIN {FS="/"} {print $7 }'`; ./deploy/dynmatch $g.seq --algorithm=myblossomdyn > mydynblossom$G;  done
