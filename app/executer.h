#ifndef EXECUTER_H
#define EXECUTER_H

#include "dynamic_matching.h"

#include "misc.h"
#include "glossary.h"

//#include "../GPAMatching/app/gpa_match.h"

/*
#include "config.h"
#include "data_structure/graph_access.h"
#include "io/graph_io.h"
#include "matching/gpa_matching.h"
#include "tools/macros_assertions.h"
#include "tools/random_functions.h"
#include "../GPAMatching/lib/tools/random_functions.h"
*/
#include <argtable2.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <climits>
#include <algorithm>

// list of available algorithms and respective names. TODO: outsource maybe?
enum ALGORITHM {
        bgs, 
        gpa, 
        naive, 
        ns, 
        rw, 
        mrw, 
        sq_m_rw, 
        lds, 
        en, 
        ei_naive, 
        mcm, 
        blossomempty, 
        blossomgreedy, 
        blossomextragreedy, 
        parallel_rw
};

extern std::vector<std::string> ALGORITHM_NAMES;
//extern std::vector<std::string> ALGORITHM_ABBREVS;

enum MEASUREMENTS {cd, av, G, M, aM, ut, tt, cs};
extern std::vector<std::string> MEASUREMENTS_NAME;

// config struct which holds different config variables
struct ex_config {
        std::string file;
        long seed; // gpa seed
        size_t at_once;
        bool all_at_once;
        int multi_run;
        int threads;
        bool no_meta;
        bool print_matchings;
        bool eco;
        bool do_mv;
        bool do_blossom;
        std::string mfile;

        std::vector<ALGORITHM> algorithms;
        std::vector<double> eps;
        std::vector<double> multiple_rws;
        std::vector<long> low_degree;
        std::vector<bool> measurements;

        ex_config () {
                seed              = 0;
                at_once           = 1;
                all_at_once       = true;
                multi_run         = 1;
                no_meta           = false;
                threads           = 1;
                print_matchings   = false;
                eco               = false;
                std::string mfile = "matchings";
                do_mv             = false;
                do_blossom        = false;

                algorithms        = std::vector<ALGORITHM>();
                eps               = std::vector<double>();
                multiple_rws      = std::vector<double>();
                low_degree        = std::vector<long>();
                measurements      = std::vector<bool>({0, 0, 0, 0, 1, 0, 0, 0});
        }
};

// convert matching from std::vector<NodeID> to std::vector<std::pair<NodeID, NodeID> >
std::vector<std::pair<NodeID, NodeID> > matching_to_pairvec (const Matching & matching);

// print matching
void print_matching (std::ostream& o, const std::vector<std::pair<NodeID, NodeID> >& matching);

// get command line arguments
void check_option_arg (std::string option, int i, int argc, char** argv);
int get_arguments(int argc, char ** argv, ex_config& conf, bool get_algorithm = true);
std::pair<std::vector<ALGORITHM>,std::pair<std::vector<double>, std::pair<std::vector<double>, std::vector<long> > > > parse_algorithms(std::string algorithms, int& threads);
std::vector<bool> parse_output(std::string format);

// read sequence in own format, obtained from using the sequencer program
size_t read_sequence (std::string file, std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& edge_sequence);

// create output filename
std::string get_output_filename (ex_config conf);

// helper function to easily create graph with n vertices
dyn_graph_access* create_graph (size_t n);

// creates new instance of the passed algorithm.
dyn_matching * init_algorithm (ALGORITHM algorithm, dyn_graph_access * G, double eps, double multiple_rws, long low_degree = -1, int threads = 1);

// auxiliary functions
EdgeID get_cumulated_degree(dyn_graph_access& G, std::vector<std::pair<NodeID, NodeID> > edgeset);
NodeID count_nodes(std::vector<bool>& nodes);
NodeID count_nodes(std::vector<int>& nodes, int min_degree = 0);

#endif // EXECUTER_H
