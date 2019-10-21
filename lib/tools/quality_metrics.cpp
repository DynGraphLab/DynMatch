/******************************************************************************
 * quality_metrics.cpp
 *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 *
 ******************************************************************************
 * Copyright (C) 2013-2015 Christian Schulz <christian.schulz@kit.edu>
 *
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

#include <algorithm>
//#include <cmath>

#include "quality_metrics.h"

#include <unordered_map>
//#include <numeric>

std::vector<std::pair<NodeID, NodeID> > quality_metrics::edgeset_intersect (
        std::vector<std::pair<NodeID, NodeID> > m_one,
        std::vector<std::pair<NodeID, NodeID> > m_two,
        int& union_size) {
        std::vector<std::pair<NodeID, NodeID> > intersect;

        std::sort(
                m_one.begin(),
                m_one.end(),
        [](const std::pair<NodeID, NodeID>& a, const std::pair<NodeID, NodeID>& b) {
                return a.first < b.first;
        }
        );

        std::sort(
                m_two.begin(),
                m_two.end(),
        [](const std::pair<NodeID, NodeID>& a, const std::pair<NodeID, NodeID>& b) {
                return a.first < b.first;
        }
        );

        size_t i = 0, j = 0;

        while (i < m_one.size() && j < m_two.size()) {
                if (m_one.at(i).first == m_two.at(j).first) {
                        if (m_one.at(i).second == m_two.at(j).second) {
                                intersect.push_back(m_one.at(i));
                        }

                        ++i;
                        ++j;
                } else if (m_one.at(i).first < m_two.at(j).first) {
                        ++i;
                } else if (m_one.at(i).first > m_two.at(j).first) {
                        ++j;
                }
        }

        // return union_size as reference
        union_size = m_one.size() + m_two.size() - intersect.size(); // | A u B | = | A | + | B | - | A n B |
        return intersect;
}

#ifndef NDEBUG
bool quality_metrics::matching_validation (std::vector<std::pair<NodeID, NodeID> > matching) {
        for (auto edge : matching) {
                if(edge.first != edge.second) {
                        std::cout <<  "self-loop in matching"  << std::endl;
                        exit(0);
                }

        }
        
        // do this once for the left side...
        std::unordered_map<NodeID, NodeID> nodes_u;

        // go through matched edges
        for (auto edge : matching) {
                // assert that key u of edge (u,v) is not found in the map
                if(nodes_u.find(edge.first) == nodes_u.end()){
                        std::cout <<  "edges contained multiple times in matching"  << std::endl;
                        exit(0);
                }

                // add the edge to the map. if another edge should be incident
                // to the same node u, than the above assertion will fail.
                nodes_u[edge.first] = edge.second;
        }

        // ...and once for the right side
        std::unordered_map<NodeID, NodeID> nodes_v;

        // same as above, but checking with v as key of (u,v)
        for (auto edge : matching) {
                if(nodes_v.find(edge.second) == nodes_v.end()) {
                        std::cout <<  "edges contained multiple times in matching"  << std::endl;
                        exit(0);
                };
                nodes_v[edge.second] = edge.first;
        }

        return true;
}
#endif

