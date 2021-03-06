/******************************************************************************
 * union_find.h 
 * *
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
                node_partition(dyn_graph_access * G) {
                        init(G); 
                }

                node_partition() {} 

                inline void init( dyn_graph_access * G) {
                        m_parent.resize(G->number_of_nodes());
                        m_rank.resize(G->number_of_nodes());

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

                void union_blocks_withreset(NodeID lhs, NodeID rhs) {
                        NodeID set_lhs = Find(lhs);
                        NodeID set_rhs = Find(rhs);
                        if( set_lhs != set_rhs ) {
                                if( m_rank[set_lhs] < m_rank[set_rhs]) {
                                        if( m_parent[set_lhs] == lhs ) m_reset.push_back(set_lhs);
                                        m_parent[set_lhs] = set_rhs;
                                } else {
                                        if( m_parent[set_rhs] == rhs ) m_reset.push_back(set_lhs);
                                        m_parent[set_rhs] = set_lhs;
                                        if( m_rank[set_lhs] == m_rank[set_rhs] ) m_rank[set_lhs]++;
                                }
                        }
                }

                void reset() {
                        for( unsigned i = 0; i < m_reset.size(); i++) {
                                m_parent[m_reset[i]] = m_reset[i];
                                m_rank[m_reset[i]] = 0;
                        }
                        m_reset.clear();
                        m_reset.resize(0);
                
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
                std::vector< NodeID > m_reset;

};

#endif
