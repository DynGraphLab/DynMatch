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
