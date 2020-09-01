/******************************************************************************
 * parse_parameters.h 
 * *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 * Christian Schulz <christian.schulz.phone@gmail.com>
 *****************************************************************************/


#ifndef PARSE_PARAMETERS_GPJMGSM8_MATCH
#define PARSE_PARAMETERS_GPJMGSM8_MATCH

#include <omp.h>
#include <sstream>
#include <argtable3.h>
#include <regex.h>
#include <string.h>
#include "configuration.h"

int parse_parameters(int argn, char **argv, 
                     MatchConfig & match_config, 
                     std::string & graph_filename) {

        const char *progname = argv[0];

        // Setup argtable parameters.
        struct arg_lit *help                        = arg_lit0(NULL, "help","Print help.");
        struct arg_str *filename                    = arg_strn(NULL, NULL, "FILE", 1, 1, "Path to graph file to compute matching from.");
        struct arg_int *user_seed                   = arg_int0(NULL, "seed", NULL, "Seed to use for the PRNG.");
        struct arg_rex *algorithm_type              = arg_rex1(NULL, "algorithm", "^(staticblossom|dynblossom|naive|randomwalk|neimansolomon|baswanaguptasen)$", "TYPE", REG_EXTENDED, "Algorithm to use. One of {staticblossom, dynblossom, randomwalk, neimansolomon, baswanaguptasen}"  );

        struct arg_dbl *eps                         = arg_dbl0(NULL, "eps", NULL, "Epsilon. Limit search depth of random walk or augmenting path search in dynblossom to 2/eps-1.");
        struct arg_lit *dynblossom_speedheuristic   = arg_lit0(NULL, "dynblossom_lazy","Lazy approach for dynblossom, i.e. only start augmenting path searchs after x newly inserted edges on an endpoint.");
        //struct arg_lit *dynblossom_weakspeedheuristic = arg_lit0(NULL, "dynblossom_weakspeedheuristic","WeakSpeedHeuristic for dynblossom.");
        struct arg_lit *dynblossom_maintain_opt     = arg_lit0(NULL, "dynblossom_maintain_opt","Maintain optimum in dynblossom. (Without this option the algorithm is called UNSAFE.)");
        struct arg_dbl *bgs_factor                  = arg_dbl0(NULL, "bgs_factor", NULL, "BGS factor.");

        struct arg_lit *measure_graph_only          = arg_lit0(NULL, "measure_graph_only","Only measure graph construction time.");
        struct arg_end *end                         = arg_end(100);

        // Define argtable.
        void* argtable[] = {
                help, filename, user_seed, algorithm_type, 
                eps, 
                dynblossom_speedheuristic, 
                //dynblossom_weakspeedheuristic, 
                dynblossom_maintain_opt, 
                measure_graph_only, 
                end
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

        if(filename->count > 0) {
                graph_filename = filename->sval[0];
        }

        configuration cfg;
        cfg.standard(match_config);

        if (algorithm_type->count > 0) {
                if(strcmp("staticblossom", algorithm_type->sval[0]) == 0) {
                        match_config.algorithm = BLOSSOM;
                        std::cout <<  "running static blossom after makeing all updates"  << std::endl;
                } else if (strcmp("randomwalk", algorithm_type->sval[0]) == 0) {
                        match_config.algorithm = RANDOM_WALK;
                        std::cout <<  "running dynamic random walks"  << std::endl;
                } else if (strcmp("dynblossom", algorithm_type->sval[0]) == 0) {
                        match_config.algorithm = DYNBLOSSOM;
                        match_config.rw_max_length = std::numeric_limits< int >::max() / 2;
                        std::cout <<  "running dynamic blossom algorithm"  << std::endl;
                } else if (strcmp("neimansolomon", algorithm_type->sval[0]) == 0) {
                        match_config.algorithm = NEIMAN_SOLOMON;
                        std::cout <<  "running dynamic neiman solomon"  << std::endl;
                } else if (strcmp("baswanaguptasen", algorithm_type->sval[0]) == 0) {
                        match_config.algorithm = BASWANA_GUPTA_SENG;
                        std::cout <<  "running dynamic baswana gupta sen"  << std::endl;
                } else {
                        fprintf(stderr, "Invalid algorithm variant: \"%s\"\n", algorithm_type->sval[0]);
                        exit(0);
                }
        }

        if (measure_graph_only->count > 0) {
                match_config.measure_graph_construction_only = true;
        }

        if(eps->count > 0 ){
                match_config.rw_max_length = ceil(2.0/eps->dval[0] - 1.0);
        }

        if(user_seed->count > 0) {
                match_config.seed = user_seed->ival[0];
        }

        if(dynblossom_speedheuristic->count > 0) {
                match_config.dynblossom_speedheuristic = true;
        }

        //if(dynblossom_weakspeedheuristic->count > 0) {
                //match_config.dynblossom_weakspeedheuristic = true;
        //}

        if(dynblossom_maintain_opt->count > 0) {
                match_config.maintain_opt = true;
        }

        if(bgs_factor->count > 0) {
                match_config.bgs_factor = bgs_factor->dval[0];
        }

        return 0;
}

#endif /* end of include guard: PARSE_PARAMETERS_GPJMGSM8 */
