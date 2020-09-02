/******************************************************************************
 * dyn_matching.h
 *
 *
 *****************************************************************************/

#ifndef DYN_MATCHING_H
#define DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "data_structure/dyn_graph_access.h"
#include "definitions.h"
#include "match_config.h"

class dyn_matching {
        public:
                dyn_matching (dyn_graph_access* G, MatchConfig & config);
                virtual ~dyn_matching ();

                virtual bool new_edge(NodeID source, NodeID target) = 0;
                virtual bool remove_edge(NodeID source, NodeID target) = 0;

                virtual std::vector< NodeID > & getM ();
                virtual NodeID getMSize ();

                virtual void retry () { }
                virtual void postprocessing() { }
        protected:
                dyn_graph_access* G;

                std::vector<NodeID> matching;
                NodeID matching_size;
                MatchConfig config;

                virtual bool is_free (NodeID u);
                virtual NodeID mate (NodeID u);
                virtual bool is_matched (NodeID u, NodeID v);
                virtual void match (NodeID u, NodeID v);
                virtual void unmatch (NodeID u, NodeID v);
};

#endif // DYN_MATCHING_H
