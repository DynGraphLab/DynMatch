#include "baswanaguptasen_dyn_matching.h"

#ifdef DM_COUNTERS
#include "counters.h"
#endif

baswanaguptasen_dyn_matching::baswanaguptasen_dyn_matching (dyn_graph_access* G) :
        dyn_matching(G) {
        O.resize(G->number_of_nodes());
        levels.resize(G->number_of_nodes(), 0);
        
        threshold = std::sqrt(G->number_of_nodes());
        
        for( unsigned i = 0; i < G->number_of_nodes(); i++) {
                O[i].set_deleted_key(std::numeric_limits<NodeID>::max());
                O[i].set_empty_key(std::numeric_limits<NodeID>::max()-1);
                O[i].min_load_factor(0.0);
        }
}

baswanaguptasen_dyn_matching::~baswanaguptasen_dyn_matching() {
        
}

bool baswanaguptasen_dyn_matching::new_edge(NodeID source, NodeID target) {
        G->new_edge(source, target);
        G->new_edge(target, source);
        
        handle_addition(source, target);

        return true;
}

bool baswanaguptasen_dyn_matching::remove_edge(NodeID source, NodeID target) {
        G->remove_edge(source, target);
        G->remove_edge(target, source);

        handle_deletion(source, target);
#ifndef NDEBUG
        check_size_constraint();
#endif

        return true;
}


// BGS SPECIFIC FUNCTIONS //

void baswanaguptasen_dyn_matching::set_level (NodeID u, int level) {
        ASSERT_TRUE(level >= 0 && level <= 1);
        levels.at(u) = level;
}

int baswanaguptasen_dyn_matching::level (NodeID u) {
        return levels.at(u);
}

// returns level of an edge, which is defined as max-level of both endnodes
int baswanaguptasen_dyn_matching::level (NodeID u, NodeID v) {
        return level(u) > level(v) ? level(u) : level(v);
}

void baswanaguptasen_dyn_matching::handle_addition (NodeID u, NodeID v) {
        if (level(u) == 1) {
                O[u].insert(v);
        } else if (level(v) == 1) {
                O[v].insert(u);
        } else { // both at level 0
                handling_insertion(u, v);
        }
}

void baswanaguptasen_dyn_matching::handling_insertion (NodeID u, NodeID v) {
        // handling_insertion gets called when both nodes are on level 0,
        // therefore both nodes own the edge. 
        O[u].insert(v);
        O[v].insert(u);
        
        // if both nodes are free, simply match them
        if (is_free(u) && is_free(v)) {
                match(u, v);
        }

        // swap the nodes, so that u owns more edges than v
        if (O[v].size() > O[u].size()) {
                NodeID tmp = u;
                u = v;
                v = tmp;
        }

        if (O[u].size() >= threshold) {
                // (u,w) becomes unmatched, if w is the mate of u.
                NodeID w = mate(u);

                if (is_matched(u, w)) {
                        unmatch(u, w);
                }

                // since u is at level 0, but violates invariant 2,
                // it will rise to level 1. therefore we assign u as
                // the only owner of all adjacent edges.
                for (auto w_pair : O[u]) {
                        O[w_pair].erase(u);
                }

                // call random settle to rise u to level 1
                NodeID x = random_settle(u);
                
                // settle freed vertices
                if (x != NOMATE) {
                        naive_settle(x);
                }
                
                if (w != NOMATE) {
                        naive_settle(w);
                }
        }
}

void baswanaguptasen_dyn_matching::handle_deletion (NodeID u, NodeID v) {
        // whatever happens afterwards, the edge (u,v) does not longer
        // exist in the graph, therefore all nodes have to forget the
        // respective endpoint as their neighbour and update their set
        // of owned edges. otherwise the algorithm will match the nodes
        // directly again.
        O[u].erase(v);
        O[v].erase(u);
        
        // if (u,v) is not in M, then there's nothing to do
        if (!is_matched(u,v)) {
                ASSERT_TRUE(!is_matched(v,u));
                return;
        }

        // if we didn't return, we remove (u,v) from M
        unmatch(u,v);

        // if (u,v) was in M and...
        if (level(u,v) == 0) { // ... (u,v) was at level 0
                naive_settle(u);
                naive_settle(v);
        }

        if (level(u,v) == 1) { // ... (u,v) was at level 1
                handling_deletion(u);
                handling_deletion(v);
        }
}

void baswanaguptasen_dyn_matching::handling_deletion (NodeID u) {
        // all edges with endpoint w at level 1 take the ownership of (u,w)
        for (auto w = O[u].begin(); w != O[u].end(); ++w) {
                if (level(*w) == 1) {
                        O[*w].insert(u);
                        O[u].erase(*w);
                }
        }

        if (O[u].size() >= threshold) {
                // if O_u is still greater or equal than sqrt(n), it stays at level 1
                // and therefore has to be matched (invariant 1)
                NodeID x = random_settle(u);
                if (x != NOMATE) {
                        naive_settle(x);
                }
        } else {
                // otherwise, if O_u is now less than sqrt(n), it drops to level 0
                set_level(u, 0);
                for (auto w : O[u]) {
                        // edges where both endpoints are level 0 are owned by both endpoints
                        if (level(w) == 0) {
                                O[w].insert(u);
                        }
                }

                naive_settle(u);

                // the set of owned edges of the previously update neighbours increased
                // by 1, therefore they might violate invariant 2.
                for (auto w : O[u]) {
                        if (O[w].size() >= std::sqrt(G->number_of_nodes())) {
                                NodeID mate_w = mate(w);
                                if (!is_free(w)) {
                                        unmatch(w, mate_w);
                                }

                                // since w is at level 0, but violates invariant 2,
                                // it will rise to level 1. therefore we assign w as
                                // the only owner of all adjacent edges.
                                for (auto y : O[w]) {
                                        O[y].erase(w);
                                }

                                NodeID x = random_settle(w);

                                if (x != NOMATE) {
                                        naive_settle(x);
                                }

                                if (mate_w != NOMATE) {
                                        naive_settle(mate_w);
                                }
                        }
                }
        }
}

NodeID baswanaguptasen_dyn_matching::random_settle (NodeID u) {
        NodeID y = 0;
        int tmp = random_functions::nextInt(0, O[u].size() - 1);
        for (auto x : O[u]) {
                if (tmp-- == 0) {
                        y = x;
                        break;
                }
        }
        ASSERT_TRUE(tmp == -1);

        for (auto w : O[y]) {
                O[w].erase(y);
        }

        NodeID x;
        if (!is_free(y)) { // if y is not free, it is matched
                x = mate(y);
                unmatch(x, y);
        } else {
                x = NOMATE;
        }

        match (u, y);

        set_level(u, 1);
        set_level(y, 1);
        return x;
}

void baswanaguptasen_dyn_matching::naive_settle (NodeID u) {
        if (!is_free(u)) return;
        
        for (auto x : O[u]) {
                if (is_free(x)) {
                        match (u, x);
                        break;
                }
        }
}

void baswanaguptasen_dyn_matching::check_size_constraint() {
        // not specifically mentioned as invariant, but still mentioned:
        // no vertex at level 0 can own an edge which is at level 1
        for( unsigned int u = 0; u < G->number_of_nodes(); u++) {
                for ( auto v : O[u] ) {
                        if (level(u) == 0 && level(v) == 1) {
                                std::cout << "before breaking: "
                                          << "level(" << u << ")=" << level(u) << ", "
                                          << "level(" << v << ")=" << level(v) << std::endl;
                                std::cout << "edge should be owned by other endpoint" << std::endl;
                                exit(1);
                        }
                }
                
                //invariant 3
                if(O[u].size() > threshold && level(u) == 0) {
                        std::cout <<  "size constraint violated"  << std::endl;
                        exit(0);
                }
                //invariant 1 
                if(level(u) == 1) {
                        if( is_free(u)) {
                                std::cout <<  "vertex not matched but on level 1"  << std::endl;
                                exit(1);
                        }
                }

                //invariant 4
                forall_out_edges_d((*G), e, u) {
                       NodeID target = G->getEdgeTarget(u, e);
                       if( is_matched(u, target )) {
                               if( level(u) != level(target) ) {
                                        std::cout << u << " and " << target 
                                                  << " ought to be on the same level, but level("
                                                  << u << ")=" << level(u) << " and level("
                                                  << target <<")=" << level(target) << std::endl;
                                        exit(0);
                               }
                       }
                        
                } endfor 

                // every vertex on level 0 has all neighbors matched
                // invariant 2 
                if(level(u) == 0 && is_free(u)) {
                        forall_out_edges_d((*G), e, u) {
                                NodeID target = G->getEdgeTarget(u, e);
                                if( is_free(target) ) {
                                        std::cout <<  "this one should not be free " << u <<  " " << target << std::endl;
                                        exit(0);
                                }
                        } endfor
                }
        }
        
}

void baswanaguptasen_dyn_matching::counters_next() {
        
}
