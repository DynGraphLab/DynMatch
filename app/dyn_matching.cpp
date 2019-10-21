#include <sys/stat.h>

#include "executer.h"
#include "blossom.h"
#include "dyn_blossom.h"
#include "mv.h"

int main (int argc, char ** argv) {
        ex_config conf;
        // get command line arguments and save them to ex_config struct
        int stop = 0;

        try {
                stop = get_arguments(argc, argv, conf);
        } catch (std::string e) {
                std::cout << e << std::endl;
                return 1;
        }

        if (stop > 0) {
                return 0;
        }

        // initialize edge sequence
        std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;
        size_t n = read_sequence(conf.file, edge_sequence);

        // check that edge sequence is not empty.
        if (edge_sequence.size() == 0) {
                throw std::string("ERROR: read empty edge sequence from file " + conf.file);
        }

        // initialize list of algorithms
        std::vector<ALGORITHM> algorithms   = conf.algorithms;
        std::vector<double>    eps          = conf.eps;
        std::vector<double>    multiple_rws = conf.multiple_rws;
        std::vector<long>      low_degree   = conf.low_degree;
        std::vector<bool>      measurements = conf.measurements;
        
        int threads = conf.threads;

        // initialize seed
        srand(conf.seed);
        random_functions::setSeed(conf.seed);
        
        size_t get_data = conf.at_once;

        //================// START CALCULATIONS //================//
        dyn_graph_access * G     = create_graph(n);

        timer t; 
        t._restart(); 
        dyn_matching * algorithm = init_algorithm(algorithms.at(0), G, eps.at(0), multiple_rws.at(0), low_degree.at(0), threads);

        // initalize timer and counters
        double time_elapsed = 0;
        int insertions      = 0;
        int deletions       = 0;
        
        NodeID matching_size;

        bool do_mv = conf.do_mv;
        bool do_blossom = conf.do_blossom;
        
        // iterate through sequence, compute matching dynamically
        if (algorithms.at(0) == ALGORITHM::blossomempty) {
                blossom_matchempty(edge_sequence, matching_size, time_elapsed, insertions, deletions);
                delete G;
        } else if (algorithms.at(0) == ALGORITHM::blossomgreedy) {
                blossom_matchgreedy(edge_sequence, matching_size, time_elapsed, insertions, deletions);
                delete G;
        } else if (algorithms.at(0) == ALGORITHM::blossomextragreedy) {
                blossom_matchextragreedy(edge_sequence, matching_size, time_elapsed, insertions, deletions);
                delete G;
        } else {
                for (size_t i = 0; i < edge_sequence.size(); ++i) { 
                        std::pair<NodeID, NodeID> edge = edge_sequence.at(i).second;
                        //~ std::cout << i << " " << edge.first << " " << edge.second << std::endl;

                        // determine whether to do an insertion or a deletion
                        if (edge_sequence.at(i).first) {
                                algorithm->new_edge(edge.first, edge.second);
                                insertions++;
                        } else {
                                algorithm->remove_edge(edge.first, edge.second);
                                deletions++;
                        }
                        
                        if (!conf.all_at_once && (i+1)%get_data == 0) {
                                algorithm->counters_next();
                        }
                } 
                
                if (do_mv) {
                        //std::cout <<  algorithm->getMSize()  << " " << time_elapsed << " " << t._elapsed() << " " << insertions << " " << deletions << std::endl;
                        mv(*G, matching_size, time_elapsed, algorithm->getM());
                } else if (do_blossom) {
                        int foo;
                        //std::cout << algorithm->getMSize()  << " " << time_elapsed << " " << t._elapsed() << " " << insertions << " " << deletions << std::endl;
                        blossom_untouched(edge_sequence, matching_size, time_elapsed, foo, foo, algorithm->getM());
                } else {
                        matching_size = algorithm->getMSize();
                }
        }
        
#ifdef DM_COUNTERS
                std::ofstream counters_file("counters");
                counters::print(counters_file);
                counters_file.close();
#endif

        std::cout << matching_size << " " << time_elapsed << " " << t._elapsed() << " " << insertions << " " << deletions << std::endl;
        std::cout <<  "checking matching"  << std::endl;

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
                        return EXIT_FAILURE;
                }
                if( matching_partner > n ) continue; 
                if( is_matched[n] || is_matched[matching_partner] ) {
                        std::cout <<  "not a matching, vertex already matched"  << std::endl;
                        return EXIT_FAILURE;
                }
                is_matched[n] = true;
                is_matched[matching_partner] = true;

        } endfor
        delete algorithm;

        return 0;
}

