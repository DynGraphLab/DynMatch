/******************************************************************************
 * gpa_matching.cpp 
 *
 * Source of GPA Matching Code 
 *
 ******************************************************************************
 * Copyright (C) 2013 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
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

#include "gpa_matching.h"
#include "compare_rating.h"
#include "../tools/macros_assertions.h"
#include "../tools/random_functions.h"
#include <algorithm>
#include <deque>

gpa_matching::gpa_matching() {

}

gpa_matching::~gpa_matching() {

}


void gpa_matching::match(const Config & config, 
                         graph_access & G, 
                         Matching & edge_matching) {

        edge_matching.resize(G.number_of_nodes());

        vector<EdgeID> edge_permutation;
        edge_permutation.reserve(G.number_of_edges());
        vector<NodeID> sources(G.number_of_edges());

        init(G, config, edge_matching, edge_permutation, sources);

        //permutation of the edges for random tie breaking
        Config gpa_perm_config = config;
        gpa_perm_config.permutation_quality = PERMUTATION_QUALITY_GOOD; 
        random_functions::permutate_entries(gpa_perm_config, edge_permutation, false);

        compare_rating cmp(&G);
        std::sort(edge_permutation.begin(), edge_permutation.end(), cmp);

        path_set pathset(&G, &config);

        //grow the paths
        forall_edges(G, e) {
                EdgeID curEdge = edge_permutation[e];
                NodeID source  = sources[curEdge];
                NodeID target  = G.getEdgeTarget(curEdge); 
                if(target < source) continue; // get rid of double edges

                pathset.add_if_applicable(source, curEdge);
        } endfor 

        extract_paths_apply_matching(G, sources, edge_matching, pathset); 
}        

void gpa_matching::init(graph_access & G, const Config & config, Matching & edge_matching, vector<EdgeID>  & edge_permutation, vector<NodeID> & sources) {

        forall_nodes(G, n) {
                edge_matching[n] = n;

                forall_out_edges(G, e, n) {
                        sources[e] = n;   
                        edge_permutation.push_back(e);
                } endfor
        } endfor


}
void gpa_matching::extract_paths_apply_matching( graph_access & G, 
                vector<NodeID> & sources,
                Matching & edge_matching, 
                path_set & pathset) {
        // extract the paths in the path set into lists of edges.
        // then, apply the dynamic programming max weight function to them. Apply 
        // the matched edges.
        double matching_rating, second_matching_rating;

        forall_nodes(G, n) {
                const path & p = pathset.get_path(n);

                if(not p.is_active()) {
                        continue;
                }
                if(p.get_tail() != n) {
                        continue;
                }
                if(p.get_length() == 0) {
                        continue;
                }

                if(p.get_head() == p.get_tail()) {
                        // ********************************
                        // handling cycles 
                        // ********************************
                        std::vector<EdgeID> a_matching, a_second_matching;
                        std::deque<EdgeID> unpacked_cycle;
                        unpack_path(p, pathset, unpacked_cycle);

                        EdgeID first = unpacked_cycle.front();
                        unpacked_cycle.pop_front();

                        maximum_weight_matching(G, 
                                        unpacked_cycle, 
                                        a_matching,
                                        matching_rating);

                        unpacked_cycle.push_front(first); 
                        EdgeID last = unpacked_cycle.back();
                        unpacked_cycle.pop_back();

                        maximum_weight_matching(G, 
                                        unpacked_cycle, 
                                        a_second_matching,
                                        second_matching_rating);

                        unpacked_cycle.push_back(last);

                        if(matching_rating > second_matching_rating) {
                                //apply first matching 
                                apply_matching(G, a_matching, sources, edge_matching); 
                        } else {
                                //apply second matching
                                apply_matching(G, a_second_matching, sources, edge_matching); 
                        }
                } else { 
                        // ********************************
                        // handling paths 
                        // ********************************
                        std::vector<EdgeID> a_matching;
                        std::vector<EdgeID> unpacked_path;

                        if(p.get_length() == 1) {
                                //match them directly
                                EdgeID e = 0;
                                if(pathset.next_vertex(p.get_tail()) == p.get_head()) {
                                        e = pathset.edge_to_next(p.get_tail());
                                } else {
                                        e = pathset.edge_to_prev(p.get_tail());
                                        ASSERT_TRUE( pathset.prev_vertex(p.get_tail()) == p.get_head() );
                                }

                                NodeID source = sources[e];
                                NodeID target = G.getEdgeTarget(e);

                                edge_matching[source] = target;
                                edge_matching[target] = source;

                                continue;                
                        }
                        unpack_path(p, pathset, unpacked_path);
                        //dump_unpacked_path(G, unpacked_path, sources); 

                        double final_rating = 0;
                        maximum_weight_matching(G, unpacked_path, a_matching, final_rating);

                        //apply matched edges
                        apply_matching(G, a_matching, sources, edge_matching); 
                } 
        } endfor
}


void gpa_matching::apply_matching( graph_access & G,
                vector<EdgeID> & matched_edges, 
                vector<NodeID> & sources,
                Matching & edge_matching) {

        //apply matched edges 
        for( unsigned i = 0; i < matched_edges.size(); i++) {
                EdgeID e = matched_edges[i];
                NodeID source = sources[e];
                NodeID target = G.getEdgeTarget(e);

                edge_matching[source] = target;
                edge_matching[target] = source;
        }

}
template <typename VectorOrDeque> 
void gpa_matching::unpack_path(const path & p, const path_set & pathset, VectorOrDeque & unpacked_path ) {
        NodeID head = p.get_head(); 
        NodeID prev = p.get_tail();
        NodeID next;
        NodeID current = prev; 

        if(prev == head) {
                //special case: the given path is a cycle
                current = pathset.next_vertex(prev);
                unpacked_path.push_back(pathset.edge_to_next(prev));
        }

        while(current != head) {
                if(pathset.next_vertex(current) == prev) {
                        next =  pathset.prev_vertex(current);              
                        unpacked_path.push_back(pathset.edge_to_prev(current));
                } else {
                        next =  pathset.next_vertex(current);              
                        unpacked_path.push_back(pathset.edge_to_next(current));
                }
                prev = current;
                current = next;
        }
}

template <typename VectorOrDeque> 
void gpa_matching::maximum_weight_matching( graph_access & G,
                VectorOrDeque & unpacked_path, 
                vector<EdgeID> & matched_edges,
                double & final_rating) {

        unsigned k = unpacked_path.size();
        if( k == 1 ) {
              matched_edges.push_back(unpacked_path[0]);
              return;
        }

        std::vector<double> ratings(k, 0.0);
        std::vector< bool > decision(k, false);

        ratings[0] = G.getEdgeWeight(unpacked_path[0]);
        ratings[1] = G.getEdgeWeight(unpacked_path[1]);

        decision[0] = true;
        if(ratings[0] < ratings[1]) {
                decision[1] = true;
        }
        //build up the decision vector
        for( EdgeID i = 2; i < k; i++) {
                ASSERT_TRUE(unpacked_path[i] < G.number_of_edges());
                double curRating = G.getEdgeWeight(unpacked_path[i]);
                if( curRating + ratings[i-2] > ratings[i-1] ) {
                        decision[i] = true;
                        ratings[i]  = curRating + ratings[i-2];
                } else {
                        decision[i] = false;
                        ratings[i]  = ratings[i-1];
                } 
        }

        if(decision[k-1]) {
                final_rating = ratings[k-1];
        } else {
                final_rating = ratings[k-2];
        }
        //construct optimal solution 
        for(int i = k-1; i >= 0;) {
                if(decision[i]) {
                        matched_edges.push_back(unpacked_path[i]);
                        i-=2;        
                } else {
                        i-=1;
                }
        }
} 

template <typename VectorOrDeque> 
void gpa_matching::dump_unpacked_path( graph_access & G,
                VectorOrDeque & unpacked_path,
                vector<NodeID>& sources) {
        //dump the path
        for( unsigned i = 0; i < unpacked_path.size(); i++) {
                EdgeID e = unpacked_path[i];
                cout << "(" << sources[e] << " " << G.getEdgeTarget(e) << ") ";
        }
        cout << endl;


}
