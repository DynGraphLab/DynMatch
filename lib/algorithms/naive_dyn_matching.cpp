/******************************************************************************
 * naive_dyn_matching.cpp
 *
 *
 *****************************************************************************/

#include "naive_dyn_matching.h"

naive_dyn_matching::naive_dyn_matching (dyn_graph_access* G, MatchConfig & match_config) : dyn_matching(G, match_config) {
}

bool naive_dyn_matching::new_edge(NodeID source, NodeID target) {

        G->new_edge(source, target);
        G->new_edge(target, source);

        // check whether the vertices are free. if so, add to the matching
        if (is_free(source) && is_free(target)) {
                match (source, target);
        } else if (config.naive_settle_on_insertion) {
                if (is_free(source) || is_free(target)) {
                        NodeID not_free  = source,
                               free_node = target;

                        if (is_free(source)) {
                                not_free  = target;
                                free_node = source;
                        }
                        
                        NodeID mate_nf = mate(not_free);
                        unmatch(not_free, mate_nf);
                        
                        ASSERT_TRUE(is_free(source) && is_free(target));
                        match(source, target);
                        
                        if(!settle(mate_nf, &free_node)) {
                                unmatch(source, target);
                                match(not_free, mate_nf);
                        }
                }
        }

        return true;
}

// CS change function to void
bool naive_dyn_matching::remove_edge(NodeID source, NodeID target) {
        G->remove_edge(source, target);
        G->remove_edge(target, source);

        /* starting calculation of matching */
        if (is_matched(source, target)) {
                unmatch(source, target);

                ASSERT_TRUE(!is_matched(source, target));

                settle(source);
                settle(target);
        }
        return true;
}

bool naive_dyn_matching::settle (NodeID u, const NodeID* avoid_ptr) {
        EdgeID deg_u = G->get_first_invalid_edge(u);
        NodeID v;
        NodeID avoid = 0;
        bool mate_found = false;
        bool avoid_defined = (avoid_ptr != nullptr);
        
        if (avoid_defined) {
                avoid = *avoid_ptr;
        }

        for (EdgeID i = 0; i < deg_u; ++i) {
                v = G->getEdgeTarget(u, i);
                if (avoid_defined) {
                       if(v == avoid) 
                        continue;
                }
                
                if (is_free(v)) {
                        mate_found = true;
                        break;
                }
        }

        if (mate_found) {
                match(u, v);
        }

        return mate_found;
}

