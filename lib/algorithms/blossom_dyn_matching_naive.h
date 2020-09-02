/******************************************************************************
 * mcm_dyn_matching.h
 *
 *
 *****************************************************************************/

#ifndef MY_BLOSSOM_DYN_MATCHING_NAIVE_H
#define MY_BLOSSOM_DYN_MATCHING_NAIVE_H

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

class blossom_dyn_matching_naive : public dyn_matching {
        public:
                blossom_dyn_matching_naive (dyn_graph_access* G, MatchConfig & config); 
                ~blossom_dyn_matching_naive (); 

                virtual bool new_edge(NodeID source, NodeID target);
                virtual bool remove_edge(NodeID source, NodeID target);
                virtual NodeID getMSize ();
                
};

#endif 
