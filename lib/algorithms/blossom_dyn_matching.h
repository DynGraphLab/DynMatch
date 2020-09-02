/******************************************************************************
 * mcm_dyn_matching.h
 *
 *
 *****************************************************************************/

#ifndef MY_BLOSSOM_DYN_MATCHING_H
#define MY_BLOSSOM_DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#include "dyn_matching.h"
#include "tools/random_functions.h"
#include "tools/timer.h"
#include "data_structure/union_find.h"
#include "tools/misc.h"
#include "node_partition.h"

class blossom_dyn_matching : public dyn_matching {
        public:
                blossom_dyn_matching (dyn_graph_access* G, MatchConfig & config); 
                ~blossom_dyn_matching (); 

                virtual bool new_edge(NodeID source, NodeID target);
                virtual bool remove_edge(NodeID source, NodeID target);
                virtual NodeID getMSize ();
                
                void shrink_path( NodeID b, NodeID v, NodeID w, node_partition & base, std::vector< NodeID > & source_bridge, std::vector< NodeID > & target_bridge, std::queue< NodeID > & Q);
                void find_path( std::vector< NodeID > & P, NodeID x, NodeID y, std::vector< NodeID > & source_bridge, std::vector< NodeID > & target_bridge);
                void augment_path(NodeID source); 

                bool maintain_opt_fallback(NodeID source, NodeID target);
        private:
                std::vector< NodeID > label;
                std::vector< NodeID > pred;
                std::vector< long > path1;
                std::vector< long > path2;
                std::vector< NodeID > source_bridge;
                std::vector< NodeID > target_bridge;
                std::vector< NodeID > reset_st_bridge;
                std::vector< NodeID > search_started;
                std::vector< bool > fallback_visited;
                int iteration;
                 
                long strue;
                node_partition base;
};

#endif 
