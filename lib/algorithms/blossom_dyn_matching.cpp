//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#include <algorithm>
#include "blossom_dyn_matching.h"
#include "node_partition.h"

blossom_dyn_matching::blossom_dyn_matching(dyn_graph_access* G, MatchConfig & match_config) : dyn_matching(G, match_config) {
        label.resize(G->number_of_nodes()); 
        pred.resize(G->number_of_nodes()); 
        path1.resize(G->number_of_nodes());
        path2.resize(G->number_of_nodes());
        source_bridge.resize(G->number_of_nodes());
        target_bridge.resize(G->number_of_nodes());
        search_started.resize(G->number_of_nodes());

        strue = 0;
        base.init(G);
        iteration = 1;

        // if base[v] = v, then v in G'
        // else v was collapsed into base[v]
        // {base(v) | v \in V} is the node set of G'
        // and edge {v,w} represents an edge {base(v), base(w)} in G'
        forall_nodes((*G), node) {
                label[node] = EVEN; // can be EVEN, ODD, or UNLABELED
                pred[node]  = UNDEFINED_NODE;
                path1[node] = 0;
                path2[node] = 0;
                source_bridge[node] = UNDEFINED_NODE;
                target_bridge[node] = UNDEFINED_NODE;
                search_started[node] = 0;
        } endfor
}

blossom_dyn_matching::~blossom_dyn_matching() {

}

bool blossom_dyn_matching::new_edge(NodeID source, NodeID target) {
        G->new_edge(source, target);
        G->new_edge(target, source);

        if( is_free(source) && is_free(target) ) {
                matching[source] = target;
                matching[target] = source;
                label[source] = UNLABELED;
                label[target] = UNLABELED;
                matching_size++;
                return true;
        }

        iteration++;
        if( search_started[source] != 0 && iteration - search_started[source] < G->number_of_edges()/2 ) return false;
        if(is_free(source)) augment_path(source);
        if(is_free(target)) augment_path(target);

        search_started[ source ] = iteration;
        search_started[ target ] = iteration;
        return true;
}

bool blossom_dyn_matching::remove_edge(NodeID source, NodeID target) {
        G->remove_edge(source, target);
        G->remove_edge(target, source);

        if (is_matched(source, target)) {
                unmatch(source, target);
                label[source] = EVEN;
                label[target] = EVEN;
        }

        
        if(is_free(source)) augment_path(source);
        if(is_free(target)) augment_path(target);

        iteration++;
        return true;
}

NodeID  blossom_dyn_matching::getMSize () {
        return matching_size;
}

void blossom_dyn_matching::augment_path(NodeID node) {
        //for( unsigned i = 0; i < reset_st_bridge.size(); i++) {
                //source_bridge[reset_st_bridge[i]] = UNDEFINED_NODE;
                //target_bridge[reset_st_bridge[i]] = UNDEFINED_NODE;
        //}
        //reset_st_bridge.clear();
        //reset_st_bridge.resize(0);

        if( matching[node] != NOMATE) return;

        NodeID LEVEL = G->number_of_nodes();
        std::queue< NodeID > Q; Q.push(node); 
        Q.push(LEVEL);
        std::vector< NodeID > T; T.push_back(node);

        bool breakthrough = false;
        int count = 0; 
        NodeID cur_level = 0;
        while( !breakthrough && !Q.empty()) { // grow tree rooted at node
                count++;
                NodeID v = Q.front(); Q.pop();
                if(  v == LEVEL ) {
                        cur_level += 2;
                        if(cur_level > (NodeID) config.rw_max_length) break;
                        if( Q.empty() ) break;

                        v = Q.front(); Q.pop();
                        Q.push(LEVEL);
                }
                // explore edges out of v
                // assume v is even node
                forall_out_edges((*G), e, v) {
                        NodeID w = G->getEdgeTarget(v, e);
                        if( base(v) == base(w) || label[base(w)] == ODD) 
                                continue; // nothing todo
                        if( label[w] == UNLABELED ) {
                                label[w] = ODD; 
                                T.push_back(w);

                                pred[w] = v;
                                label[matching[w]] = EVEN; 

                                T.push_back(matching[w]);
                                Q.push(matching[w]);
                        } else { // base(w) is EVEN
                                NodeID hv = base(v);
                                NodeID hw = base(w);

                                strue++;
                                path1[hv] = strue;
                                path2[hw] = strue;

                                while( (path1[hw] != strue && path2[hv] != strue) && (matching[hv] != NOMATE || matching[hw] != NOMATE) ) {
                                        if( matching[hv] != NOMATE ) {
                                                hv = base( pred[ matching[hv] ] );
                                                path1[hv] = strue;
                                        }

                                        if( matching[hw] != NOMATE ) {
                                                hw = base( pred[ matching[hw] ] );
                                                path2[hw] = strue;
                                        }
                                }

                                if( path1[ hw ] == strue || path2[ hv ] == strue ) {
                                        // shrink blossom
                                        NodeID b = (path1[hw] == strue) ? hw : hv; //base
                                        shrink_path(b,v,w, base, source_bridge, target_bridge, Q);
                                        shrink_path(b,w,v, base, source_bridge, target_bridge, Q);
                                } else {
                                        // augment path
                                        std::vector< NodeID > P_int;
                                        find_path( P_int, v, hv, source_bridge, target_bridge);

                                        std::vector< NodeID > P;
                                        P.push_back(w);
                                        P.insert(P.end(), P_int.begin(), P_int.end()); 
                                        while( !P.empty() ) {
                                                NodeID a = P.back(); P.pop_back();
                                                NodeID b = P.back(); P.pop_back();

                                                matching[a] = b;
                                                matching[b] = a;
                                        }
                                        T.push_back(w);
                                        breakthrough = true;
                                        matching_size++;
                                        break;
                                }

                        }        
                } endfor
        }
        for( unsigned i = 0; i < T.size(); i++) {
                if( matching[T[i]] == NOMATE ) {
                        label[T[i]] = EVEN;
                } else {
                        label[T[i]] = UNLABELED;
                }
        }
        base.split(T);
        //std::cout <<  "run for " <<  count  << std::endl;
}

void blossom_dyn_matching::shrink_path( NodeID b, NodeID v, NodeID w, node_partition & base, std::vector< NodeID > & source_bridge, std::vector< NodeID > & target_bridge, std::queue< NodeID > & Q) {
        // Note we are working in x prime (so node can be blossoms by itself
        NodeID x = base(v);
        while(x != b) {
                base.union_blocks(x,b);
                //base.union_blocks_withreset(x,b);
                x = matching[x];

                base.union_blocks(x,b);
                //base.union_blocks_withreset(x,b);
                base.make_rep(b);

                Q.push(x);

                source_bridge[x] = v; target_bridge[x] = w;
                x = base( pred[x] );
                //reset_st_bridge.push_back(x);
        }
}


void blossom_dyn_matching::find_path( std::vector< NodeID > & P, NodeID x, NodeID y, std::vector< NodeID > & source_bridge, std::vector< NodeID > & target_bridge) {
        if ( x == y ) {
                P.push_back(x);
                return;
        }

        if( label[x] == EVEN ) {
                P.push_back(x);
                P.push_back(matching[x]);
                find_path(P, pred[matching[x]], y, source_bridge, target_bridge);
                return;
        } else { // x is ODD
                P.push_back(x);

                std::vector< NodeID > P2; 
                find_path( P2, source_bridge[x], matching[x], source_bridge, target_bridge);

                std::reverse(P2.begin(), P2.end());
                P.insert(P.end(), P2.begin(), P2.end()); // concatenate P and P2
                find_path( P, target_bridge[x], y, source_bridge, target_bridge);
                return;
        }
}
