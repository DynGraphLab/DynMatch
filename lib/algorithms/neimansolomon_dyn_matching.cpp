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

#include "neimansolomon_dyn_matching.h"
#include "random_functions.h"

//========================// Fv struct methods //========================//
neimansolomon_dyn_matching::Fv::Fv () {
}

bool neimansolomon_dyn_matching::Fv::insert (NodeID node) {
        if (free.find(node) != free.end()) {
                return false;
        }

        free.insert({node, node});

        return true;
}

bool neimansolomon_dyn_matching::Fv::remove (NodeID node) {
        if (free.find(node) == free.end()) {
                return false;
        }

        free.erase(node);

        return true;
}

bool neimansolomon_dyn_matching::Fv::has_free () {
        return !free.empty();
}

NodeID neimansolomon_dyn_matching::Fv::get_free () { // returns false if there is no free, and true + reference when there is a free neighbour
        if (!has_free()) return NOMATE;

        return free.begin()->first;
}


neimansolomon_dyn_matching::neimansolomon_dyn_matching (dyn_graph_access* G, MatchConfig & config) : dyn_matching(G, config), F_max(G->number_of_nodes(), G->number_of_nodes()) {
        // F is a n-size vector of empty Fv-data structures, where n denotes the number of nodes in G
        F.resize(G->number_of_nodes());
}

bool neimansolomon_dyn_matching::new_edge(NodeID source, NodeID target) {
        // first add the node to the data structure G;
        G->new_edge(source, target);
        G->new_edge(target, source);

        ASSERT_TRUE(foo == bar);
        handle_addition(source, target);
        check_invariants();

        return true;
}

void neimansolomon_dyn_matching::handle_addition (NodeID u, NodeID v) {
        if (F_max.contains(u)) {
                F_max.changeKey(u, deg(u));
        } else if (is_free(u)) {
                F_max.insert(u, deg(u));
        }

        if (F_max.contains(v)) {
                F_max.changeKey(v, deg(v));
        } else if (is_free(v)) {
                F_max.insert(v, deg(v));
        }

        if (is_free(u) && is_free(v)) { // if both are free...
                match(u, v);                 // match them!
        
        } else if (is_free(u) || is_free(v)) {
                // if u is free, then the code works out fine,
                // otherwise we simply swap u and v, in order
                // to stay as near as possible to the papers notation
                if (is_free(v)) {
                        NodeID tmp = u;
                        u = v;
                        v = tmp;
                }

                NodeID v_ = mate(v);
                ASSERT_TRUE(v != NOMATE); // save the mate of v in v_
                // and assert, that v has a mate
                // notice, that since we change
                // u and v so that u is the free
                // node, v always has to be matched
                F[v_].remove(u);

                if (has_free(v_)) {
                        unmatch(v, v_);

                        // switch from u - v---v_ - v_free (<- augh-path /w len 3)
                        // to          u---v - v_---v_free
                        match(u, v);
                        match(v_, get_free(v_));
                } else {
                        // tell all neighbours of u, that u is free
                        for( unsigned int e = 0; e < G->getNodeDegree(u); e++) {
                                F[G->getEdgeTarget(u,e)].insert(u);
                        }
                }
        } else {
                // do nothing
        }

        handle_problematic();
}

bool neimansolomon_dyn_matching::remove_edge(NodeID source, NodeID target) {
        G->remove_edge(source, target);
        G->remove_edge(target, source);

        handle_deletion(source, target);
        check_invariants();

        return true;
}

bool neimansolomon_dyn_matching::has_free (NodeID node) {
        return F[node].has_free();
}

NodeID neimansolomon_dyn_matching::get_free (NodeID node) {
        NodeID free_node;
        // since the described algorithm always first checks has_free
        // before calling get_free, I'm going to do it likewise.
        // therefore I assume and assert, that get_free has found
        // a free node
        ASSERT_TRUE(has_free(node));
        free_node = F[node].get_free();

        return free_node;
}

void neimansolomon_dyn_matching::match (NodeID u, NodeID v) {
        ASSERT_TRUE(is_free(u));
        ASSERT_TRUE(is_free(v));
        
        // remove u and v from F_max, since when matched, they're not free anymore
        if (F_max.contains(u)) {
                F_max.deleteNode(u);
        }
        if (F_max.contains(v)) {
                F_max.deleteNode(v);
        }

        for (NodeID w : {u, v}) {
                if (is_free(w)) {
                        for( unsigned int e = 0; e < G->getNodeDegree(w); e++) {
                                F[G->getEdgeTarget(w,e)].remove(w);
                        }
                }
        }
        
        matching[u] = v;
        matching[v] = u;
        
        matching_size += 2;
        
        ASSERT_TRUE(!F_max.contains(u));
        ASSERT_TRUE(!F_max.contains(v));

        ASSERT_TRUE(!is_free(u));
        ASSERT_TRUE(!is_free(v));
}


NodeID neimansolomon_dyn_matching::deg (NodeID u) {
        return (NodeID) G->getNodeDegree(u);
}

NodeID neimansolomon_dyn_matching::threshold () {
        return std::sqrt(2 * G->number_of_edges());
}

bool neimansolomon_dyn_matching::aug_path (NodeID u) {
        ASSERT_TRUE(!has_free(u));
        ASSERT_TRUE(is_free(u));
        ASSERT_TRUE(deg(u) <= std::sqrt(2*G->number_of_nodes() + 2*G->number_of_edges()));

        NodeID w, w_, x;
        bool aug_path_found = false;

        //TODO: check performance
        //for (DynEdge dummy : G->getEdgesFromNode(u)) {
        for( unsigned int e = 0; e < G->getNodeDegree(u); e++) {
                w = G->getEdgeTarget(u,e);
                // since we assume that u has no free neighbours,
                // all neighbours have to be matched and therefore
                // have a mate
                w_ = mate(w);
                ASSERT_TRUE(w_ != NOMATE);
                
                if (has_free(w_)) {
                        x = get_free(w_);
                        aug_path_found = true;
                        break;
                }
        }

        if (aug_path_found) {
                // here I swapped the statements from the original paper due to technical limitations
                unmatch(w, w_);

                match(u, w);
                match(w_, x);
        } else {
                //TODO: check performance
                for( unsigned int e = 0; e < G->getNodeDegree(u); e++) {
                        F[G->getEdgeTarget(u,e)].insert(u);
                }
                
                if (F_max.contains(u)) {
                        F_max.changeKey(u, deg(u));
                } else {
                        F_max.insert(u, deg(u));
                }
                ASSERT_TRUE(is_free(u));
        }
        
        return aug_path_found;
}

NodeID neimansolomon_dyn_matching::surrogate (NodeID u) {
        ASSERT_TRUE(!has_free(u));

        NodeID w  = u;
        NodeID w_ = u;

        for( unsigned int e = 0; e < G->getNodeDegree(u); e++) {
                w = G->getEdgeTarget(u,e);
                // since we assume that u has no free neighbours,
                // all neighbours have to be matched and therefore
                // have a mate
                w_ = mate(w);
                ASSERT_TRUE(w_ != NOMATE);
                
                if (deg(w_) <= threshold()) break;
        }

        ASSERT_TRUE(is_matched(w,w_));
        unmatch(w, w_);

        ASSERT_TRUE(is_free(w_));

        match(u, w);

        ASSERT_TRUE(is_free(w_));

        return w_;
}

void neimansolomon_dyn_matching::handle_deletion (NodeID u, NodeID v) {
        if (F_max.contains(u)) {
                F_max.changeKey(u, deg(u));
        }
        if (F_max.contains(v)) {
                F_max.changeKey(v, deg(v));
        }

        // the edge (u,v) is not in the matching if the mate of u is not v and
        // vice versa
        if (
                (matching[u] != v) && 
                (matching[v] != u)
        ) {
                if (is_free(u)) {
                        F[v].remove(u);
                }

                if (is_free(v)) {
                        F[u].remove(v);
                }
        } else { // (u,v) is in the matching
                ASSERT_TRUE(is_matched(u,v));

                unmatch(u, v);

                bool surrogated = false;
                for (NodeID z : {
                        u, v
                }) {
                        do { // auxiliary while-loop in order to jump back
                                // to 3(b)i. on surrogation
                                surrogated = false;

                                if (has_free(z)) {
                                        match (z, get_free(z));
                                } else {
                                        if (deg(z) > threshold()) {
                                                z = surrogate(z);
                                                surrogated = true;
                                        } else {
                                                aug_path(z);
                                        }
                                }
                        } while (surrogated);
                }
        }

        handle_problematic();
}

void neimansolomon_dyn_matching::handle_problematic () {
        if (!F_max.empty()) {
                NodeID u = F_max.maxElement();

                if (deg(u) <= std::sqrt(2 * G->number_of_nodes())) return;

                ASSERT_TRUE(is_free(u));

                NodeID s_u = surrogate(u);

                ASSERT_TRUE(is_free(s_u));

                if (!aug_path(s_u)) {
                     got_free.push_back(s_u);   
                }
        }
}

