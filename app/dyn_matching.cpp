#include <sys/stat.h>

#include "blossom.h"
#include "dyn_blossom.h"
#include "mv.h"
#include "parse_parameters.h"
#include "definitions.h"
#include "match_config.h"

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

        //================// START CALCULATIONS //================//
        dyn_graph_access * G = new dyn_graph_access(n);

        timer t; 
        t._restart(); 
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
                        break;
                case BLOSSOM:
                        break;
        
        }

        // initalize timer and counters
        double time_elapsed = 0;
        NodeID matching_size = 0;

        // iterate through sequence, compute matching dynamically
        //if (algorithms.at(0) == ALGORITHM::blossomempty) {
                //blossom_matchempty(edge_sequence, matching_size, time_elapsed, insertions, deletions);
        //} else if (algorithms.at(0) == ALGORITHM::blossomgreedy) {
                //blossom_matchgreedy(edge_sequence, matching_size, time_elapsed, insertions, deletions);
        //} else if (algorithms.at(0) == ALGORITHM::blossomextragreedy) {
                //blossom_matchextragreedy(edge_sequence, matching_size, time_elapsed, insertions, deletions);
        //} else {
                for (size_t i = 0; i < edge_sequence.size(); ++i) { 
                        std::pair<NodeID, NodeID> & edge = edge_sequence.at(i).second;

                        if (edge_sequence.at(i).first) {
                                algorithm->new_edge(edge.first, edge.second);
                        } else {
                                algorithm->remove_edge(edge.first, edge.second);
                        }
                } 
                
                //if (match_config.post_mv) {
                        //mv(*G, matching_size, time_elapsed, algorithm->getM());
                //} else if (match_config.post_blossom) {
                        //int foo;
                        //blossom_untouched(edge_sequence, matching_size, time_elapsed, foo, foo, algorithm->getM());
                //} else {
                        matching_size = algorithm->getMSize();
                //}
        //}

        std::cout << matching_size << " " << time_elapsed << " " << t._elapsed() << std::endl;
        std::cout << "************ checking matching ****************"  << std::endl;

        // check if matching is really a matching:
        std::vector< NodeID > matchingp = algorithm->getM();
        std::vector< bool > is_matched(n, false);
        forall_nodes_d((*G), n) {
                NodeID matching_partner = matchingp[n];
                if( matching_partner == NOMATE ) {
                        continue;
                }

                if( matchingp[matching_partner] != n) {
                        std::cout <<  "not a matching " <<  n <<  " " << matching_partner << " " << matchingp[matching_partner]  << std::endl;
                }
                if( matching_partner > n ) continue; 
                if( is_matched[n] || is_matched[matching_partner] ) {
                        std::cout <<  "not a matching, vertex already matched"  << std::endl;
                }
                is_matched[n] = true;
                is_matched[matching_partner] = true;

        } endfor
        delete G;
        delete algorithm;

        return 0;
}

