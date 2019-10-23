#include <iostream>
#include <stdlib.h>
#include <fstream>

#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/max_cardinality_matching.hpp>

#include "tools/timer.h"
#include "definitions.h"

//const unsigned long NOMATE = std::numeric_limits<unsigned long>::max();

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> mygraph;
typedef std::vector<boost::graph_traits<mygraph>::vertex_descriptor> matemap;
typedef boost::property_map<mygraph, boost::vertex_index_t>::type vmi;

typedef typename boost::graph_traits< mygraph >::vertex_iterator vertex_iterator_t;

// passing an untouched matching as InitialMatchingFinder outsources
// responsibility to initialize the MateMap
template <typename Graph, typename MateMap>
struct untouched_matching { 
        //~ typedef typename graph_traits< Graph >::vertex_iterator vertex_iterator_t;

        static void find_matching(const Graph& g, MateMap mate) {
                //~ vertex_iterator_t vi, vi_end;
                //~ for(boost::tie(vi,vi_end) = vertices(g); vi != vi_end; ++vi)
                        //~ put(mate, *vi, graph_traits<Graph>::null_vertex());
        }
};

void set_matching (const mygraph& g, matemap& mate, const std::vector<NodeID>& matching) {
        size_t i = 0;

        vertex_iterator_t vi, vi_end;
        for(boost::tie(vi,vi_end) = vertices(g); vi != vi_end; ++vi) {
                if (matching[i] != NOMATE) {
                        //~ graph_traits< mygraph >::vertex_descriptor u;
                        auto u = boost::vertex (matching[i], g);
                        put(&mate[0], *vi, u);
                } else {
                        put(&mate[0], *vi, boost::graph_traits< mygraph >::null_vertex());
                }
                ++i;
            }
}

void init (const std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& sequence, mygraph& g, matemap& m, int& inserted, int& removed) {
        inserted = 0;
        removed  = 0;
        
        int mode;
        long u;
        long v;
        
        for (auto e : sequence) {
                mode = e.first;
                u    = e.second.first;
                v    = e.second.second;
                
                if (mode == 1) {
                        boost::add_edge(u, v, g);
                        inserted++;
                
                } else if (mode == 0) {
                        boost::remove_edge(u, v, g);
                        removed++;
                        
                }
        }
        
        vertex_iterator_t vi, vi_end;
        boost::tie(vi, vi_end) = boost::vertices(g);
        int n_vertices = *vi_end;
        
        m = matemap(n_vertices);
}

template< template <typename, typename> class InitialMatchingFinder, typename Graph, typename MateMap, typename VertexIndexMap >
void blossom_match (const Graph& g, MateMap m, VertexIndexMap vim, unsigned long& m_size, double& time_elapsed) {
        timer::restart();

        boost::matching< Graph, MateMap, VertexIndexMap, boost::edmonds_augmenting_path_finder, InitialMatchingFinder, boost::no_matching_verifier >(g, m, vim); 
        
        time_elapsed = timer::elapsed();
        m_size = boost::matching_size(g, m);
}

void blossom_matchempty (const std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& sequence, unsigned long& m_size, double& time_elapsed, int& inserted, int& removed) {
        mygraph g;
        matemap m;
        
        init(sequence, g, m, inserted, removed);
        blossom_match<boost::empty_matching>(g, &m[0], boost::get(boost::vertex_index, g), m_size, time_elapsed);
}

void blossom_matchgreedy (const std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& sequence, unsigned long& m_size, double& time_elapsed, int& inserted, int& removed) {
        mygraph g;
        matemap m;
        
        init(sequence, g, m, inserted, removed);
        blossom_match<boost::greedy_matching>(g, &m[0], boost::get(boost::vertex_index, g), m_size, time_elapsed);
}

void blossom_matchextragreedy (const std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& sequence, unsigned long& m_size, double& time_elapsed, int& inserted, int& removed) {
        mygraph g;
        matemap m;
        
        init(sequence, g, m, inserted, removed);
        blossom_match<boost::extra_greedy_matching>(g, &m[0], boost::get(boost::vertex_index, g), m_size, time_elapsed);
}

void blossom_untouched (const std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& sequence, unsigned long& m_size, double& time_elapsed, int& inserted, int& removed, const std::vector<NodeID>& matching) {
        mygraph g;
        matemap m;
        
        init(sequence, g, m, inserted, removed);
        set_matching(g, m, matching);
        
        blossom_match<untouched_matching>(g, &m[0], boost::get(boost::vertex_index, g), m_size, time_elapsed);
}

//~ struct dyn_blossom {
        //~ void blossom_dyn ();
        
        //~ mygraph g;
        //~ matemap m;
        
        //~ NodeID m_size;
        
        //~ dyn_blossom (NodeID n) {
                //~ for (NodeID i = 0; i < n; ++i) {
                        //~ boost::add_vertex(g);
                //~ }
                
                //~ g = mygraph(n);
                
                //~ vertex_iterator_t vi, vi_end;
                //~ boost::tie(vi, vi_end) = boost::vertices(g);
                //~ int n_vertices = *vi_end;
                
                //~ m = matemap(n_vertices);
        //~ }
        
        //~ void new_edge (NodeID source, NodeID target) {
                //~ double t;
                //~ boost::add_edge(source, target, g);
                //~ blossom_match<untouched_matching>(g, &m[0], boost::get(boost::vertex_index, g), m_size, t);
        //~ }
        
        //~ void remove_edge (NodeID source, NodeID target) {
                
        //~ }
//~ };

/*
void blossom_matchempty (const std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& sequence, unsigned long& m_size, double& time_elapsed, int& inserted, int& removed) {
        blossom_matchempty (sequence, m_size, time_elapsed, inserted, removed, {});
}

void blossom_matchgreedy (const std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& sequence, unsigned long& m_size, double& time_elapsed, int& inserted, int& removed) {
        blossom_matchgreedy (sequence, m_size, time_elapsed, inserted, removed, {});
}

void blossom_matchextragreedy (const std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& sequence, unsigned long& m_size, double& time_elapsed, int& inserted, int& removed) {
        blossom_matchextragreedy (sequence, m_size, time_elapsed, inserted, removed, {});
}
*/
