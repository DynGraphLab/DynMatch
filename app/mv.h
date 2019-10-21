#include "Graph.h"
#include "graph_simple.h"
#include "read_table.h"

#include <iostream>

#include "dynamic_matching.h"

void mv (dyn_graph_access& G, NodeID& matching_size, double& time_elapsed, std::vector<NodeID> matching) {
        graph g1;
        std::unordered_map<unsigned int, unsigned int> ids;
        std::vector<std::pair<unsigned int, unsigned int> > _matching;
        
        std::vector<unsigned int> e1 = std::vector<unsigned int>(0), 
                                  e2 = std::vector<unsigned int>(0);
        
        NodeID v;
        
        for (NodeID u = 0; u < G.number_of_nodes(); ++u) {
                for (EdgeID e = 0; e < G.getNodeDegree(u); ++e) {
                        v = G.getEdgeTarget(u, e);
                        
                        e1.push_back(u);
                        e2.push_back(v);
                }
        }
        
        g1.create_graph_vectors(e1, e2, &ids);
        
        MVGraph g(g1);

        for(size_t i = 0; i < matching.size(); ++i) {
                if (matching[i] != NOMATE) {
                        _matching.push_back({ids[i], ids[matching[i]]});
                }
        }
        
        g.init(_matching);

        timer::restart();
        g.max_match();
        time_elapsed = timer::elapsed();
        
        matching_size = g.matchnum;
        
        g.check();
}
