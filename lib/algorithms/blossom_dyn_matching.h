/******************************************************************************
 * mcm_dyn_matching.h
 *
 *
 ******************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
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
        private:
                std::vector< NodeID > label;
                std::vector< NodeID > pred;
                std::vector< long > path1;
                std::vector< long > path2;
                std::vector< NodeID > source_bridge;
                std::vector< NodeID > target_bridge;

                std::vector< NodeID > reset_st_bridge;
                 
                long strue;
                NodeID last_source;
                node_partition base;
};

#endif 
