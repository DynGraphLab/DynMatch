#include "mv_algorithm.h"

mv_algorithm::mv_algorithm(dyn_graph_access* G, MatchConfig & config) : dyn_matching(G, config) { matching_size = 0;} ; 
mv_algorithm::~mv_algorithm() {}

bool mv_algorithm::new_edge(NodeID source, NodeID target) {
        G->new_edge(source, target);
        G->new_edge(target, source);
        return true;
}

bool mv_algorithm::remove_edge(NodeID source, NodeID target) {
        G->remove_edge(source, target);
        G->remove_edge(target, source);
        return true;
}

void mv_algorithm::postprocessing() {
        mv(*G, matching_size, mv_matching);
}


NodeID mv_algorithm::getMSize() {
        return (NodeID)matching_size;
}

std::vector< NodeID > & mv_algorithm::getM() {
        return mv_matching;
};
