#include "executer.h"

std::vector<std::string> ALGORITHM_NAMES = {
        "bgs", 
        "gpa", 
        "naive", 
        "ns", 
        "rw", 
        "mrw", 
        "sq-m-rw", 
        "lds", 
        "en", 
        "ei-naive", 
        "mcm", 
        "blossomempty", 
        "blossomgreedy", 
        "blossomextragreedy", 
        "parallel_rw"
};

std::vector<std::string> MEASUREMENTS_NAME = {"cd", "av", "G", "M", "aM", "ut", "tt", "cs"};

std::vector<std::pair<NodeID, NodeID> > matching_to_pairvec (const Matching & matching) {
        std::vector<std::pair<NodeID, NodeID> > pairvec;

        for (size_t i = 0; i < matching.size(); ++i) {
                if (matching.at(i) != i) {
                        pairvec.push_back({i, matching.at(i)});
                }
        }

        return pairvec;
}

void print_matching (std::ostream& o, const std::vector<std::pair<NodeID, NodeID> >& matching) {
        for (size_t i = 0; i < matching.size(); ++i) {
                o << matching.at(i).first << " " << matching.at(i).second << "; ";
        }
        o << std::endl;
}

void check_option_arg (std::string option, int i, int argc, char** argv) {
        if (i+1 >= argc || argv[i+1][0] == '-') {
                throw std::string("ERROR: argument for option " + option + " missing.");
        }
}

int get_arguments(int argc, char ** argv, ex_config& conf, bool get_algorithm) {
        const char *progname = argv[0];

        // Setup argtable parameters.
        struct arg_lit  *help                                = arg_lit0("h", "help","Print help.");
        struct arg_str  *algorithms;
        
        if (get_algorithm) {
                         algorithms                          = arg_str1("a", "algorithms", "[alg1:arg1:arg2,alg2,...]", glossary::algorithms.c_str());
        } else {
                         algorithms                          = arg_str0("a", "algorithms", "[alg1:arg1:arg2,alg2,...]", glossary::algorithms.c_str());
        }
        
        struct arg_file *sequence                            = arg_file1("f", "file", "<file>", "Path to dynamic edge update sequence.");
        struct arg_str  *user_seed                           = arg_str0(NULL, "seed", "<seed>", "Seed to be used from randomized algorithms. Use 'all' to create seed automatically.");
        struct arg_str  *_step                               = arg_str0("s", "step", "<step>", "matching size and update time taken will only be gathered all <step> sequence steps. use 'all' to gather measurements only once after having processed the whole sequence.");
        struct arg_int  *_multi_run                          = arg_int0("r", "repetitions", "<repetitions>", "number of experiment repetitions.");
        struct arg_str  *_output                             = arg_str0("o", "output-format", "[cd,av,E,M,aM,ut,cs]", glossary::output_format.c_str());
        struct arg_lit  *_no_meta                    = arg_lit0(NULL, "no-meta", "Do not print meta information about experiment on standard output.");
        struct arg_lit  *_print_matchings                    = arg_lit0("p", "print-matchings", "Print matchings of last experiment repetition into file 'matchings'.");
        struct arg_str  *_print_matchings_str                = arg_str0("m", "matchings-to-file", "<filename>", "Print matchings of last experiment repetition into file <filename>.");
        struct arg_lit  *_eco                                = arg_lit0("e", "eco", "Run in economy mode. No matchings will be extra validated and only matching size will be reported. No matchings will be printed, even when using -m or -p flags.");
        struct arg_lit  *_opt                                = arg_lit0(NULL, "opt", "Run Micali-Vazirani algorithm on all free vertices in final graph to obtain optimal solution.");
        struct arg_lit  *_opt_mv                             = arg_lit0(NULL, "mv", "Run Micali-Vazirani algorithm on all free vertices in final graph to obtain optimal solution.");
        struct arg_lit  *_opt_bl                             = arg_lit0(NULL, "blossom", "Run blossom algorithm on all free vertices in final graph to obtain optimal solution.");

        struct arg_end  *end                                 = arg_end(100);

        // Define argtable.
        void* argtable[] = {
                help, algorithms, sequence, user_seed, _step, _multi_run, _output, _no_meta, _print_matchings, _print_matchings_str, _eco, _opt, _opt_mv, _opt_bl, end
        };

        // Parse arguments.
        int nerrors = arg_parse(argc, argv, argtable);

        // Catch case that help was requested.
        if (help->count > 0) {
                printf("Usage: %s", progname);
                arg_print_syntax(stdout, argtable, "\n");
                arg_print_glossary(stdout, argtable,"  %-50s %s\n");
                printf("\nThis is an experimental program for fully dynamic maximal matching.\n");
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1;
        }


        if (nerrors > 0) {
                arg_print_errors(stderr, end, progname);
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                throw std::string("Try '%s --help' for more information.\n")+ std::string(progname);
        }

        if (algorithms->count > 0) {
                auto parsed = parse_algorithms(algorithms->sval[0], conf.threads);
                conf.algorithms = parsed.first;
                conf.eps        = parsed.second.first;
                conf.multiple_rws = parsed.second.second.first;
                conf.low_degree = parsed.second.second.second;
        }

        if (sequence->count > 0) {
                conf.file = sequence->filename[0];
        }

        if (user_seed->count > 0) {
                if (std::string(user_seed->sval[0]) == std::string("auto")) {
                        srand(time(0));
                        conf.seed = rand();
                } else {
                        try {
                                conf.seed = std::stoi(user_seed->sval[0]);
                        } catch (...) {
                                throw std::string("could not parse ") + std::string(user_seed->sval[0]) + std::string(" as seed. use 'auto' or pass an integer seed");
                        }
                }
        }

        if(_step->count > 0) {
                if (std::string(_step->sval[0]) == std::string("all")) {
                        conf.at_once = 1;
                        conf.all_at_once = true;
                } else {
                        conf.all_at_once = false;
                        try {
                                conf.at_once = static_cast<unsigned int>(std::stoul(_step->sval[0]));
                        } catch (...) {
                                throw std::string("could not parse ") + std::string(_step->sval[0]) + std::string(" to integer.");
                        }
                }
        }

        if (_multi_run->count > 0) {
                conf.multi_run = _multi_run->ival[0];
        }

        if (_output->count > 0) {
                conf.measurements = parse_output(_output->sval[0]);
        }

        if (_print_matchings->count > 0) {
                conf.print_matchings = true;

                conf.mfile = std::string("matchings");
        }

        if (_no_meta->count > 0) {
                conf.no_meta = true;
        }

        if (_eco->count > 0) {
                conf.eco = true;
        }
        
        if (_opt->count > 0 || _opt_mv->count > 0) {
                conf.do_mv = true;
        }

        if (_opt_bl->count > 0) {
                conf.do_blossom = true;
        }
        
        if (_print_matchings_str->count > 0) {
                conf.print_matchings = true;

                conf.mfile = std::string(_print_matchings_str->sval[0]);
        
        }
        arg_freetable(argtable,sizeof(argtable)/sizeof(argtable[0]));
        
        return 0;
}

std::pair<std::vector<ALGORITHM>,std::pair<std::vector<double>, std::pair<std::vector<double>, std::vector<long> > > > parse_algorithms(std::string algorithms, int& threads) {
        if (*algorithms.begin() != '[') throw std::string("wrong format in ") + algorithms;
        if (*algorithms.rbegin() != ']') throw std::string("wrong format in ") + algorithms;

        algorithms = algorithms.substr(1, algorithms.size()-2);

        std::vector<std::string> foo = split(algorithms, ',');

        std::vector<ALGORITHM> algs (foo.size(), ALGORITHM::naive);
        std::vector<double>    eps (foo.size(), -1);
        std::vector<double>    multiple_rws (foo.size(), 1);
        std::vector<long>      low_degree (foo.size(), -1);

        for (size_t i = 0; i < foo.size(); ++i) {
                std::vector<std::string> bar = split(foo[i], ':');
                int index = index_of(ALGORITHM_NAMES, bar[0]);

                if (index >= 0) {
                        algs.at(i) = static_cast<ALGORITHM>(index);

                        if ((index >= ALGORITHM::rw && index <= ALGORITHM::en) || index == ALGORITHM::parallel_rw) { // && index != ALGORITHM::sq_m_rw) {
                                if (bar.size() >= 2) {
                                        try {
                                                eps.at(i) = std::stod(bar[1]);
                                        } catch (...) {
                                                throw std::string("could not parse ") + bar[1] + std::string(" to double. passed as eps parameter to ") + ALGORITHM_NAMES.at(index);
                                        }
                                }

                                if (bar.size() >= 3) {
                                        try {
                                                multiple_rws.at(i) = std::stoi(bar[2]);
                                        } catch (...) {
                                                throw std::string("could not parse ") + bar[2] + std::string(" to int. passed as multi rw parameter to ") + ALGORITHM_NAMES.at(index);
                                        }
                                }

                                if (bar.size() >= 4) {
                                        try {
                                                low_degree.at(i) = std::stol(bar[3]);
                                        } catch (...) {
                                                throw std::string("could not parse ") + bar[3] + std::string(" to int. passed as low degree parameter to ") + ALGORITHM_NAMES.at(index);
                                        }
                                }

                                if (bar.size() >= 5) {
                                        try {
                                                threads = std::stoi(bar[4]);
                                        } catch (...) {
                                                throw std::string("could not parse ") + bar[4] + std::string(" to int. passed as threads parameter to ") + ALGORITHM_NAMES.at(index);
                                        }
                                }
                        }
                        
                        if (index == ALGORITHM::mcm) {
                                if (bar.size() >= 2) {
                                        try {
                                                eps.at(i) = std::stod(bar[1]);
                                        } catch (...) {
                                                throw std::string("could not parse ") + bar[1] + std::string(" to int. passed as multi rw parameter to ") + ALGORITHM_NAMES.at(index);
                                        }
                                }
                        }

                } else {
                        throw std::string("algorithm ") + bar[0] + std::string(" unknown.");
                }
        }

        return {algs, {eps, {multiple_rws, low_degree}}};
}

//
std::vector<bool> parse_output(std::string format) {
        if (*format.begin() == '[' && *format.rbegin() == ']') {
                std::vector<bool> f = std::vector<bool>(8,0);

                std::vector<std::string> format_s = split (format.substr(1, format.size()-2), ',');

                for (auto ms : format_s) {
                        bool found = false;
                        size_t i = 0;

                        for (size_t j = 0; j < MEASUREMENTS_NAME.size(); ++j) {
                                std::string name = MEASUREMENTS_NAME.at(j);

                                if (name == ms) {
                                        i = j;
                                        found = true;
                                        break;
                                }
                        }

                        if (found) {
                                f.at(i) = true;
                        } else {
                                std::cerr << "ignoring unknown output option: " << ms << std::endl;
                        }
                }

                return f;
        } else {
                throw std::string("wrong format in ") + format;
        }
}

size_t read_sequence (std::string file, std::vector<std::pair<int, std::pair<NodeID, NodeID> > >& edge_sequence) {
        std::string line;
        std::ifstream input(file);
        edge_sequence.resize(0);
        size_t n = 0;
        int i = 0;

        if (input.is_open()) {
                std::getline(input, line);
                std::vector<std::string> substr = split(line, ' ');

                std::string hash = substr.at(0);
                if (hash != "#") throw std::string("META DATA SEEMS TO BE MISSING");

                n = std::stoul(substr.at(1).c_str());

                while (std::getline(input, line)) {
                        i++;
                        std::vector<std::string> substr = split(line, ' ');

                        int addition = atoi(substr.at(0).c_str());
                        NodeID u = atoi(substr.at(1).c_str());
                        NodeID v = atoi(substr.at(2).c_str());

                        edge_sequence.push_back({addition, {u, v}});
                }
        }

        input.close();

        return n;
}

std::string get_output_filename (ex_config conf) {
        std::string output = split(conf.file, '.').at(0) + "_";
        output = output + std::to_string(conf.at_once);

        std::vector<std::string> buf = split(output, '/');
        output = buf.at(buf.size() - 1);

        output = "output/" + output;

        std::cout << "printing output to directory: " << output << "/" << std::endl;

        return output;
}

//dyn_graph_access* create_graph (size_t n) {
        //dyn_graph_access * G = new dyn_graph_access();

        //G->start_construction(n);

        //for (size_t i = 0; i < n; ++i) {
                //G->new_node();
        //}

        //G->finish_construction();

        //return G;
//}

EdgeID get_cumulated_degree(dyn_graph_access& G, std::vector<std::pair<NodeID, NodeID> > edgeset) {
        EdgeID cum_degree = 0;

        for (auto e : edgeset) {
                cum_degree += G.getNodeDegree(e.first);
        }

        return cum_degree;
}

NodeID count_nodes(std::vector<bool>& nodes) {
        NodeID count = 0;

        for (auto n : nodes) {
                if (n) count++;
        }

        return count;
}

NodeID count_nodes(std::vector<int>& nodes, int min_degree) {
        NodeID count = 0;

        for (auto n : nodes) {
                if (n >= min_degree) count++;
        }

        return count;
}

dyn_matching * init_algorithm (ALGORITHM algorithm, dyn_graph_access * G, double eps, double multiple_rws, long low_degree, int threads) {
        if (algorithm == ALGORITHM::bgs) {
                return new baswanaguptasen_dyn_matching(G);

        } else if (algorithm == ALGORITHM::naive) {
                return new naive_dyn_matching(G);

        } else if (algorithm == ALGORITHM::ns) {
                return new neimansolomon_dyn_matching(G);

        } else if (algorithm == ALGORITHM::rw) {
                if (threads <= 1) {
                        return new rw_dyn_matching(G, eps, multiple_rws, false);
                } else {
                        return new parallel_rw_dyn_matching(G, eps, multiple_rws, false, false, nullptr, threads);
                }

        } else if (algorithm == ALGORITHM::mrw) {
                if (threads <= 1) {
                        return new rw_dyn_matching(G, eps, multiple_rws, true);
                } else {
                        return new parallel_rw_dyn_matching(G, eps, multiple_rws, true, false, nullptr, threads);
                }

        } else if (algorithm == ALGORITHM::sq_m_rw) {
                std::cout <<  "discarded for now"  << std::endl;
                exit(0);

        } else if (algorithm == ALGORITHM::lds) {
                unsigned long* buf;
                
                if (low_degree < 0) {
                        buf = nullptr;
                } else {
                        buf  = new unsigned long;
                        *buf = low_degree;
                }

                if (threads <= 1) {
                        return new rw_dyn_matching(G, eps, multiple_rws, true, true, buf);
                } else {
                        return new parallel_rw_dyn_matching(G, eps, multiple_rws, true, true, buf, threads);
                }
                
                if (buf != nullptr) {
                        delete buf;
                }

        } else if (algorithm == ALGORITHM::en) {
                return new en_dyn_matching(G, eps, multiple_rws);

        } else if (algorithm == ALGORITHM::ei_naive) {
                return new naive_dyn_matching(G, true);

        } else if (algorithm == ALGORITHM::mcm) {
                //~ std::cerr << eps << std::endl;
                std::cout <<  eps  << std::endl;
                return new blossom_dyn_matching(G, (eps));
        } else if (algorithm == ALGORITHM::parallel_rw) {
                std::cout <<  "usage option discarded for now. use rw, mrw or lds instead and pass thread number larger 1."  << std::endl;
                exit(0);

        } else {
                return nullptr;
        }
}
