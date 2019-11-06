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

#include "rw_dyn_matching.h"

rw_dyn_matching::rw_dyn_matching (dyn_graph_access* G, MatchConfig & config) : dyn_matching(G, config) {
        direct_new_mates.resize(G->number_of_nodes(), NOMATE);
        this->augpath = std::vector<NodeID> (config.rw_max_length + 4, NOMATE);
}

bool rw_dyn_matching::new_edge(NodeID source, NodeID target) {
        G->new_edge(source, target);
        G->new_edge(target, source);

        handle_insertion (source, target);

        return true;
}

bool rw_dyn_matching::remove_edge(NodeID source, NodeID target) {
        G->remove_edge(source, target);
        G->remove_edge(target, source);

        handle_deletion(source, target);

        return true;
}

void rw_dyn_matching::handle_insertion (NodeID source, NodeID target) {
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

                unsigned long i = 0;
                bool augpath_found = false;
                size_t length = 0;

                // alternate versions, either constant number of repetitions or based upon maximal degree
                while (i < config.rw_repetitions_per_node && !augpath_found) {
                        augpath_found = cs_random_walk(v, augpath, length, {u, m_u});
                        i++;
                }

                if (augpath_found) {
                        ASSERT_TRUE(!(length % 2));
                        resolve_augpath(augpath, length);
                } else {
                        unmatch (u, m_u);
                        match (u, v);
                }
        }
}

void rw_dyn_matching::handle_deletion (NodeID source, NodeID target) {
        if (is_matched(source, target)) {
                unmatch(source, target);

                ASSERT_TRUE(!is_matched(source, target));

                for (NodeID w : {
                        source, target
                }) {
                        if (!is_free(w)) break;

                        unsigned long long i = 0;
                        size_t length = 0;
                        bool augpath_found = false;
                        
                        // alternate versions, either constant number of repetitions or based upon maximal degree
                        while (i < config.rw_repetitions_per_node && !augpath_found) {
                                augpath_found = cs_random_walk(w, augpath, length);
                                i++;
                        }

                        if (augpath_found) {
                                resolve_augpath(augpath, length);
                        }
                }
        }
}

bool rw_dyn_matching::break_rw (int step) {
        // break if the random walk exceeds the rw_maxlen
        return (step >= config.rw_max_length) ? true : false;
}

bool rw_dyn_matching::cs_random_walk (NodeID start, std::vector<NodeID>& augpath, size_t& length) {
        return cs_random_walk(start, augpath, length, start);
}

bool rw_dyn_matching::cs_random_walk (NodeID start, std::vector<NodeID>& augpath, size_t& length, NodeID avoid) {
        return cs_random_walk (start, augpath, length, {avoid, avoid});
}

bool rw_dyn_matching::cs_random_walk (NodeID start, std::vector<NodeID>& augpath, size_t& length, std::pair<NodeID, NodeID> avoid) {
        int step = 0;
        length = 0;
        
        NodeID u = start;
        NodeID t = avoid.first;
        NodeID s = avoid.second;

        bool augpath_found = false;

        augpath[length++] = u;
        std::vector< NodeID > set_to_notmate;

        // perform random walk until break condition is met
        while (step < config.rw_max_length) {

                if( config.rw_low_degree_settle ) {
                        if (G->getNodeDegree(u) < config.rw_low_degree_value) {
                                NodeID v;

                                if (find_free(u, v, start)) {
                                        augpath[length++] = v;
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
                EdgeID new_matching_edge = random_functions::nextInt(0, max_index-1);
                NodeID v = G->getEdgeTarget(u, new_matching_edge);

                if (v == t || v == s) { // we don't want to go back to vertex t, therefore we choose a surrogate in this case
                        bool cont = surrogate(u, new_matching_edge, max_index, {t, s});

                        if (!cont) break;
                        v = G->getEdgeTarget(u, new_matching_edge);
                }

                if (!_rw_is_freecs(v)) {
                        // if the vertex is not free, then retrieve mate
                        NodeID w;
                        if (direct_new_mates[v] == NOMATE) {
                                w = mate(v);
                        } else {
                                w = direct_new_mates[v];
                        }

                        // add these vertices to the augmenting path
                        augpath[length++] = v;
                        augpath[length++] = w;

                        if( direct_new_mates[v] == NOMATE) set_to_notmate.push_back(v);
                        if( direct_new_mates[u] == NOMATE) set_to_notmate.push_back(u);
                        if( direct_new_mates[w] == NOMATE) set_to_notmate.push_back(w);

                        direct_new_mates[u] = v;
                        direct_new_mates[v] = u;
                        direct_new_mates[w] = w;

                        // step to next vertex
                        u = w;
                        t = v;

                        step += 2;
                } else {
                        // if the vertex v is free, we have found an endpoint
                        // to our augmenting path
                        augpath[length++] = v;
                        augpath_found = true;

                        step += 1;
                        break;
                }

        }

        if (!augpath_found) {
                if( config.rw_ending_additional_settle ) {
                        NodeID v;

                        // if no augmenting path was found, we perform a linear search
                        // through N(u). in case we find a free neigh-
                        // bor, we add this vertex to the augmenting path.
                        if (find_free(u, v, start)) {
                                augpath[length++] = v;
                                augpath_found = true;
                                set_to_notmate.push_back(v);
                        }
                } 
        }
        for( unsigned i = 0; i < set_to_notmate.size(); i++) {
                direct_new_mates[set_to_notmate[i]] = NOMATE;
        }

        return augpath_found;
}

void rw_dyn_matching::resolve_augpath (const std::vector<NodeID>& augpath, const size_t& length) {
        // an augmenting path is always of uneven length, therefore consists
        // of an even number of vertices
        // further it is at least of length 1, which means we need at
        // least two vertices
        ASSERT_TRUE((!(length%1)) && length >= 2);

        if (length == 2) {
                match(augpath.at(0), augpath.at(1));
        } else {
                NodeID u, v, w;
                size_t i;

                // (length of the augpath)-1 has to be even. for those first
                // steps, we assume every second edge to be matched
                for (i = 0; i < length-3; i+=2) {
                        u = augpath[i],
                        v = augpath[i+1],
                        w = augpath[i+2];

                        unmatch(v, w);
                        match(u,v);
                }

                match(augpath[i], augpath[i+1]);
        }
}

bool rw_dyn_matching::surrogate (NodeID source, EdgeID& new_matching_edge, EdgeID max_index, std::pair<NodeID, NodeID> avoid) {
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

bool rw_dyn_matching::_rw_is_freecs(NodeID u) {
        bool u_free = false;

        // if v exists in the new_mates hash-map, but points to itself, then it is free
        if (direct_new_mates[u] != NOMATE) {
                if (direct_new_mates[u] == u) {
                        u_free = true;
                } else {
                        u_free = false;
                }
        } else {
                u_free = is_free(u);
        }

        return u_free;
}


bool rw_dyn_matching::find_free (NodeID u, NodeID& v, NodeID t) {
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

                if (_rw_is_freecs(v)) {
                        mate_found = true;
                        break;
                }
        }

        return mate_found;
}


