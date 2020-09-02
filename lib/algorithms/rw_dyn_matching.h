/******************************************************************************
 * rw_dyn_matching.h
 *
 *
 *****************************************************************************/

#ifndef RW_DYN_MATCHING_H
#define RW_DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>

#include "dyn_matching.h"
#include "tools/random_functions.h"
#include "tools/timer.h"
#include "tools/misc.h"

class rw_dyn_matching : public dyn_matching {
        public:
                rw_dyn_matching (dyn_graph_access* G, MatchConfig & config); 

                virtual bool new_edge(NodeID source, NodeID target);
                virtual bool remove_edge(NodeID source, NodeID target);

                
        protected:
                virtual void handle_insertion (NodeID source, NodeID target);
                virtual void handle_deletion (NodeID source, NodeID target);

                bool fast_random_walk (NodeID start);
                bool cs_random_walk (NodeID start, std::vector<NodeID>& augpath, size_t& length);
                bool cs_random_walk (NodeID start, std::vector<NodeID>& augpath, size_t& length, NodeID avoid);
                
                virtual bool cs_random_walk (NodeID start, std::vector<NodeID>& augpath, size_t& length, std::pair<NodeID, NodeID> avoid);
                virtual void resolve_augpath(const std::vector<NodeID>& augpath, const size_t& length);
                
                virtual bool break_rw (int step);
                bool surrogate (NodeID src, EdgeID& nme, EdgeID max_index, std::pair<NodeID, NodeID> avoid);
                bool _rw_is_freecs(NodeID u); 
                bool find_free (NodeID u, NodeID& v, NodeID t); 

                std::vector< NodeID > direct_new_mates;
                std::vector< NodeID > augpath;
};

#endif // RW_DYN_MATCHING_H
