/******************************************************************************
 * rw_dyn_matching.cpp
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

#include "parallel_rw_dyn_matching.h"

#ifdef DM_COUNTERS
#include "counters.h"
#endif


parallel_rw_dyn_matching::parallel_rw_dyn_matching (dyn_graph_access* G, double eps, unsigned multiple_rws, bool additional_settle, bool low_degree_settle, NodeID* low_degree, int num_threads) : dyn_matching(G) {
        this->num_threads = num_threads;
        omp_set_num_threads(num_threads);
        
        direct_new_mates.resize(num_threads, std::vector< NodeID >(G->number_of_nodes(), NOMATE));
        
        rng.resize(num_threads);
        for (int i = 0; i < num_threads; ++i) {
                rng[i].seed(rand());
        }
        
        this->eps = eps;
        this->multiple_rws = multiple_rws;
        this->rw_maxlen = comp_rw_maxlen();
        this->additional_settle = additional_settle;
        this->low_degree_settle = low_degree_settle;
        
        if (low_degree == nullptr) {
                this->low_degree = comp_rw_maxlen();
        } else {
                this->low_degree = *low_degree;
        }

#ifdef DM_COUNTERS
        counters::create(std::string("rw" + std::to_string(eps)));
        counters::get(std::string("rw" + std::to_string(eps))).create("match()");
#endif
}

bool parallel_rw_dyn_matching::new_edge(NodeID source, NodeID target) {
        G->new_edge(source, target);
        G->new_edge(target, source);

        handle_insertion (source, target);
        return true;
}

bool parallel_rw_dyn_matching::remove_edge(NodeID source, NodeID target) {
        G->remove_edge(source, target);
        G->remove_edge(target, source);

        handle_deletion(source, target);

        return true;
}

double parallel_rw_dyn_matching::comp_rw_maxlen() {
        return 1/eps;
}

void parallel_rw_dyn_matching::handle_insertion (NodeID source, NodeID target) {
        // check whether the vertices are free. if so, add to the matching
        if (is_free(source) && is_free(target)) {
                match (source, target);
        } else if (is_free(source) || is_free(target)) {
                // get the matched vertex and its mate
                NodeID u = is_free(source)? target : source;
                NodeID m_u = is_free(source)? source : target;
                NodeID v = mate(u);

                unmatch (u, v);
                match (u, m_u);

                unsigned i = 0;
                //unsigned j = 0;
                
                std::vector<NodeID> augpath;
                bool augpath_found = false;
                
                std::vector<NodeID> local;
                bool local_found = false;
                
                #pragma omp parallel for shared(augpath_found) shared(augpath) private(local) private(local_found)
                for (i = 0; i < multiple_rws; i++) {
                        if (augpath_found) { 
                                #pragma omp critical 
                                {
                                        i = multiple_rws; 
                                }
                                continue; 
                        }
                        
                        local_found = cs_random_walk(v, local, {u, m_u}, omp_get_thread_num());
                        
                        if (local_found && !augpath_found) {
                                #pragma omp critical 
                                {
                                        augpath_found = true;
                                        augpath = local;
                                }
                        }
                }
                
                if (augpath_found) {
                        ASSERT_TRUE(!(augpath.size() % 2));
                        resolve_augpath(augpath);
                } else {
                        unmatch (u, m_u);
                        match (u, v);
                }
        }
}

void parallel_rw_dyn_matching::handle_deletion (NodeID source, NodeID target) {
        if (is_matched(source, target)) {
                unmatch(source, target);

                ASSERT_TRUE(!is_matched(source, target));

                for (NodeID w : {
                        source, target
                }) {
                        if (!is_free(w)) break;

                        unsigned i = 0;
                        std::vector<NodeID> augpath;
                        bool augpath_found = false;

                        while (i < multiple_rws && !augpath_found) {
                                augpath_found = cs_random_walk(w, augpath);
                                i++;
                        }

                        if (augpath_found) {
                                resolve_augpath(augpath);
                        }
                }
        }
}

bool parallel_rw_dyn_matching::break_rw (int step) {
        // break if the random walk exceeds the rw_maxlen
        return (step >= rw_maxlen) ? true : false;
}

bool parallel_rw_dyn_matching::cs_random_walk (NodeID start, std::vector<NodeID>& augpath) {
        return cs_random_walk(start, augpath, start);
}

bool parallel_rw_dyn_matching::cs_random_walk (NodeID start, std::vector<NodeID>& augpath, NodeID avoid) {
        return cs_random_walk (start, augpath, {avoid, avoid});
}

bool parallel_rw_dyn_matching::cs_random_walk (NodeID start, std::vector<NodeID>& augpath, std::pair<NodeID, NodeID> avoid, int thread_id) {
        int step = 0;
        NodeID u = start;
        NodeID t = avoid.first;
        NodeID s = avoid.second;

        bool augpath_found = false;

        // initialize augpath to be empty, then add starting node
        augpath.resize(0);
        augpath.push_back(u);
        std::vector< NodeID > set_to_notmate;

        // perform random walk until break condition is met
        while (!break_rw(step)) {

                if( low_degree_settle ) {
                        if (G->getNodeDegree(u) < low_degree) {
                                NodeID v;

                                if (find_free(u, v, start, thread_id)) {
                                        augpath.push_back(v);
                                        augpath_found = true;
                                        set_to_notmate.push_back(u);
                                        set_to_notmate.push_back(v);
                                        break;
                                }
                        }
                }

                EdgeID max_index = G->get_first_invalid_edge(u);

                if (max_index <= 0) break;

                // randomly choose a vertex v from N(u)
                std::uniform_int_distribution<int> A(0, max_index-1);
                EdgeID new_matching_edge = A(rng[thread_id]);
                NodeID v = G->getEdgeTarget(u, new_matching_edge);
                
                if (v == t || v == s) { // we don't want to go back to vertex t, therefore we choose a surrogate in this case
                        bool cont = surrogate(u, new_matching_edge, max_index, {t, s});

                        if (!cont) break;
                        v = G->getEdgeTarget(u, new_matching_edge);
                }

                if (!_rw_is_freecs(v, thread_id)) {
                        // if the vertex is not free, then retrieve mate
                        NodeID w;
                        if (direct_new_mates[thread_id][v] == NOMATE) {
                                w = mate(v);
                        } else {
                                w = direct_new_mates[thread_id][v];
                        }

                        // add these vertices to the augmenting path
                        augpath.push_back(v);
                        augpath.push_back(w);

                        if( direct_new_mates[thread_id][v] == NOMATE) set_to_notmate.push_back(v);
                        if( direct_new_mates[thread_id][u] == NOMATE) set_to_notmate.push_back(u);
                        if( direct_new_mates[thread_id][w] == NOMATE) set_to_notmate.push_back(w);

                        direct_new_mates[thread_id][u] = v;
                        direct_new_mates[thread_id][v] = u;
                        direct_new_mates[thread_id][w] = w;

                        // step to next vertex
                        u = w;
                        t = v;

                        step += 2;
                } else {
                        // if the vertex v is free, we have found an endpoint
                        // to our augmenting path
                        augpath.push_back(v);
                        augpath_found = true;

                        step += 1;
                        break;
                }

        }

        if (!augpath_found) {
                if( additional_settle ) {
                        NodeID v;

                        // if no augmenting path was found, we perform a linear search
                        // through N(u). in case we find a free neigh-
                        // bor, we add this vertex to the augmenting path.
                        if (find_free(u, v, start, thread_id)) {
                                augpath.push_back(v);
                                augpath_found = true;
                                set_to_notmate.push_back(v);
                        }
                } 
        }
        for( unsigned i = 0; i < set_to_notmate.size(); i++) {
                direct_new_mates[thread_id][set_to_notmate[i]] = NOMATE;
        }



        return augpath_found;
}

void parallel_rw_dyn_matching::resolve_augpath (const std::vector<NodeID>& augpath) {
        // an augmenting path is always of uneven length, therefore consists
        // of an even number of vertices
        // further it is at least of length 1, which means we need at
        // least two vertices
        ASSERT_TRUE((!(augpath.size()%1)) && augpath.size () >= 2);

        if (augpath.size() == 2) {
                match(augpath.at(0), augpath.at(1));
        } else {
                NodeID u, v, w;
                size_t i;
                
                // (length of the augpath)-1 has to be even. for those first
                // steps, we assume every second edge to be matched
                for (i = 0; i < augpath.size()-3; i+=2) {
                        u = augpath.at(i),
                        v = augpath.at(i+1),
                        w = augpath.at(i+2);

                        unmatch(v, w);
                        match(u,v);
                }

                match(augpath.at(i), augpath.at(i+1));
        }
}

bool parallel_rw_dyn_matching::surrogate (NodeID source, EdgeID& new_matching_edge, EdgeID max_index, std::pair<NodeID, NodeID> avoid) {
        if (max_index <= 1) { // only one vertex exists, this vertex is the one
                // encoded by new_matching_edge
                return false;
        }

        bool surrogate_found = false;

        for (size_t i = 0; i < max_index; ++i) {
                if (new_matching_edge == max_index-1) {
                        new_matching_edge = 0;
                } else {
                        new_matching_edge++;
                }

                NodeID v = G->getEdgeTarget(source, new_matching_edge);

                if (v != avoid.first && v != avoid.second) {
                        surrogate_found = true;
                        break;
                }
        }

        return surrogate_found;
}

bool parallel_rw_dyn_matching::_rw_is_freecs(NodeID u, int thread_id) {
        bool u_free = false;

        // if v exists in the new_mates hash-map, but points to itself, then it is free
        if (direct_new_mates[thread_id][u] != NOMATE) {
                if (direct_new_mates[thread_id][u] == u) {
                        u_free = true;
                } else {
                        u_free = false;
                }
        } else {
                u_free = is_free(u);
        }

        return u_free;
}


bool parallel_rw_dyn_matching::find_free (NodeID u, NodeID& v, NodeID t, int thread_id) {
        EdgeID deg_u = G->getNodeDegree(u);
        bool mate_found = false;

        for (EdgeID i = 0; i < deg_u; ++i) {
                v = G->getEdgeTarget(u, i);

                // if v is t, which is the node we want to ignore, we continue searching
                // since we search linearily, we can be sure, that no node before t was
                // free
                if (v == t) {
                        continue;
                }

                if (_rw_is_freecs(v, thread_id)) {
                        mate_found = true;
                        break;
                }
        }

        return mate_found;
}

void parallel_rw_dyn_matching::counters_next() {
#ifdef DM_COUNTERS
        counters::get(std::string("rw" + std::to_string(eps))).get("match()").next();
#endif
}
