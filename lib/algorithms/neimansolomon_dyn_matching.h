/******************************************************************************
 * neiman_solomon_dyn_matching.h
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

#ifndef NEIMANSOLOMON_DYN_MATCHING_H
#define NEIMANSOLOMON_DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <cmath>
//#include <random>
//#include <chrono>

#include "dyn_matching.h"
#include "../data_structure/avl_tree.h"
#include "../data_structure/priority_queues/maxNodeHeap.h"
#include "../data_structure/priority_queues/bucket_pq.h"
#include "../data_structure/priority_queues/node_bucket_pq.h"
#include "../tools/timer.h"

class neimansolomon_dyn_matching : public dyn_matching {
        public:
                neimansolomon_dyn_matching (dyn_graph_access* G);

                virtual bool new_edge(NodeID source, NodeID target);
                virtual bool remove_edge(NodeID source, NodeID target);

                virtual void counters_next();

        private:
                struct Fv {
                        std::unordered_map<NodeID, NodeID> free;

                        Fv ();

                        bool insert (NodeID node);

                        bool remove (NodeID node);

                        bool has_free ();

                        // returns false if there is no free, and true +
                        // reference when there is a free neighbour
                        NodeID get_free ();
                };
                
                // F(v) is a data structure that holds the free neighbours
                // of a vertex v. We save this data structure for all v in
                // the vector F
                std::vector<Fv> F;

                // F_max holds all free vertices index by their degree
                node_bucket_pq F_max;

                // checks wether the node has a free neighbour by performing F.at(node).has_free().
                // I wrap the Fv-structs function so that I can stick close to the paper syntax.
                bool has_free (NodeID node);

                // returns the free node of neighbour, assuming that has_free has been performed earlier
                NodeID get_free (NodeID node);

                // adds an edge to the matching M
                void match (NodeID u, NodeID v);
                
                // returns the degree of a node
                NodeID deg (NodeID u);

                // returns the value sqrt(2*n + 2*m), where n is the number of nodes and m the number of edges
                NodeID threshold ();

                // function to eliminate augmenting paths of length 3, returns whether an augpath was found or not
                bool aug_path (NodeID u);

                // find a surrogate node for u.
                NodeID surrogate (NodeID u);

                // function to wrap the matchings update
                void handle_addition (NodeID u, NodeID v);
                void handle_deletion (NodeID u, NodeID v);

                // function to bound vertex degree
                void handle_problematic ();
                
                std::vector<NodeID> got_free;
                
                void check_invariants () {
                        std::queue<NodeID> Q;
                        
                        // check invariant 1: every free vertex has degree at most sqrt(2n+2m)
                        for (NodeID u = 0; u < G->number_of_nodes(); ++u) {
                                if (is_free(u)) {
                                        Q.push(u);
                                        if (deg(u) > std::sqrt(2*G->number_of_nodes() + 2*G->number_of_edges())) {
                                                std::cerr << "vertex " << u << " violates invariant 1" << std::endl;
                                                exit(1);
                                        }
                                }
                        }
                        
                        for (NodeID u : got_free) {
                                if (deg(u) > std::sqrt(2*G->number_of_edges())) {
                                        std::cerr << "vertex " << u << " violates invariant 2" << std::endl;
                                        exit(1);
                                }
                        }
                        
                        while (!Q.empty()) {
                                NodeID u = Q.front();
                                Q.pop();
                                for (EdgeID e = 0; e < G->getNodeDegree(u); ++e) {
                                        NodeID v = G->getEdgeTarget(u, e);
                                        // augmenting path of length 1, basically a free edge
                                        if (is_free(v)) {
                                                std::cerr << "vertex " << u << " violates invariant 3, because there is a free edge to " << v << std::endl;
                                                exit(1);
                                        }
                                        NodeID w = mate(v);
                                        
                                        for (EdgeID f = 0; f < G->getNodeDegree(w); ++f) {
                                                NodeID x = G->getEdgeTarget(w, f);
                                                // augmenting path of length 3
                                                if (is_free(x) && u != x) {
                                                        std::cerr << "vertex " << u << " violates invariant 3, because there is an augpath of length 3 to " << x << std::endl;
                                                        exit(1);
                                                }
                                        }
                                }
                        }
                        
                        got_free.clear();
                }
};

#endif // NEIMANSOLOMON_DYN_MATCHING_H
