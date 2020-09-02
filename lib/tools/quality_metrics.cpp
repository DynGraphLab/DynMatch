/******************************************************************************
 * quality_metrics.cpp
 *
 *****************************************************************************/

#include <algorithm>
#include "quality_metrics.h"
#include <unordered_map>

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

