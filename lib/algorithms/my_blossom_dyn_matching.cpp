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
        node_partition base(G);
        label.resize(G->number_of_nodes()); 
        pred.resize(G->number_of_nodes()); 

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
                std::cout <<  "========================================================= " <<  node  << std::endl;

                bool breakthrough = false;
                while( !breakthrough && !Q.empty()) { // grow tree rooted at node
                        NodeID v = Q.front(); Q.pop();
                        // explore edges out of v
                        // assume v is even node

                         forall_out_edges((*G), e, v) {
                                 NodeID w = G->getEdgeTarget(v, e);
                                 std::cout <<  "target is " <<  w  << std::endl;
                                 if( base(v) == base(w) || label[base(w)] == ODD) 
                                         continue; // nothing todo
                                 if( label[w] == UNLABELED ) {
                                        std::cout <<  "case UNLABELED"  << std::endl;
                                        label[w] = ODD; 
                                        T.push_back(w);

                                        pred[w] = v;
                                        std::cout <<  "setting " << w << " odd"   << std::endl;
                                        std::cout <<  "setting " << matching[w] << " even"   << std::endl;
                                        std::cout <<  "pred " << w << " " << matching[w]   << std::endl;
                                        label[matching[w]] = EVEN; 
                                        
                                        T.push_back(matching[w]);
                                        Q.push(matching[w]);
                                 } else { // base(w) is EVEN

                                        std::cout <<  "case EVEN "  << std::endl;
                                        if( label[base(w)] != EVEN) std::cout <<  "problem "  << std::endl;
                                        NodeID hv = base(v);
                                        NodeID hw = base(w);
                                        std::cout <<  "a"  << std::endl;

                                        strue++;
                                        path1[hv] = strue;
                                        path2[hw] = strue;
                                        std::cout <<  "b"  << std::endl;
                                        std::cout <<  hv << " " << hw  << std::endl;

                                        while( (path1[hw] != strue && path2[hv] != strue) && (matching[hv] != NOMATE || matching[hw] != NOMATE) ) {
                                                std::cout <<  hv << " " << hw  << std::endl;
                                                if( matching[hv] != NOMATE ) {
                                                        std::cout <<  pred[ matching[hv] ]  << std::endl;
                                                        hv = base( pred[ matching[hv] ] );
                                                        path1[hv] = strue;
                                                }

                                                std::cout <<  hv << " " << hw  << std::endl;
                                                if( matching[hw] != NOMATE ) {
                                                        hw = base( pred[ matching[hw] ] );
                                                        path1[hw] = strue;
                                                }

                                                std::cout <<  hv << " " << hw  << std::endl;
                                        }
                                        std::cout <<  "c " << strue  << std::endl;

                                        if( path1[ hw ] == strue || path2[ hv ] == strue ) {
                                                std::cout <<  "found blossom"  << std::endl;
                                                // shrink blossom
                                                NodeID b = (path1[hw] == strue) ? hw : hv; //base
                                                shrink_path(b,v,w, base, source_bridge, target_bridge, Q);
                                                shrink_path(b,w,v, base, source_bridge, target_bridge, Q);
                                        } else {
                                                std::cout <<  "e"  << std::endl;
                                                // augment path
                                                std::vector< NodeID > P;
                                                find_path( P, v, hv, source_bridge, target_bridge);

                                                P.push_back(w);
                                                while( !P.empty() ) {
                                                        NodeID a = P.back(); P.pop_back();
                                                        NodeID b = P.back(); P.pop_back();

                                                        matching[a] = b;
                                                        matching[b] = a;
                                                        std::cout <<  "setting matching " <<  a << " " << b  << std::endl;
                                                }
                                                T.push_back(w);
                                                for( unsigned i = 0; i < T.size(); i++) {
                                                        label[T[i]] = UNLABELED;
                                                }
                                                base.split(T);
                                                breakthrough = true;
                                                std::cout <<  "Q.size " <<  Q.size()   << std::endl;
                                                break;
                                        }

                                 }        
                         } endfor
                }
        } endfor
        std::cout <<  "done "  << std::endl;
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
        std::cout <<  "shrinking path " <<  b <<  " " <<  v << " " << w  << std::endl;
        while(x != b) {
                std::cout <<  "a"  << std::endl;
                base.union_blocks(x,b);
                std::cout <<  "b"  << std::endl;
                x = matching[x];
                std::cout <<  "c"  << std::endl;
               
                if( x == NOMATE ) {
                        std::cout <<  "problem x is NOMATE " << b  << std::endl;
                        exit(0);
                }
                std::cout <<  x << " " << b  << std::endl;
                base.union_blocks(x,b);
                std::cout <<  "d"  << std::endl;
                base.make_rep(b);
                std::cout <<  "e"  << std::endl;

                Q.push(x);
                std::cout <<  "f"  << std::endl;

                source_bridge[x] = v; target_bridge[x] = w;
                std::cout <<  "g"  << std::endl;
                x = base( pred[x] );
                std::cout <<  "h"  << std::endl;
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
