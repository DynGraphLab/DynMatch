#include <sys/stat.h>

#include "executer.h"

int berror_counter = 1;

void berror ()  {
#ifdef MDEBUG
        std::cerr << "#######################" << std::endl
                  << "#          " << berror_counter++ << "          #" << std::endl
                  << "#######################" << std::endl;
#endif
}


int main (int argc, char ** argv) {
        berror();
        ex_config conf;
        try {
                // get command line arguments and save them to ex_config struct
                int stop = 0;

                try {
                        berror();
                        stop = get_arguments(argc, argv, conf);
                        berror();
                } catch (std::string e) {
                        std::cout << e << std::endl;
                        return 1;
                }

                if (stop > 0) {
                        return 0;
                }

                berror();

                // initialize output
                std::streambuf * buf;
                buf = std::cout.rdbuf();

                std::ostream output(buf);

                std::ofstream matching_file(conf.mfile.c_str());

                // initialize edge sequence
                std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;
                size_t n = read_sequence(conf.file, edge_sequence);

                // check that edge sequence is not empty.
                if (edge_sequence.size() == 0) {
                        throw std::string("ERROR: read empty edge sequence from file " + conf.file);
                }

                // initialize list of algorithms
                std::vector<ALGORITHM> algorithms = conf.algorithms;
                std::vector<double>    eps        = conf.eps;
                std::vector<double>    multiple_rws = conf.multiple_rws;

                std::vector<bool> measurements    = conf.measurements;

                ASSERT_TRUE(algorithms.size() == eps.size());

                // initialize seed
                srand(conf.seed);
                random_functions::setSeed(conf.seed);

                // calculate size (lines) of the resulting data to reserve space.
                size_t result_size;
                if (conf.all_at_once) {
                        result_size = 1;
                } else if (edge_sequence.size() % conf.at_once == 0) {
                        result_size = edge_sequence.size()/conf.at_once;
                } else {
                        result_size = edge_sequence.size()/conf.at_once + 1;
                }


                // intialize result vectors

                // combined_data is 3D: algorithms X evaluated sequence steps X measurements
                // algorithms is determined by algorithms.size(),
                // the evaluated sequence steps corresponds to result_size,
                // following data is measured: insertions, deletions, cardinality of G, cardinality of M
                size_t combs = 4;
                std::vector<std::vector<std::vector<int> > > combined_data(algorithms.size(), std::vector<std::vector<int> >(result_size, std::vector<int>(combs, 0)));

                // combined_data_d is 2D: algorithms X evaluated sequence steps
                // the dimensions are the same as for combined_data,
                // measured data is: average cardinality of M, elapsed time, total elapsed time including dyn_graph_access updates
                size_t combs_d = 3;
                std::vector<std::vector<std::vector<double> > > combined_data_d(algorithms.size(), std::vector<std::vector<double> >(result_size, std::vector<double>(combs_d, 0)));

                // matchings is 3D vector, which contains at the position (i, j, k) the kth edge of the jth sequence step of the ith algorithm:
                //
                // matchings = [
                //                 [                           // algorithm
                //                     [(u,v), (w,x), ...],    // matching of sequence step which edges as entries
                //                     [(a,b), (...), ...],
                //                     ...
                //                 ],
                //                 [                           // next algorithm
                //                     [...],                  // again matching for every sequence step.
                //                     ...
                //                 ],
                //                 ...
                //             ]
                //
                // matchings is therefore of dimension: algorithms X evaluated sequence steps X matching size (X 2, since the elements then again are pairs)
                std::vector<std::vector<std::vector<std::pair<NodeID, NodeID> > > > matchings(algorithms.size(), std::vector<std::vector<std::pair<NodeID, NodeID> > >(result_size));

                // previous matchings holds the matchings of the previous run, in order to compare results of different runs
                // for deterministic algorithms (neiman-solomon) there should be no changes in the results (similarity = 1),
                // for randomized algorithms it is highly unlikely that the results do not differ (similarity < 1).
                std::vector<std::vector<std::vector<std::pair<NodeID, NodeID> > > > previous_matchings(0);

                std::vector<std::vector<double> > cross_run_similarities(algorithms.size(), std::vector<double>(result_size, 0));
                std::vector<std::vector<int> > cross_run_sim_counters(algorithms.size(), std::vector<int>(result_size, 0));

                std::vector<bool> nodes(n, false);
                std::vector<int> node_degrees(n, 0);

                berror();

                //================// START CALCULATIONS //================//
                
                timer t;
                t._restart();
                
                try {
                        for (int k = 0; k < conf.multi_run; ++k) { // run the whole thing several times
                                for (size_t l = 0; l < algorithms.size(); ++l) { // run different algorithms
                                        dyn_graph_access * G = create_graph(n);
                                        dyn_matching * algorithm = init_algorithm(algorithms.at(l), G, eps.at(l), multiple_rws.at(l));

                                        std::vector<std::pair<NodeID, NodeID> > matching;
                                        
                                        // initalize timer and counters
                                        double time_elapsed = 0;
                                        double total_time_elapsed = 0;
                                        int insertions = 0;
                                        int deletions = 0;
                                        int j = 0; // counter for entries in data vectors. from range 0 to result_size

                                        node_degrees.resize(0);
                                        node_degrees.resize(n, 0);
                                        
                                        NodeID free_nodes_last_round = 0;
                                        
                                        double start = t._elapsed();
                                        
                                        for (size_t i = 0; i < edge_sequence.size(); ++i) { // iterate through sequence
                                                try {
                                                        std::pair<NodeID, NodeID> edge = edge_sequence.at(i).second;

                                                        double tmp;
                                                        bool success;

                                                        // for every NodeID save if it exists or not.
                                                        nodes.at(edge.first) = true;
                                                        nodes.at(edge.second) = true;

                                                        // determine whether to do an insertion or a deletion
                                                        if (edge_sequence.at(i).first) {
                                                                success = algorithm->new_edge(edge.first, edge.second);

                                                                if (success) {
                                                                        node_degrees.at(edge.first) = node_degrees.at(edge.first) + 1;
                                                                        node_degrees.at(edge.second) = node_degrees.at(edge.second) + 1;
                                                                }

                                                                insertions++;
                                                        } else {
                                                                success = algorithm->remove_edge(edge.first, edge.second);

                                                                if (success) {
                                                                        if (node_degrees.at(edge.first) > 0) {
                                                                                node_degrees.at(edge.first) = node_degrees.at(edge.first) - 1;
                                                                        } else {
                                                                                node_degrees.at(edge.first) = 0;
                                                                        }

                                                                        if (node_degrees.at(edge.second) > 0) {
                                                                                node_degrees.at(edge.second) = node_degrees.at(edge.second) - 1;
                                                                        } else {
                                                                                node_degrees.at(edge.second) = 0;
                                                                        }
                                                                }

                                                                deletions++;
                                                        }
                                                        
                                                        time_elapsed += tmp;
                                                } catch (std::bad_alloc& e) {
                                                        throw std::string("bad_alloc caught when processing step ") + std::to_string(i) + std::string(" with algorithm ") + ALGORITHM_NAMES.at(l) ;
                                                }

                                                // every at_once steps we collect data
                                                if (
                                                        (!conf.all_at_once && ((i+1) % conf.at_once == 0 || i == edge_sequence.size() - 1 )) ||
                                                        (conf.all_at_once && i == edge_sequence.size() -1)
                                                ) {
                                                        try {
                                                                
                                                                if (algorithms.at(l) == ALGORITHM::mcm) {
                                                                        timer t2;
                                                                        t2._restart();
                                                                        algorithm->retry();
                                                                        time_elapsed += t2._elapsed();
                                                                }
                                                                
                                                                total_time_elapsed = total_time_elapsed + t._elapsed() - start;
                                                                
                                                                size_t size_M = 0;
                                                                
                                                                if (conf.eco) {
                                                                        size_M = algorithm->getMSize();
                                                                } else {
                                                                        auto m = algorithm->getM();
                                                                        
                                                                        for (int i  = 0; i < m.size(); ++i) {
                                                                                if (m[i] != NOMATE)
                                                                                matching.push_back({i, m[i]});
                                                                        }
                                                                        // validate the matching to exclude errors in the algorithm
                                                                        quality_metrics::matching_validation(matching);
                                                                        size_M = matching.size();
                                                                        
                                                                        if (conf.print_matchings || conf.measurements.at(MEASUREMENTS::cs)) {
                                                                                matchings.at(l).at(j) = matching;
                                                                        }
                                                                }


                                                                // set algorithm counters to next entry
                                                                algorithm->counters_next();

                                                                // save data in arrays, l = algorithm, j = entry
                                                                combined_data.at(l).at(j).at(0) = get_cumulated_degree(*G, matching);
                                                                combined_data.at(l).at(j).at(1) = count_nodes(node_degrees, 1); // count number of nodes with degree > 1

                                                                // size gets divided by two since M holds every edge twice as (u,v) and (v,u)
                                                                combined_data.at(l).at(j).at(2) = algorithm->getG().number_of_edges()/2;
                                                                combined_data.at(l).at(j).at(3) = size_M/2;
                                                                combined_data_d.at(l).at(j).at(0) = combined_data_d.at(l).at(j).at(0) + size_M/(2.0 * conf.multi_run);
                                                                combined_data_d.at(l).at(j).at(1) = combined_data_d.at(l).at(j).at(1) + time_elapsed/conf.multi_run;
                                                                
                                                                combined_data_d.at(l).at(j).at(2) = combined_data_d.at(l).at(j).at(2) + total_time_elapsed/conf.multi_run;

                                                                // iterator for data vector gets incremented only when data is written to the data vector
                                                                // j = 0:1:result_size; using octave syntax
                                                                j++;

                                                                // reset counters
                                                                total_time_elapsed = 0;
                                                                time_elapsed = 0;
                                                                insertions = 0;
                                                                deletions = 0;
                                                                
                                                        } catch (std::bad_alloc &e) {
                                                                throw std::string("bad_alloc caught when processing results after step ") + std::to_string(i) + std::string(" with algorithm ") + ALGORITHM_NAMES.at(l);
                                                        }
                                                }
                                        }
                                        
                                        delete algorithm;
                                } // going to next algorithm


                                // compute matching similarity between different runs.
                                if (previous_matchings.size() > 0 && conf.multi_run > 1 && conf.measurements.at(MEASUREMENTS::cs)) {
                                        ASSERT_TRUE(matchings.size() == previous_matchings.size()); // both vectors should contain algorithms.size() subvectors
                                        ASSERT_TRUE(matchings.size() == algorithms.size());

                                        std::cerr << "performing cross run similarity computations ... " << std::endl;

                                        for (size_t i = 0; i < matchings.size(); ++i) { // iterate through the algorithms
                                                ASSERT_TRUE(matchings.at(i).size() == previous_matchings.at(i).size()); // the subvectors contain the matchings for
                                                ASSERT_TRUE(matchings.at(i).size() == result_size);                     // the corresponding ith sequence step. since
                                                // every run processes the same sequence, the
                                                // size of those subvectors should also match.
                                                for (size_t j = 0; j < matchings.at(i).size(); ++j) {
                                                        if (matchings.at(i).at(j).size() != 0 || previous_matchings.at(i).at(j).size() != 0) {
                                                                int union_size = 1;
                                                                int intersect_size = quality_metrics::edgeset_intersect(matchings.at(i).at(j), previous_matchings.at(i).at(j), union_size).size();

                                                                // same as with cross algorithm similarity, for union size = 0, similarity is 0
                                                                cross_run_similarities.at(i).at(j) = cross_run_similarities.at(i).at(j) + (union_size ? ((intersect_size * 1.0) / union_size) : 0);
                                                                cross_run_sim_counters.at(i).at(j)++;
                                                        }
                                                }
                                        }

                                        previous_matchings = matchings;
                                        matchings.resize(algorithms.size());
                                }
                        } // end of multiple runs with the same algorithm

                } catch (std::string &e) {
                        throw std::string ("caught std::string between line 125 and 281:\n") + e;
                } catch (std::bad_alloc &e) {
                        throw std::string ("caught std::bad_alloc between line 125 and 281.");
                }
                
                //================// PROCESS INFORMATION AND PRINT TO FILE //================//

                berror();

                size_t node_count = count_nodes(nodes);

                if (!conf.no_meta) {
                        output << "# nodes: " << std::endl
                               << "# " << node_count << std::endl
                               << "# algorithms: " << std::endl
                               << "# ";

                        for (size_t i = 0; i < algorithms.size(); ++i) {
                                output << ALGORITHM_NAMES.at(algorithms.at(i)) << " ";
                        }

                        output << std::endl
                               << "# eps: " << std::endl
                               << "# ";

                        for (size_t i = 0; i < algorithms.size(); ++i) {
                                output << eps.at(i) << " ";
                        }

                        output << std::endl
                               << "# rws: " << std::endl
                               << "# ";

                        for (size_t i = 0; i < algorithms.size(); ++i) {
                                output << multiple_rws.at(i) << " ";
                        }

                        output << std::endl
                               << "# seed: " << std::endl
                               << "# " << conf.seed;

                        output << std::endl
                               << "# repetitions: " << std::endl
                               << "# " << conf.multi_run;

                        output << std::endl
                               << "# output format: " << std::endl
                               << "# ";

                        for (size_t i = 0; i < measurements.size(); ++i) {
                                if (measurements.at(i)) {
                                        output << MEASUREMENTS_NAME.at(i) << " ";
                                }
                        }
                }

                output << std::endl;

                ASSERT_TRUE(combined_data.size() == combined_data_d.size());

                // write data to file
                for (size_t i = 0; i < result_size; ++i) { // iterate through sequence steps
                        for (size_t j = 0; j < algorithms.size(); ++j) { // iterate through different runs with different algorithms
                                for (size_t k = 0; k < measurements.size(); ++k) {
                                        if (measurements.at(k)) {
                                                if (k < combs) {
                                                        output << combined_data.at(j).at(i).at(k) << " ";
                                                } else if (k < combs + combs_d) {
                                                        output << combined_data_d.at(j).at(i).at(k % combs) << " ";
                                                } else if (k == combs + combs_d) {
                                                        // check that we don't divdide by zero
                                                        output << (cross_run_sim_counters.at(j).at(i) ? cross_run_similarities.at(j).at(i) / cross_run_sim_counters.at(j).at(i) : 0) << " ";
                                                }
                                        }
                                }

                                if (conf.print_matchings) {
                                        print_matching(matching_file, matchings.at(j).at(i));
                                }
                        }

                        output << std::endl;
                        matching_file << std::endl;
                }

                if (conf.print_matchings) {
                        matching_file.close();
                }
        } catch (std::string e) {
                std::cerr << "FAILED to finish matching computations when processing: " << conf.file << std::endl;
                std::cerr << e << std::endl;
        }

        return 0;
}

