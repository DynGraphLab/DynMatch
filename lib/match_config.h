/******************************************************************************
 * match_config.h 
 *
 * Christian Schulz <christian.schulz.phone@gmail.com>
 *****************************************************************************/

#ifndef MATCH_CONFIG_DI1ES4T0
#define MATCH_CONFIG_DI1ES4T0

#include "definitions.h"

struct MatchConfig
{
        MatchConfig() {}

        PermutationQuality permutation_quality;

        AlgorithmType algorithm;

        BlossomInitType blossom_init;

        int seed;
        
        std::string filename;

        bool post_blossom;

        bool fast_rw;

        bool dynblossom_speedheuristic;

        bool dynblossom_weakspeedheuristic;

        bool measure_graph_construction_only;

        bool maintain_opt;

        double bgs_factor;

        //***************************
        // Random Walk Configurations
        //***************************
        int                     rw_max_length;
        bool                    rw_low_degree_settle;
        unsigned int            rw_low_degree_value;
        bool                    rw_ending_additional_settle;
        unsigned long long      rw_repetitions_per_node;

        bool naive_settle_on_insertion;

        void LogDump(FILE *out) const {
        }
};


#endif /* end of include guard: MATCH_CONFIG_DI1ES4T0 */
