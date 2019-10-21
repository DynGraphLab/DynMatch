/******************************************************************************
 * en_dyn_matching.cpp
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

#include "en_dyn_matching.h"

#ifdef DM_COUNTERS
#include "counters.h"
#endif


en_dyn_matching::en_dyn_matching (dyn_graph_access* G, double eps, unsigned multiple_rws) : rw_dyn_matching(G, eps, multiple_rws) {
#ifdef DM_COUNTERS
        counters::create("en");
        counters::get("en").create("match()");
#endif
}

void en_dyn_matching::handle_insertion (NodeID source, NodeID target) {
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

                if (settle(v)) {
                        return;
                }

                unsigned i = 0;
                //~ std::vector<NodeID> augpath;
                bool augpath_found = false;
                size_t length = 0;

                while (i < multiple_rws && !augpath_found) {
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

void en_dyn_matching::handle_deletion (NodeID source, NodeID target) {
        if (is_matched(source, target)) {
                unmatch(source, target);

                ASSERT_TRUE(!is_matched(source, target));
                ASSERT_TRUE(is_free(source));
                ASSERT_TRUE(is_free(target));

                // first try naive settle. if naive settle works out, don't do anything else.
                // if the vertex remains free, we perform a random walk
                settle(source);

                if (is_free(source)) {
                        unsigned i = 0;
                        //~ std::vector<NodeID> augpath;
                        bool augpath_found = false;
                        size_t length = 0;

                        while (i < multiple_rws && !augpath_found) {
                                augpath_found = cs_random_walk(source, augpath, length);
                                i++;
                        }

                        if (augpath_found) {
                                resolve_augpath(augpath, length);
                        }
                }

                if (is_free(target)) { // the random walk on source could match target
                        // therefore only settle target, if it's still free
                        settle(target);

                        if (is_free(target)) {
                                unsigned i = 0;
                                //~ std::vector<NodeID> augpath;
                                bool augpath_found = false;
                                size_t length = 0;

                                while (i < multiple_rws && !augpath_found) {
                                        augpath_found = cs_random_walk(target, augpath, length);
                                        i++;
                                }

                                if (augpath_found) {
                                        resolve_augpath(augpath, length);
                                }
                        }
                }
        }
}

bool en_dyn_matching::settle (NodeID u) {
        EdgeID deg_u = G->get_first_invalid_edge(u);
        NodeID v;
        bool mate_found = false;

        for (EdgeID i = 0; i < deg_u; ++i) {
                v = G->getEdgeTarget(u, i);
                if (is_free(v)) {
                        mate_found = true;
                        break;
                }
        }

        if (mate_found) {
                match(u, v);
        }

        return mate_found;
}

void en_dyn_matching::counters_next() {
#ifdef DM_COUNTERS
        counters::get("en").get("match()").next();
#endif
}
