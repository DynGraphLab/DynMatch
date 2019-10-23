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
#include "configuration.h"

int parse_parameters(int argn, char **argv, 
                     MatchConfig & match_config, 
                     std::string & graph_filename) {

        const char *progname = argv[0];

        // Setup argtable parameters.
        struct arg_lit *help                        = arg_lit0(NULL, "help","Print help.");
        struct arg_str *filename                    = arg_strn(NULL, NULL, "FILE", 1, 1, "Path to graph file to compute matching from.");
        struct arg_int *user_seed                   = arg_int0(NULL, "seed", NULL, "Seed to use for the PRNG.");
        struct arg_rex *algorithm_type              = arg_rex0(NULL, "algorithm", "^(blossom|mv|randomwalk|neimansolomon|baswanaguptaseng)$", "TYPE", REG_EXTENDED, "Algorithm to use. One of {blossom, mv, randomwalk, neimansolomon, baswanaguptaseng}"  );

        struct arg_dbl *eps                         = arg_dbl0(NULL, "eps", NULL, "Epsilon.");
        struct arg_int *rw_low_degree_value         = arg_int0(NULL, "rw_low_degree_value", NULL, "Random Walk: Low degree value.");
        struct arg_lit *rw_ending_additional_settle = arg_lit0(NULL, "rw_ending_additional_settle","Random Walk: Enable additional settle for unsuccessful paths.");
        struct arg_int *rw_repetitions_per_node     = arg_int0(NULL, "rw_repetitions_per_node", NULL, "Random Walk: Number of rw repetitions per insert/delete.");
        struct arg_lit *naive_settle_on_insertion   = arg_lit0(NULL, "naive_settle_on_insertion","Naive: settle on insertion.");

        struct arg_lit *post_mv                     = arg_lit0(NULL, "post_mv","Run MV algorithm afterwards.");
        struct arg_lit *post_blossom                = arg_lit0(NULL, "post_blossom","Run Blossom algorithm afterwards.");
        struct arg_end *end                         = arg_end(100);

        // Define argtable.
        void* argtable[] = {
                help, filename, user_seed, algorithm_type, eps, rw_low_degree_value, rw_ending_additional_settle, rw_repetitions_per_node, naive_settle_on_insertion, post_mv, post_blossom, 
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
                if(strcmp("blossom", algorithm_type->sval[0]) == 0) {
                        match_config.algorithm = BLOSSOM;
                } else if (strcmp("mv", algorithm_type->sval[0]) == 0) {
                        match_config.algorithm = MV;
                } else if (strcmp("randomwalk", algorithm_type->sval[0]) == 0) {
                        match_config.algorithm = RANDOM_WALK;
                } else if (strcmp("neimansolomon", algorithm_type->sval[0]) == 0) {
                        match_config.algorithm = NEIMAN_SOLOMON;
                } else if (strcmp("baswanaguptaseng", algorithm_type->sval[0]) == 0) {
                        match_config.algorithm = BASWANA_GUPTA_SENG;
                } else {
                        fprintf(stderr, "Invalid algorithm variant: \"%s\"\n", algorithm_type->sval[0]);
                        exit(0);
                }
        }

        if(eps->count > 0 ){
                match_config.rw_max_length = ceil(2.0/eps->dval[0] - 1.0);
        }

        if(rw_low_degree_value->count > 0) {
                match_config.rw_low_degree_value  = rw_low_degree_value->ival[0];
                match_config.rw_low_degree_settle = true;
        }

        if(rw_ending_additional_settle->count > 0) {
                match_config.rw_ending_additional_settle = true;
        }

        if(rw_repetitions_per_node->count > 0) {
                match_config.rw_repetitions_per_node = rw_repetitions_per_node->ival[0];
        }

        if(naive_settle_on_insertion->count > 0) {
                match_config.naive_settle_on_insertion = true;
        }

        if(post_mv->count > 0) {
                match_config.post_mv = true;
        }

        if(post_blossom->count > 0) {
                match_config.post_blossom = true;
        }


        return 0;
}

#endif /* end of include guard: PARSE_PARAMETERS_GPJMGSM8 */
