//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#include "my_blossom_dyn_matching.h"
#include "node_partition.h"

my_blossom_dyn_matching::my_blossom_dyn_matching(dyn_graph_access* G, MatchConfig & match_config) : dyn_matching(G, match_config) {
        //matching[i] initialized to NOMATE for all i
        //matching_size = 0
        //base.resize(G->number_of_nodes()); // canonical representative for v in G' (G' is the graph that has blossoms shrinked
        //
        label.resize(G->number_of_nodes()); 
        pred.resize(G->number_of_nodes()); 
        node_partition base(G);

        long strue = 0;
        std::vector< long > path1(G->number_of_nodes(),0);
        std::vector< long > path2(G->number_of_nodes(),0);

        std::vector< NodeID > source_bridge(G->number_of_nodes(),UNDEFINED_NODE);
        std::vector< NodeID > target_bridge(G->number_of_nodes(),UNDEFINED_NODE);


        // if base[v] = v, then v in G'
        // else v was collapsed into base[v]
        // {base(v) | v \in V} is the node set of G'
        // and edge {v,w} represents an edge {base(v), base(w)} in G'
        forall_nodes((*G), node) {
                //base[node]  = node;
                label[node] = EVEN; // can be EVEN, ODD, or UNLABELED
                pred[node]  = UNDEFINED_NODE;
        } endfor

        forall_nodes((*G), node) {
                if( matching[node] != NOMATE) continue;

                std::queue< NodeID > Q; Q.push(node);
                std::vector< NodeID > T; T.push_back(node);

                bool breakthrough = false;
                while( !breakthrough && !Q.empty()) { // grow tree rooted at node
                        NodeID v = Q.front(); Q.pop();
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
                                                for( unsigned i = 0; i < T.size(); i++) {
                                                        label[T[i]] = UNLABELED;
                                                }
                                                base.split(T);
                                                breakthrough = true;
                                                break;
                                        }

                                }        
                        } endfor
                }
        } endfor
        forall_nodes((*G), node) {
                if( matching[node] != NOMATE ) {
                        matching_size++;
                }
        } endfor
        std::cout <<  "matching size " <<  matching_size / 2  << std::endl;


}

my_blossom_dyn_matching::~my_blossom_dyn_matching() {

}

bool my_blossom_dyn_matching::new_edge(NodeID source, NodeID target) {
        if(is_free(source)) augment_path(source);
        if(is_free(target)) augment_path(target);
        return true;
}

bool my_blossom_dyn_matching::remove_edge(NodeID source, NodeID target) {
        return true;
}

NodeID  my_blossom_dyn_matching::getMSize () {
        return 0;
}

void my_blossom_dyn_matching::augment_path(NodeID source) {

}

void my_blossom_dyn_matching::shrink_path( NodeID b, NodeID v, NodeID w, node_partition & base, std::vector< NodeID > & source_bridge, std::vector< NodeID > & target_bridge, std::queue< NodeID > & Q) {
        // Note we are working in x prime (so node can be blossoms by itself
        NodeID x = base(v);
        while(x != b) {
                base.union_blocks(x,b);
                x = matching[x];

                base.union_blocks(x,b);
                base.make_rep(b);

                Q.push(x);

                source_bridge[x] = v; target_bridge[x] = w;
                x = base( pred[x] );
        }
}


void my_blossom_dyn_matching::find_path( std::vector< NodeID > & P, NodeID x, NodeID y, std::vector< NodeID > & source_bridge, std::vector< NodeID > & target_bridge) {
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
