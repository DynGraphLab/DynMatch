#include <sys/stat.h>
#include <iostream>

#include "blossom_static.h"
#include "blossom_dyn_matching.h"
#include "my_blossom_dyn_matching.h"
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
        graph_io gio;
        std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;
        int n = gio.read_sequence(graph_filename, edge_sequence);

        // initialize seed
        srand(match_config.seed);
        random_functions::setSeed(match_config.seed);

        dyn_graph_access * G = new dyn_graph_access(n);
        dyn_matching * algorithm = NULL;
        switch( match_config.algorithm ) {
                case RANDOM_WALK:
                        algorithm = new rw_dyn_matching(G, match_config);
                        break;
                case BASWANA_GUPTA_SENG:
                        algorithm = new baswanaguptasen_dyn_matching(G, match_config);
                        break;
                case NEIMAN_SOLOMON:
                        algorithm = new neimansolomon_dyn_matching(G, match_config);
                        break;
                case NAIVE: 
                        algorithm = new naive_dyn_matching(G, match_config);
                        break;
                case MV: 
                        algorithm = new mv_algorithm(G, match_config);
                        break;
                case MYBLOSSOMDYNMATCHING: {
                        for (size_t i = 0; i < edge_sequence.size(); ++i) { 
                                std::pair<NodeID, NodeID> & edge = edge_sequence.at(i).second;
                                G->new_edge(edge.first, edge.second);
                                G->new_edge(edge.second, edge.first);
                        } 

                        dyn_matching* myalgorithm = new my_blossom_dyn_matching(G, match_config);
                        delete myalgorithm;}
                        break;
                case DYNBLOSSOM:
                        algorithm = new blossom_dyn_matching(G, match_config);
                case BLOSSOM: {
                        unsigned long matching_size = 0;
                        switch( match_config.blossom_init ) {
                                case BLOSSOMEMPTY:
                                        blossom_matchempty(edge_sequence, matching_size);
                                        break;
                                case BLOSSOMGREEDY:
                                        blossom_matchgreedy(edge_sequence, matching_size);
                                        break;
                                case BLOSSOMEXTRAGREEDY:
                                        blossom_matchextragreedy(edge_sequence, matching_size);
                                        break;
                        }}

                        break;

        }

        if(algorithm != NULL) 
                run_dynamic_algorithm(G, edge_sequence, algorithm, match_config);

        delete G;
        delete algorithm;

        return 0;
}

