/******************************************************************************
 * rw_dyn_matching.h
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
