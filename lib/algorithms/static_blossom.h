/******************************************************************************
 * mcm_dyn_matching.h
 *
 *
 *****************************************************************************/

#ifndef STATIC_BLOSSOM_MATCHING_H
#define STATIC_BLOSSOM_MATCHING_H

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

class static_blossom : public dyn_matching {
        public:
                static_blossom (dyn_graph_access* G, MatchConfig & config); 
                ~static_blossom (); 

                virtual bool new_edge(NodeID source, NodeID target);
                virtual bool remove_edge(NodeID source, NodeID target);
                virtual NodeID getMSize ();
                virtual void postprocessing();
                
                void shrink_path( NodeID b, NodeID v, NodeID w, node_partition & base, std::vector< NodeID > & source_bridge, std::vector< NodeID > & target_bridge, std::queue< NodeID > & Q);
                void find_path( std::vector< NodeID > & P, NodeID x, NodeID y, std::vector< NodeID > & source_bridge, std::vector< NodeID > & target_bridge);
                void init( std::vector< NodeID > & matching );
        private:
                std::vector< NodeID > label;
                std::vector< NodeID > pred;
                bool initalized;

};

#endif 
