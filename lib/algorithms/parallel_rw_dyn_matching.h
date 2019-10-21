/******************************************************************************
 * parallel_rw_dyn_matching.h
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

#ifndef PARALLEL_RW_DYN_MATCHING_H
#define PARALLEL_RW_DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <omp.h>

#include "dyn_matching.h"
#include "../tools/random_functions.h"
#include "../tools/timer.h"
#include "../tools/misc.h"

class parallel_rw_dyn_matching : public dyn_matching {
        public:
                parallel_rw_dyn_matching (dyn_graph_access* G, double eps, unsigned multiple_rws = 1,  bool additional_settle = false, bool low_degree_settle = false, NodeID* low_degree = nullptr, int num_threads = 1);

                virtual bool new_edge(NodeID source, NodeID target);

                virtual bool remove_edge(NodeID source, NodeID target);

                virtual void counters_next();
                
        protected:
                double comp_rw_maxlen();

                virtual void handle_insertion (NodeID source, NodeID target);
                virtual void handle_deletion (NodeID source, NodeID target);

                bool cs_random_walk (NodeID start, std::vector<NodeID>& augpath);
                bool cs_random_walk (NodeID start, std::vector<NodeID>& augpath, NodeID avoid);
                
                virtual bool cs_random_walk (NodeID start, std::vector<NodeID>& augpath, std::pair<NodeID, NodeID> avoid, int thread_id = 0);
                virtual void resolve_augpath(const std::vector<NodeID>& augpath);
                
                virtual bool break_rw (int step);
                bool surrogate (NodeID src, EdgeID& nme, EdgeID max_index, std::pair<NodeID, NodeID> avoid);
                bool _rw_is_freecs(NodeID u, int thread_id = 0); 
                bool find_free (NodeID u, NodeID& v, NodeID t, int thread_id); 

                double eps;
                unsigned multiple_rws;
                double rw_maxlen;
                
                std::vector< std::vector< NodeID > > direct_new_mates;
                std::vector< std::mt19937 > rng;

                
                bool additional_settle;
                bool low_degree_settle;
                double low_degree;
                int num_threads;
};

#endif // PARALLEL_RW_DYN_MATCHING_H
