#include <sys/stat.h>
#include <iostream>

#include "extern/boost_blossom/blossom_static.h"
#include "blossom_dyn_matching.h"
#include "blossom_dyn_matching_naive.h"
#include "static_blossom.h"
#include "mv_algorithm.h"
#include "io/graph_io.h"
#include "rw_dyn_matching.h"
#include "baswanaguptasen_dyn_matching.h"
#include "neimansolomon_dyn_matching.h"
#include "naive_dyn_matching.h"
#include "parse_parameters.h"
#include "definitions.h"
#include "match_config.h"
#include "sanity.h"
#include "dynamic_algorithm_meta.h"

int main (int argn, char ** argv) {
        MatchConfig match_config;
        std::string graph_filename;

        int ret_code = parse_parameters(argn, argv, 
                        match_config, 
                        graph_filename); 

        if(ret_code) {
                return 0;
        }

        // initialize edge sequence
        timer t;
        t._restart();
        graph_io gio;
        std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;
        int n = gio.read_sequence(graph_filename, edge_sequence);

        // initialize seed
        srand(match_config.seed);
        random_functions::setSeed(match_config.seed);
        dyn_graph_access * G = new dyn_graph_access(n);
        unsigned long matching_size = 0;
        NodeID maxnumnodes = 0;
        for (size_t i = 0; i < edge_sequence.size(); ++i) { 
                std::pair<NodeID, NodeID> & edge = edge_sequence[i].second;

                maxnumnodes = std::max(edge.first, maxnumnodes);
                maxnumnodes = std::max(edge.second, maxnumnodes);
                if( edge.first == edge.second ) continue;
                if (edge_sequence.at(i).first) {
                        if(!G->isEdge(edge.first, edge.second)) {
                                G->new_edge(edge.first, edge.second);
                                G->new_edge(edge.second, edge.first);
                        }
                } else {
                        if( G->isEdge(edge.first, edge.second) ) {
                                G->remove_edge(edge.first, edge.second);
                                G->remove_edge(edge.second, edge.first);
                        }
                }
        } 
        delete G;
        std::cout <<  (maxnumnodes+1)  << std::endl;

        return 0;
}

