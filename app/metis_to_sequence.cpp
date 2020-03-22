#include <stdio.h>
#include <iostream>
#include <argtable3.h>
#include <sstream>
#include <regex.h>
#include "graph_access.h"
#include "graph_io.h"
#include "random_functions.h"


using namespace std;

int main(int argn, char **argv)
{
        const char *progname = argv[0];

        // Setup argtable parameters.
        struct arg_str *filename                    = arg_strn(NULL, NULL, "FILE", 1, 1, "Path to Metis file.");
        struct arg_lit *help                        = arg_lit0(NULL, "help","Print help.");
        struct arg_lit *shuffle                     = arg_lit0(NULL, "shuffle","Shuffle edges.");
        struct arg_int *user_seed                   = arg_int0(NULL, "seed", NULL, "Seed to use for the PRNG.");
        struct arg_end *end                         = arg_end(100);

        // Define argtable.
        void* argtable[] = {
                filename, shuffle, user_seed, help, end
        };
        // Parse arguments.
        int nerrors = arg_parse(argn, argv, argtable);

        // Catch case that help was requested.
        if (help->count > 0) {
                printf("Usage: %s", progname);
                arg_print_syntax(stdout, argtable, "\n");
                arg_print_glossary(stdout, argtable,"  %-40s %s\n");
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1;
        }

        if (nerrors > 0) {
                arg_print_errors(stderr, end, progname);
                printf("Try '%s --help' for more information.\n",progname);
                arg_freetable(argtable, sizeof(argtable) / sizeof(argtable[0]));
                return 1; 
        }

        std::string graph_filename;
        if(filename->count > 0) {
                graph_filename = filename->sval[0];
        }

        graph_access G; 
        graph_io gio;
        gio.readGraphWeighted(G, graph_filename);

        std::vector< std::pair< NodeID, NodeID > > sequence;
        forall_nodes(G, node) {
                forall_out_edges(G, e, node) {
                        NodeID target = G.getEdgeTarget(e);
                        std::pair< NodeID, NodeID > pair;
                        pair.first  = node;
                        pair.second = target;
                        sequence.push_back(pair);
                } endfor
        } endfor

        if(shuffle->count > 0) {
                if( user_seed->count > 0 ) {
                        random_functions::setSeed(user_seed->ival[0]);
                } else {
                        random_functions::setSeed(0);
                }
                random_functions::permutate_vector_anytype(sequence);
        }

        std::stringstream ss;
        ss << graph_filename << ".seq";
        
        ofstream f(ss.str());
        f << "# " << G.number_of_nodes() << " " << G.number_of_edges()/2 << std::endl;
        for( unsigned int i = 0; i < sequence.size(); i++) {
                f << "1 " << sequence[i].first << " " << sequence[i].second << std::endl;
        }

        f.close();
        
        return 0;
}

