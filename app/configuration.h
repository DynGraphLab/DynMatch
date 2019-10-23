/******************************************************************************
 * configuration.h 
 * *
 * Christian Schulz <christian.schulz.phone@gmail.com>
 *****************************************************************************/


#ifndef CONFIGURATION_3APG5V7Z
#define CONFIGURATION_3APG5V7Z

#include "match_config.h"

class configuration {
        public:
                configuration() {} ;
                virtual ~configuration() {};
                void standard( MatchConfig & config );

};

inline void configuration::standard( MatchConfig & config ) {
        config.filename                    = "";
        config.seed                        = 0;
        config.post_blossom                = false;
        config.post_mv                     = false;
        config.algorithm                   = RANDOM_WALK;
        config.rw_max_length               = 10;
        config.rw_low_degree_settle        = false;
        config.rw_ending_additional_settle = false;
        config.rw_low_degree_value         = 10;
        config.rw_repetitions_per_node     = 1;
}

#endif /* end of include guard: CONFIGURATION_3APG5V7Z */
