/******************************************************************************
 * baswanaguptasen_dyn_matching.h
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

#ifndef BASWANAGUPTASEN_DYN_MATCHING_H
#define BASWANAGUPTASEN_DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <sparsehash/dense_hash_set>

#include "dyn_matching.h"
#include "../tools/random_functions.h"
#include "../tools/timer.h"

class baswanaguptasen_dyn_matching : public dyn_matching {
        public:
                baswanaguptasen_dyn_matching (dyn_graph_access* G);
                ~baswanaguptasen_dyn_matching ();

                virtual bool new_edge(NodeID source, NodeID target);
                virtual bool remove_edge(NodeID source, NodeID target);

                virtual void counters_next();

        protected:
                // O_u denotes the set of edges owned by u. we save those
                // edges by simply saving the edges endpoint, since every
                // edge owned by u also starts at u.
                //~ std::vector<std::unordered_map<NodeID, NodeID> > O;
                std::vector<google::dense_hash_set<NodeID> > O;
                std::vector<int> levels;
                double threshold;


                void set_level (NodeID u, int level);
                int level (NodeID u);
                int level (NodeID u, NodeID v);
                void check_size_constraint();
                
                virtual void handle_addition (NodeID u, NodeID v);
                virtual void handling_insertion (NodeID u, NodeID v);

                void handle_deletion (NodeID u, NodeID v);
                void handling_deletion (NodeID u);

                NodeID random_settle (NodeID u);
                void naive_settle (NodeID u);
};

#endif // BASWANAGUPTASEN_DYN_MATCHING_H
