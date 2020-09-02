/******************************************************************************
 * naive_dyn_matching.h
 *
 *
 *****************************************************************************/

#ifndef NAIVE_DYN_MATCHING_H
#define NAIVE_DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#include "dyn_matching.h"
#include "tools/random_functions.h"
#include "tools/timer.h"


class naive_dyn_matching : public dyn_matching {
        public:
                naive_dyn_matching (dyn_graph_access* G, MatchConfig & config);

                virtual bool new_edge(NodeID source, NodeID target);
                virtual bool remove_edge(NodeID source, NodeID target);

        protected:
                virtual bool settle (NodeID u, const NodeID* avoid_ptr = nullptr);

};

#endif // NAIVE_DYN_MATCHING_H
