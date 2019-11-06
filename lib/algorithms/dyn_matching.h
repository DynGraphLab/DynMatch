/******************************************************************************
 * dyn_matching.h
 *
 *
 ******************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
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
