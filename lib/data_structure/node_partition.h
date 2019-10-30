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
                //node_partition(unsigned n = 0) {
                        //m_n = n;
                        
                        ////m_parent.set_deleted_key(std::numeric_limits<unsigned>::max());
                        ////m_parent.set_empty_key(std::numeric_limits<unsigned>::max()-1);
                        ////m_parent.min_load_factor(0.0);

                        ////m_rank.set_deleted_key(std::numeric_limits<unsigned>::max());
                        ////m_rank.set_empty_key(std::numeric_limits<unsigned>::max()-1);
                        ////m_rank.min_load_factor(0.0);
                        
                        ////for( unsigned i = 0; i < n; i++) {
                                ////Add_Set(i);
                        ////}
                //};
                
                //inline void Union(unsigned lhs, unsigned rhs)
                //{
                        //int set_lhs = Find(lhs);
                        //int set_rhs = Find(rhs);
                        //if( set_lhs != set_rhs ) {
                                //if( m_rank[set_lhs] < m_rank[set_rhs] ) {
                                        //m_parent[set_lhs] = set_rhs;
                                //} else {
                                        //m_parent[set_rhs] = set_lhs;
                                        //if( m_rank[set_lhs] == m_rank[set_rhs] ) {
                                                //m_rank[set_lhs]++;
                                        //}
                                //}
                                //--m_n;
                        //}
                //};

                inline NodeID Find(NodeID element)
                {
                        if( m_parent[element] != element ) {
                                NodeID retValue = Find( m_parent[element] );  
                                m_parent[element] = retValue; // path compression
                                return retValue;
                        }
                        return element;
                };


                //inline void Add_Set (unsigned element) {
                        //m_parent[element] = element;
                        //m_rank[element] = 0;
                        //m_n++;
                //}
                
                //inline void Reset () {
                        //m_parent.clear();
                        //m_rank.clear();
                        //m_n = 0;
                //}


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
                                //reset(T[i]);
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
                //google::dense_hash_map< unsigned, unsigned > m_parent;
                //google::dense_hash_map< unsigned, unsigned > m_rank;

                std::vector< NodeID > m_parent;
                std::vector< NodeID > m_rank;

};

#endif
