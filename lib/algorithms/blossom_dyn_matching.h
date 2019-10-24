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

#ifndef MCM_DYN_MATCHING_H
#define MCM_DYN_MATCHING_H

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

#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

class blossom_dyn_matching : public dyn_matching {
        public:
                typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> mygraph;
                typedef std::vector<boost::graph_traits<mygraph>::vertex_descriptor> matemap;
                typedef boost::property_map<mygraph, boost::vertex_index_t>::type vmi;

                blossom_dyn_matching (dyn_graph_access* G, MatchConfig & config)
                : dyn_matching(G, config), kernel(create<mygraph, matemap, boost::graph_traits<mygraph>::vertex_descriptor*, vmi>(G->number_of_nodes(), config.rw_max_length)) {

                }

                virtual bool new_edge(NodeID source, NodeID target) {
                        return kernel.new_edge(source, target);
                }

                virtual bool remove_edge(NodeID source, NodeID target) {
                        return false;
                }

                virtual void retry () {
                        
                }

                virtual NodeID getMSize () {
                        return kernel.matching_size;
                }

        protected:
                template<typename Graph, typename MateMap, typename VertexIndexMap>
                class dyn_blossom {
                        //generates the type of an iterator property map from vertices to type X
                        template <typename X>
                        struct map_vertex_to_ {
                                typedef boost::iterator_property_map<typename std::vector<X>::iterator,
                                        VertexIndexMap> type;
                        };

                        typedef typename boost::graph_traits<Graph>::vertex_descriptor vertex_descriptor_t;
                        typedef typename std::pair<vertex_descriptor_t, vertex_descriptor_t> vertex_pair_t;
                        typedef typename boost::graph_traits<Graph>::edge_descriptor edge_descriptor_t;
                        typedef typename boost::graph_traits<Graph>::vertices_size_type v_size_t;
                        typedef typename boost::graph_traits<Graph>::edges_size_type e_size_t;
                        typedef typename boost::graph_traits<Graph>::vertex_iterator vertex_iterator_t;
                        typedef typename boost::graph_traits<Graph>::out_edge_iterator out_edge_iterator_t;
                        typedef typename std::deque<vertex_descriptor_t> vertex_list_t;
                        typedef typename std::vector<edge_descriptor_t> edge_list_t;
                        typedef typename map_vertex_to_<vertex_descriptor_t>::type vertex_to_vertex_map_t;
                        typedef typename map_vertex_to_<int>::type vertex_to_int_map_t;
                        typedef typename map_vertex_to_<vertex_pair_t>::type vertex_to_vertex_pair_map_t;
                        typedef typename map_vertex_to_<v_size_t>::type vertex_to_vsize_map_t;
                        typedef typename map_vertex_to_<e_size_t>::type vertex_to_esize_map_t;

                        public:
                                dyn_blossom (const Graph& arg_g, MateMap arg_mate,
                                             VertexIndexMap arg_vm, long max_level = std::numeric_limits<long>::max()) :
                                        matching_size(0),
                                        g(arg_g),
                                        mate(mate_vector.begin(), vm),
                                        mate_vector(n_vertices),
                                        ancestor_of_v(ancestor_of_v_vector.begin(), vm),
                                        ancestor_of_v_vector(n_vertices),
                                        ancestor_of_w(ancestor_of_w_vector.begin(), vm),
                                        ancestor_of_w_vector(n_vertices),
                                        vertex_state(vertex_state_vector.begin(), vm),
                                        vertex_state_vector(n_vertices),
                                        origin(origin_vector.begin(), vm),
                                        origin_vector(n_vertices),
                                        pred(pred_vector.begin(), vm),
                                        pred_vector(n_vertices),
                                        bridge(bridge_vector.begin(), vm),
                                        bridge_vector(n_vertices),
                                        n_vertices(num_vertices(arg_g)),
                                        vm(arg_vm)

                                {
                                        std::cout <<  "here"  << std::endl;
                                        if (max_level < 0) {
                                                this->max_level = std::numeric_limits<e_size_t>::max();
                                        } else {
                                                this->max_level = max_level;
                                        }
                                        std::cout <<  "here"  << std::endl;
                                        
                                        vertex_iterator_t vi, vi_end;
                                        for(boost::tie(vi,vi_end) = vertices(g); vi != vi_end; ++vi) {
                                        std::cout <<  "here"  << std::endl;
                                                vertex_descriptor_t u = *vi;
                                        //std::cout <<  "here " << u  << std::endl;
                                        //std::cout <<  "here " << mate.size()  << std::endl;
                                                mate[u] = get(arg_mate, u);
                                        std::cout <<  "here"  << std::endl;
                                                
                                                if (mate[u] == boost::graph_traits<Graph>::null_vertex()) {
                                                        vertex_state[u] = boost::graph::detail::V_EVEN;
                                                } else {
                                                        vertex_state[u] = boost::graph::detail::V_UNREACHED;
                                                }
                                                
                                        std::cout <<  "here"  << std::endl;
                                                origin[u] = u;
                                        std::cout <<  "here"  << std::endl;
                                                pred[u] = u;
                                        std::cout <<  "here"  << std::endl;
                                                ancestor_of_v[u] = 0;
                                        std::cout <<  "here"  << std::endl;
                                                ancestor_of_w[u] = 0;
                                        std::cout <<  "here"  << std::endl;
                                        }
                                        std::cout <<  "here"  << std::endl;
                                }

                                bool new_edge (NodeID u, NodeID v) {
                                        using namespace boost;
                                        edge_descriptor_t current_edge = add_edge(u, v, g).first;

                                        vertex_descriptor_t vd_u = source(current_edge, g);
                                        vertex_descriptor_t vd_v = target(current_edge, g);

                                        bool left = augment_matching(vd_u);
                                        bool right = augment_matching(vd_v);
                                        
                                        return left || right;
                                }

                                bool augment_matching(vertex_descriptor_t start) {
                                        using namespace boost;
                                        if (mate[start] != graph_traits<Graph>::null_vertex()) {
                                                return false;
                                        }
                                        
                                        // perform reset of previously changed values
                                        for (auto u : reset) {
                                                origin[u] = u;
                                                pred[u] = u;
                                                ancestor_of_v[u] = 0;
                                                ancestor_of_w[u] = 0;
                                                if (mate[u] == graph_traits<Graph>::null_vertex()) {
                                                        vertex_state[u] = graph::detail::V_EVEN;
                                                } else {
                                                        vertex_state[u] = graph::detail::V_UNREACHED;
                                                }
                                        }
                                        reset.clear();
                                        ds.Reset();

                                        e_size_t timestamp = 0;
                                        even_edges.clear();

                                        // Count on first level how many new even edges are inserted to the queue,
                                        // next keep counting how many edges have been dequeued and if all edges
                                        // which were added on first level are processed, then we have reached a new level.
                                        // While processing the initial level, count how many edges are being added
                                        // on the subsequent levels. When reaching a new level, swap the contents of
                                        // the counters.
                                        e_size_t count_0 = 0;
                                        e_size_t count_1 = 0;
                                        e_size_t level   = 0;

                                        out_edge_iterator_t ei, ei_end;
                                        for(boost::tie(ei,ei_end) = out_edges(start,g); ei != ei_end; ++ei) {
                                                if (target(*ei,g) != start) {
                                                        even_edges.push_back( *ei );
                                                        count_0++;
                                                }
                                        }
                                        
                                        //end initializations

                                        vertex_descriptor_t v,w,w_free_ancestor,v_free_ancestor;
                                        w_free_ancestor = graph_traits<Graph>::null_vertex();
                                        v_free_ancestor = graph_traits<Graph>::null_vertex(); 
                                        bool found_alternating_path = false;

                                        while(!even_edges.empty() && !found_alternating_path && level < max_level) {
                                                // since we push even edges onto the back of the list as
                                                // they're discovered, taking them off the back will search
                                                // for augmenting paths depth-first.
                                                edge_descriptor_t current_edge = even_edges.back();
                                                even_edges.pop_back();

                                                v = source(current_edge,g);
                                                w = target(current_edge,g);

                                                vertex_descriptor_t v_prime = origin[ds.Find(v)];
                                                vertex_descriptor_t w_prime = origin[ds.Find(w)];

                                                // because of the way we put all of the edges on the queue,
                                                // v_prime should be labeled V_EVEN; the following is a
                                                // little paranoid but it could happen...
                                                if (vertex_state[v_prime] != graph::detail::V_EVEN) {
                                                        std::swap(v_prime,w_prime);
                                                        std::swap(v,w);
                                                }

                                                if (vertex_state[w_prime] == graph::detail::V_UNREACHED) {
                                                        vertex_state[w_prime] = graph::detail::V_ODD;
                                                        vertex_descriptor_t w_prime_mate = mate[w_prime];
                                                        vertex_state[w_prime_mate] = graph::detail::V_EVEN;
                                                        out_edge_iterator_t ei, ei_end;
                                                        for( boost::tie(ei,ei_end) = out_edges(w_prime_mate, g); ei != ei_end; ++ei) {
                                                                if (target(*ei,g) != w_prime_mate) {
                                                                        even_edges.push_back(*ei);
                                                                        ++count_1;
                                                                }
                                                        }
                                                        pred[w_prime] = v;
                                                        reset.insert(w_prime);
                                                        reset.insert(w_prime_mate);
                                                }

                                                //w_prime == v_prime can happen below if we get an edge that has been
                                                //shrunk into a blossom
                                                else if (vertex_state[w_prime] == graph::detail::V_EVEN && w_prime != v_prime) {
                                                        vertex_descriptor_t w_up = w_prime;
                                                        vertex_descriptor_t v_up = v_prime;
                                                        vertex_descriptor_t nearest_common_ancestor 
                                                        = graph_traits<Graph>::null_vertex();
                                                        w_free_ancestor = graph_traits<Graph>::null_vertex();
                                                        v_free_ancestor = graph_traits<Graph>::null_vertex();

                                                        // We now need to distinguish between the case that
                                                        // w_prime and v_prime share an ancestor under the
                                                        // "parent" relation, in which case we've found a
                                                        // blossom and should shrink it, or the case that
                                                        // w_prime and v_prime both have distinct ancestors that
                                                        // are free, in which case we've found an alternating
                                                        // path between those two ancestors.

                                                        ++timestamp;

                                                        while (nearest_common_ancestor == graph_traits<Graph>::null_vertex() && 
                                                               (v_free_ancestor == graph_traits<Graph>::null_vertex() || 
                                                                w_free_ancestor == graph_traits<Graph>::null_vertex())
                                                        ) {
                                                                ancestor_of_w[w_up] = timestamp;
                                                                ancestor_of_v[v_up] = timestamp;
                                                                reset.insert(w_up);
                                                                reset.insert(v_up);

                                                                if (w_free_ancestor == graph_traits<Graph>::null_vertex())
                                                                        w_up = parent(w_up);
                                                                if (v_free_ancestor == graph_traits<Graph>::null_vertex())
                                                                        v_up = parent(v_up);

                                                                if (mate[v_up] == graph_traits<Graph>::null_vertex())
                                                                        v_free_ancestor = v_up;
                                                                if (mate[w_up] == graph_traits<Graph>::null_vertex())
                                                                        w_free_ancestor = w_up;

                                                                if (ancestor_of_w[v_up] == timestamp)
                                                                        nearest_common_ancestor = v_up;
                                                                else if (ancestor_of_v[w_up] == timestamp)
                                                                        nearest_common_ancestor = w_up;
                                                                else if (v_free_ancestor == w_free_ancestor && 
                                                                         v_free_ancestor != graph_traits<Graph>::null_vertex())
                                                                        nearest_common_ancestor = v_up;
                                                        }

                                                        if (nearest_common_ancestor == graph_traits<Graph>::null_vertex())
                                                                found_alternating_path = true; //to break out of the loop
                                                        else {
                                                                //shrink the blossom
                                                                link_and_set_bridges(w_prime, nearest_common_ancestor, std::make_pair(w,v));
                                                                link_and_set_bridges(v_prime, nearest_common_ancestor, std::make_pair(v,w));
                                                        }
                                                }
                                                
                                                // When we have processed all edges, that were added during one
                                                // level, then we have finished another level and are moving on
                                                // to check the next level. hence, we reset our counters, to newly
                                                // count down the number of edges on the newly reached level.
                                                if (--count_0 == 0) {
                                                        count_0 = count_1;
                                                        count_1 = 0;
                                                        ++level;
                                                }
                                        }

                                        if (!found_alternating_path) {
                                                return false;
                                        }

                                        // retrieve the augmenting path and put it in aug_path
                                        reversed_retrieve_augmenting_path(v, v_free_ancestor);
                                        retrieve_augmenting_path(w, w_free_ancestor);

                                        // augment the matching along aug_path
                                        vertex_descriptor_t a,b;
                                        while (!aug_path.empty()) {
                                                a = aug_path.front();
                                                aug_path.pop_front();
                                                b = aug_path.front();
                                                aug_path.pop_front();
                                                mate[a] = b;
                                                mate[b] = a;
                                                
                                                reset.insert(a);
                                                reset.insert(b);
                                        }

                                        ++matching_size;
                                        return true;
                                }


                                template <typename PropertyMap>
                                void get_current_matching(PropertyMap pm)
                                {
                                        vertex_iterator_t vi,vi_end;
                                        for(boost::tie(vi,vi_end) = vertices(g); vi != vi_end; ++vi)
                                                put(pm, *vi, mate[*vi]);
                                }




                                template <typename PropertyMap>
                                void get_vertex_state_map(PropertyMap pm)
                                {
                                        vertex_iterator_t vi,vi_end;
                                        for(boost::tie(vi,vi_end) = vertices(g); vi != vi_end; ++vi)
                                                put(pm, *vi, vertex_state[origin[ds.Find(*vi)]]);
                                }



                                e_size_t matching_size;

                        private:

                                vertex_descriptor_t parent(vertex_descriptor_t x) {
                                        using namespace boost;
                                        if (vertex_state[x] == graph::detail::V_EVEN
                                                        && mate[x] != graph_traits<Graph>::null_vertex())
                                                return mate[x];
                                        else if (vertex_state[x] == graph::detail::V_ODD) {
                                                return origin[ds.Find(pred[x])];
                                        } else
                                                return x;
                                }




                                void link_and_set_bridges(vertex_descriptor_t x,
                                                          vertex_descriptor_t stop_vertex,
                                                          vertex_pair_t the_bridge)
                                {
                                        using namespace boost;
                                        for(vertex_descriptor_t v = x; v != stop_vertex; v = parent(v))
                                        {
                                                ds.Union(v, stop_vertex);
                                                origin[ds.Find(stop_vertex)] = stop_vertex;
                                                reset.insert(ds.Find(stop_vertex));

                                                if (vertex_state[v] == graph::detail::V_ODD)
                                                {
                                                        bridge[v] = the_bridge;
                                                        out_edge_iterator_t oei, oei_end;
                                                        for(boost::tie(oei, oei_end) = out_edges(v,g); oei != oei_end; ++oei)
                                                        {
                                                                if (target(*oei,g) != v)
                                                                {
                                                                        even_edges.push_back(*oei);
                                                                }
                                                        }
                                                }
                                        }
                                }


                                // Since none of the STL containers support both constant-time
                                // concatenation and reversal, the process of expanding an
                                // augmenting path once we know one exists is a little more
                                // complicated than it has to be. If we know the path is from v to
                                // w, then the augmenting path is recursively defined as:
                                //
                                // path(v,w) = [v], if v = w
                                //           = concat([v, mate[v]], path(pred[mate[v]], w),
                                //                if v != w and vertex_state[v] == graph::detail::V_EVEN
                                //           = concat([v], reverse(path(x,mate[v])), path(y,w)),
                                //                if v != w, vertex_state[v] == graph::detail::V_ODD, and bridge[v] = (x,y)
                                //
                                // These next two mutually recursive functions implement this definition.

                                void retrieve_augmenting_path(vertex_descriptor_t v, vertex_descriptor_t w)
                                {
                                        using namespace boost;
                                        if (v == w)
                                                aug_path.push_back(v);
                                        else if (vertex_state[v] == graph::detail::V_EVEN)
                                        {
                                                aug_path.push_back(v);
                                                aug_path.push_back(mate[v]);
                                                retrieve_augmenting_path(pred[mate[v]], w);
                                        }
                                        else //vertex_state[v] == graph::detail::V_ODD
                                        {
                                                aug_path.push_back(v);
                                                reversed_retrieve_augmenting_path(bridge[v].first, mate[v]);
                                                retrieve_augmenting_path(bridge[v].second, w);
                                        }
                                }


                                void reversed_retrieve_augmenting_path(vertex_descriptor_t v,
                                                                       vertex_descriptor_t w)
                                {
                                        using namespace boost;

                                        if (v == w)
                                                aug_path.push_back(v);
                                        else if (vertex_state[v] == graph::detail::V_EVEN)
                                        {
                                                reversed_retrieve_augmenting_path(pred[mate[v]], w);
                                                aug_path.push_back(mate[v]);
                                                aug_path.push_back(v);
                                        }
                                        else //vertex_state[v] == graph::detail::V_ODD
                                        {
                                                reversed_retrieve_augmenting_path(bridge[v].second, w);
                                                retrieve_augmenting_path(bridge[v].first, mate[v]);
                                                aug_path.push_back(v);
                                        }
                                }


                                Graph g;
                                e_size_t max_level;

                                //storage for the property maps below
                                //~ std::vector<vertex_descriptor_t> ds_parent_vector;
                                //~ std::vector<v_size_t> ds_rank_vector;

                                //iterator property maps
                                vertex_to_vertex_map_t mate;
                                std::vector<vertex_descriptor_t> mate_vector;

                                vertex_to_esize_map_t ancestor_of_v;
                                std::vector<e_size_t> ancestor_of_v_vector;

                                vertex_to_esize_map_t ancestor_of_w;
                                std::vector<e_size_t> ancestor_of_w_vector;

                                vertex_to_int_map_t vertex_state;
                                std::vector<int> vertex_state_vector;

                                vertex_to_vertex_map_t origin;
                                std::vector<vertex_descriptor_t> origin_vector;

                                vertex_to_vertex_map_t pred;
                                std::vector<vertex_descriptor_t> pred_vector;

                                vertex_to_vertex_pair_map_t bridge;
                                std::vector<vertex_pair_t> bridge_vector;

                                v_size_t n_vertices;
                                VertexIndexMap vm;
                                
                                union_find ds;
                                std::unordered_set<vertex_descriptor_t> reset;
                                
                                vertex_list_t aug_path;
                                edge_list_t even_edges;
                        };

                        template <typename Graph, typename MateMap, typename RawMateMap, typename VertexIndexMap>
                        dyn_blossom<Graph, RawMateMap, VertexIndexMap> create (unsigned long n, long max_level) {
                                using namespace boost;
                                typedef typename graph_traits<mygraph>::vertex_iterator v_desc;
                                
                                std::cout <<  "n " << n  << std::endl;
                                std::cout <<  "max " << max_level  << std::endl;
                                Graph arg_g(n);
                                MateMap arg_m(n);
                                
                                v_desc vi, vi_end;
                                for(boost::tie(vi,vi_end) = vertices(arg_g); vi != vi_end; ++vi) {
                                        arg_m[*vi] = graph_traits<Graph>::null_vertex();
                                }
                                
                                return dyn_blossom<Graph, RawMateMap, VertexIndexMap>(arg_g, &arg_m[0], get(vertex_index, arg_g), max_level);
                        }
                        
                dyn_blossom< mygraph, boost::graph_traits<mygraph>::vertex_descriptor*, vmi> kernel;
};

#endif // MCM_DYN_MATCHING_H
