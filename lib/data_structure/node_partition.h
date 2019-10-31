/******************************************************************************
 * union_find.h 
 * *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 * Christian Schulz <christian.schulz.phone@gmail.com>
 *****************************************************************************/

#ifndef NODE_PARTITION_H
#define NODE_PARTITION_H

#include "definitions.h"

#include <sparsehash/dense_hash_map>

// A simple Union-Find datastructure implementation.
// This is sometimes also caled "disjoint sets datastructure.
class node_partition 
{
        public:
                node_partition(dyn_graph_access * G) : m_parent(G->number_of_nodes()), m_rank(G->number_of_nodes()) {
                        forall_nodes((*G), node) {
                                m_parent[node] = node;
                                m_rank[node] = 0;
                        } endfor
                        
                }
               
                inline NodeID Find(NodeID element)
                {
                        if( m_parent[element] != element ) {
                                NodeID retValue = Find( m_parent[element] );  
                                m_parent[element] = retValue; // path compression
                                return retValue;
                        }
                        return element;
                };

                void union_blocks(NodeID lhs, NodeID rhs) {
                        NodeID set_lhs = Find(lhs);
                        NodeID set_rhs = Find(rhs);
                        if( set_lhs != set_rhs ) {
                                if( m_rank[set_lhs] < m_rank[set_rhs]) {
                                        m_parent[set_lhs] = set_rhs;
                                } else {
                                        m_parent[set_rhs] = set_lhs;
                                        if( m_rank[set_lhs] == m_rank[set_rhs] ) m_rank[set_lhs]++;
                                }
                        }

                }

                void split( std::vector< NodeID > & T ) {
                        for( unsigned i = 0; i < T.size(); i++) {
                                m_parent[T[i]] = T[i];
                                m_rank[T[i]] = 0;
                                make_rep(T[i]);
                        }
                }

                void make_rep( NodeID v) {
                        NodeID set = Find(v);
                        if ( set != v ) {
                                // make v the representative
                                m_parent[v] = v;
                                m_parent[set] = v; 
                        }
                
                }

                NodeID operator()( NodeID v ) { return Find( v ); }

        private:
                std::vector< NodeID > m_parent;
                std::vector< NodeID > m_rank;

};

#endif
