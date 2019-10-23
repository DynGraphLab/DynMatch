#include <sys/stat.h>

#include "blossom.h"
#include "dyn_blossom.h"
#include "mv.h"
#include "mv_algorithm.h"
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
                        break;
                case NEIMAN_SOLOMON:
                        break;
                case MV: 
                        algorithm = new mv_algorithm(G, match_config);
                        break;
                case BLOSSOM:
                         break;

        }

        // initalize timer and counters

        // iterate through sequence, compute matching dynamically
        //if (algorithms.at(0) == ALGORITHM::blossomempty) {
        //blossom_matchempty(edge_sequence, matching_size, time_elapsed, insertions, deletions);
        //} else if (algorithms.at(0) == ALGORITHM::blossomgreedy) {
        //blossom_matchgreedy(edge_sequence, matching_size, time_elapsed, insertions, deletions);
        //} else if (algorithms.at(0) == ALGORITHM::blossomextragreedy) {
        //blossom_matchextragreedy(edge_sequence, matching_size, time_elapsed, insertions, deletions);
        //} else {

        run_dynamic_algorithm(G, edge_sequence, algorithm, match_config);

        delete G;
        delete algorithm;

        return 0;
}

