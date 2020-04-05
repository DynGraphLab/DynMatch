//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#include <algorithm>
#include "blossom_dyn_matching_naive.h"
#include "node_partition.h"
#include "static_blossom.h"

blossom_dyn_matching_naive::blossom_dyn_matching_naive(dyn_graph_access* G, MatchConfig & match_config) : dyn_matching(G, match_config) {
}

blossom_dyn_matching_naive::~blossom_dyn_matching_naive() {

}

bool blossom_dyn_matching_naive::new_edge(NodeID source, NodeID target) {
        G->new_edge(source, target);
        G->new_edge(target, source);

        static_blossom sblossom(G, config);
        sblossom.postprocessing();
        matching_size = sblossom.getMSize();
        matching = sblossom.getM();

        return true;
}

bool blossom_dyn_matching_naive::remove_edge(NodeID source, NodeID target) {
        G->remove_edge(source, target);
        G->remove_edge(target, source);

        static_blossom sblossom(G, config);
        sblossom.postprocessing();
        matching_size = sblossom.getMSize();
        matching = sblossom.getM();

        
        return true;
}

NodeID  blossom_dyn_matching_naive::getMSize () {
        return matching_size;
}


