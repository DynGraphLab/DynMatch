/*
 * customized dynamic blossom algorithm based upon boost implementation.
 * 
 */

#ifndef DYN_BLOSSOM_H
#define DYN_BLOSSOM_H

namespace FOO {
using namespace boost;

template<typename Graph, typename MateMap, typename VertexIndexMap>
class dyn_blossom {
        public:
                //generates the type of an iterator property map from vertices to type X
                template <typename X>
                struct map_vertex_to_
                {
                        typedef boost::iterator_property_map<typename std::vector<X>::iterator,
                                VertexIndexMap> type;
                };

                typedef typename graph_traits<Graph>::vertex_descriptor
                vertex_descriptor_t;
                typedef typename std::pair< vertex_descriptor_t, vertex_descriptor_t >
                vertex_pair_t;
                typedef typename graph_traits<Graph>::edge_descriptor edge_descriptor_t;
                typedef typename graph_traits<Graph>::vertices_size_type v_size_t;
                typedef typename graph_traits<Graph>::edges_size_type e_size_t;
                typedef typename graph_traits<Graph>::vertex_iterator vertex_iterator_t;
                typedef typename graph_traits<Graph>::out_edge_iterator
                out_edge_iterator_t;
                typedef typename std::deque<vertex_descriptor_t> vertex_list_t;
                typedef typename std::vector<edge_descriptor_t> edge_list_t;
                typedef typename map_vertex_to_<vertex_descriptor_t>::type
                vertex_to_vertex_map_t;
                typedef typename map_vertex_to_<int>::type vertex_to_int_map_t;
                typedef typename map_vertex_to_<vertex_pair_t>::type
                vertex_to_vertex_pair_map_t;
                typedef typename map_vertex_to_<v_size_t>::type vertex_to_vsize_map_t;
                typedef typename map_vertex_to_<e_size_t>::type vertex_to_esize_map_t;

                dyn_blossom (const Graph& arg_g, MateMap arg_mate,
                             VertexIndexMap arg_vm, e_size_t max_level = std::numeric_limits<e_size_t>::max()) :
                        g(arg_g),
                        vm(arg_vm),
                        n_vertices(num_vertices(arg_g)),
                        matching_size(0),

                        mate_vector(n_vertices),
                        ancestor_of_v_vector(n_vertices),
                        ancestor_of_w_vector(n_vertices),
                        vertex_state_vector(n_vertices),
                        origin_vector(n_vertices),
                        pred_vector(n_vertices),
                        bridge_vector(n_vertices),
                        ds_parent_vector(n_vertices),
                        ds_rank_vector(n_vertices),

                        mate(mate_vector.begin(), vm),
                        ancestor_of_v(ancestor_of_v_vector.begin(), vm),
                        ancestor_of_w(ancestor_of_w_vector.begin(), vm),
                        vertex_state(vertex_state_vector.begin(), vm),
                        origin(origin_vector.begin(), vm),
                        pred(pred_vector.begin(), vm),
                        bridge(bridge_vector.begin(), vm),
                        ds_parent_map(ds_parent_vector.begin(), vm),
                        ds_rank_map(ds_rank_vector.begin(), vm),

                        ds(ds_rank_map, ds_parent_map),
                        
                        max_level(max_level)
                {
                        vertex_iterator_t vi, vi_end;
                        for(boost::tie(vi,vi_end) = vertices(g); vi != vi_end; ++vi)
                                mate[*vi] = get(arg_mate, *vi);
                }

                bool new_edge (NodeID u, NodeID v) {
                        edge_descriptor_t current_edge = add_edge(u, v, g).first;

                        vertex_descriptor_t vd_u = source(current_edge, g);
                        vertex_descriptor_t vd_v = target(current_edge, g);

                        augment_matching(vd_u);
                        augment_matching(vd_v);

                        return true;
                }

                bool augment_matching(vertex_descriptor_t start) {
                        if (mate[start] != graph_traits<Graph>::null_vertex()) {
                                return false;
                        }
                        //As an optimization, some of these values can be saved from one
                        //iteration to the next instead of being re-initialized each
                        //iteration, allowing for "lazy blossom expansion." This is not
                        //currently implemented.

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

                        vertex_iterator_t vi, vi_end;
                        for(boost::tie(vi,vi_end) = vertices(g); vi != vi_end; ++vi) {
                                vertex_descriptor_t u = *vi;

                                origin[u] = u;
                                pred[u] = u;
                                ancestor_of_v[u] = 0;
                                ancestor_of_w[u] = 0;
                                ds.make_set(u);

                                if (mate[u] == graph_traits<Graph>::null_vertex()) {
                                        vertex_state[u] = graph::detail::V_EVEN;
                                        if (u == start) {
                                                out_edge_iterator_t ei, ei_end;
                                                for(boost::tie(ei,ei_end) = out_edges(u,g); ei != ei_end; ++ei) {
                                                        if (target(*ei,g) != u) {
                                                                even_edges.push_back( *ei );
                                                                count_0++;
                                                        }
                                                }
                                        }
                                } else {
                                        vertex_state[u] = graph::detail::V_UNREACHED;
                                }
                        }

                        //end initializations

                        vertex_descriptor_t v,w,w_free_ancestor,v_free_ancestor;
                        w_free_ancestor = graph_traits<Graph>::null_vertex();
                        v_free_ancestor = graph_traits<Graph>::null_vertex(); 
                        bool found_alternating_path = false;

                        while(!even_edges.empty() && !found_alternating_path) {
                                // since we push even edges onto the back of the list as
                                // they're discovered, taking them off the back will search
                                // for augmenting paths depth-first.
                                edge_descriptor_t current_edge = even_edges.back();
                                even_edges.pop_back();

                                v = source(current_edge,g);
                                w = target(current_edge,g);

                                vertex_descriptor_t v_prime = origin[ds.find_set(v)];
                                vertex_descriptor_t w_prime = origin[ds.find_set(w)];

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
                                put(pm, *vi, vertex_state[origin[ds.find_set(*vi)]]);
                }



                e_size_t matching_size;

        private:

                vertex_descriptor_t parent(vertex_descriptor_t x)
                {
                        if (vertex_state[x] == graph::detail::V_EVEN
                                        && mate[x] != graph_traits<Graph>::null_vertex())
                                return mate[x];
                        else if (vertex_state[x] == graph::detail::V_ODD)
                                return origin[ds.find_set(pred[x])];
                        else
                                return x;
                }




                void link_and_set_bridges(vertex_descriptor_t x,
                                          vertex_descriptor_t stop_vertex,
                                          vertex_pair_t the_bridge)
                {
                        for(vertex_descriptor_t v = x; v != stop_vertex; v = parent(v))
                        {
                                ds.union_set(v, stop_vertex);
                                origin[ds.find_set(stop_vertex)] = stop_vertex;

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
                VertexIndexMap vm;
                v_size_t n_vertices;
                
                e_size_t max_level;

                //storage for the property maps below
                std::vector<vertex_descriptor_t> mate_vector;
                std::vector<e_size_t> ancestor_of_v_vector;
                std::vector<e_size_t> ancestor_of_w_vector;
                std::vector<int> vertex_state_vector;
                std::vector<vertex_descriptor_t> origin_vector;
                std::vector<vertex_descriptor_t> pred_vector;
                std::vector<vertex_pair_t> bridge_vector;
                std::vector<vertex_descriptor_t> ds_parent_vector;
                std::vector<v_size_t> ds_rank_vector;

                //iterator property maps
                vertex_to_vertex_map_t mate;
                vertex_to_esize_map_t ancestor_of_v;
                vertex_to_esize_map_t ancestor_of_w;
                vertex_to_int_map_t vertex_state;
                vertex_to_vertex_map_t origin;
                vertex_to_vertex_map_t pred;
                vertex_to_vertex_pair_map_t bridge;
                vertex_to_vertex_map_t ds_parent_map;
                vertex_to_vsize_map_t ds_rank_map;

                vertex_list_t aug_path;
                edge_list_t even_edges;
                disjoint_sets< vertex_to_vsize_map_t, vertex_to_vertex_map_t > ds;
        };

        template <typename Graph, typename MateMap, typename RawMateMap, typename VertexIndexMap>
        dyn_blossom<Graph, RawMateMap, VertexIndexMap> create (unsigned long n) {
                Graph arg_g(n);
                MateMap arg_m(n);
                
                vertex_iterator_t vi, vi_end;
                for(boost::tie(vi,vi_end) = vertices(arg_g); vi != vi_end; ++vi)
                        arg_m[*vi] = graph_traits<Graph>::null_vertex();
                        //~ std::cout << get(&arg_m[0], *vi) << std::endl;
                
                return dyn_blossom<Graph, RawMateMap, VertexIndexMap>(arg_g, &arg_m[0], get(vertex_index, arg_g));
        }
}

#endif // DYN_BLOSSOM_H
