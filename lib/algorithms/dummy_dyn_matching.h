/******************************************************************************
 * dummy_dyn_matching.h
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

#ifndef DUMMY_DYN_MATCHING_H
#define DUMMY_DYN_MATCHING_H

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

#include "dyn_matching.h"


class dummy_dyn_matching : public dyn_matching {
        public:
                dummy_dyn_matching (dyn_graph_access* G) : dyn_matching(G) {
                        
                }

                virtual bool new_edge(NodeID source, NodeID target) {
                        bool foo = G->new_edge(source, target);
                        bool bar = G->new_edge(target, source);
                        return foo && bar;
                }

                virtual bool remove_edge(NodeID source, NodeID target) {
                        bool foo = G->remove_edge(source, target);
                        bool bar = G->remove_edge(target, source);
                        return foo && bar;
                }

                virtual std::vector<NodeID> getM () {
                        return std::vector<NodeID>(G->number_of_nodes(), NOMATE);
                }

                virtual size_t getMSize () {
                        return 0;
                }

                virtual void counters_next() {
                        
                }
};

#endif // DUMMY_DYN_MATCHING_H
