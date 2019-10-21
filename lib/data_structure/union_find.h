/******************************************************************************
 * union_find.h 
 * *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 * Christian Schulz <christian.schulz.phone@gmail.com>
 *****************************************************************************/

#ifndef UNION_FIND_H
#define UNION_FIND_H

#include "definitions.h"

#include <sparsehash/dense_hash_map>

// A simple Union-Find datastructure implementation.
// This is sometimes also caled "disjoint sets datastructure.
class union_find 
{
        public:
                union_find(unsigned n = 0) {
                        m_n = 0;
                        
                        m_parent.set_deleted_key(std::numeric_limits<unsigned>::max());
                        m_parent.set_empty_key(std::numeric_limits<unsigned>::max()-1);
                        m_parent.min_load_factor(0.0);

                        m_rank.set_deleted_key(std::numeric_limits<unsigned>::max());
                        m_rank.set_empty_key(std::numeric_limits<unsigned>::max()-1);
                        m_rank.min_load_factor(0.0);
                        
                        for( unsigned i = 0; i < n; i++) {
                                Add_Set(i);
                        }
                };
                
                inline void Union(unsigned lhs, unsigned rhs)
                {
                        int set_lhs = Find(lhs);
                        int set_rhs = Find(rhs);
                        if( set_lhs != set_rhs ) {
                                if( m_rank[set_lhs] < m_rank[set_rhs] ) {
                                        m_parent[set_lhs] = set_rhs;
                                } else {
                                        m_parent[set_rhs] = set_lhs;
                                        if( m_rank[set_lhs] == m_rank[set_rhs] ) {
                                                m_rank[set_lhs]++;
                                        }
                                }
                                --m_n;
                        }
                };

                inline unsigned Find(unsigned element)
                {
                        if ( m_parent.find(element) == m_parent.end() ) {
                                Add_Set(element);
                                return element;
                        }
                        if( m_parent[element] != element ) {
                                unsigned retValue = Find( m_parent[element] );  
                                m_parent[element] = retValue; // path compression
                                return retValue;
                        }
                        return element;
                };

                inline void Add_Set (unsigned element) {
                        m_parent[element] = element;
                        m_rank[element] = 0;
                        m_n++;
                }
                
                inline void Reset () {
                        m_parent.clear();
                        m_rank.clear();
                        m_n = 0;
                }

                // Returns:
                //   The total number of sets.
                inline unsigned n() const
                { return m_n; };

        private:
                google::dense_hash_map< unsigned, unsigned > m_parent;
                google::dense_hash_map< unsigned, unsigned > m_rank;

                // Number of elements in UF data structure.
                unsigned m_n;
};

#endif
