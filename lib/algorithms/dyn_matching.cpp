#include "dyn_matching.h"

dyn_matching::dyn_matching (dyn_graph_access* G) {
        this->G = G;
        matching.resize(G->number_of_nodes(), NOMATE);
        matching_size = 0;
}

dyn_matching::~dyn_matching () {
        if ( G != nullptr ) {
                delete G;
                G = nullptr;
        }
}

dyn_graph_access dyn_matching::getG () {
        return *G;
}

std::vector<NodeID> dyn_matching::getM () {
        return matching;
}

NodeID dyn_matching::getMSize () {
        return matching_size/2;
}



bool dyn_matching::is_free (NodeID u) {
        return matching[u] == NOMATE;
}

bool dyn_matching::is_matched (NodeID u, NodeID v) {
        if (is_free(u) || is_free(v)) return false;
        
        ASSERT_TRUE((matching[u] == v) == (matching[v] == u));
        return matching[u] == v && matching[v] == u;
}

NodeID dyn_matching::mate (NodeID u) {
        return matching[u];
}

void dyn_matching::match (NodeID u, NodeID v) {
        ASSERT_TRUE(is_free(u));
        ASSERT_TRUE(is_free(v));
        ASSERT_TRUE(u != v);
        
        matching[u] = v;
        matching[v] = u;
        
        matching_size += 2;
}

void dyn_matching::unmatch (NodeID u, NodeID v) {
        ASSERT_TRUE(!is_free(u));
        ASSERT_TRUE(!is_free(v));
        ASSERT_TRUE(u != v);
        
        matching[u] = NOMATE;
        matching[v] = NOMATE;

        matching_size -= 2;
}
