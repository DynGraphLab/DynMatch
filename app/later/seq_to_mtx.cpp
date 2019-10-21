#include "executer.h"

#include <iostream>
#include <string>
#include <vector>

typedef std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;

int main (int argc, char ** argv) {
        if (argc <= 1) {
                std::cerr << "error: no sequence file passed" << std::endl;
                return 1;
        }
        
        std::string file(argv[1]);
        edge_sequence seq;
	
        size_t until = std::numeric_limits<size_t>::max();
        if (argc >= 3) {
                try {
                        until = stoul(std::string(argv[2]));
                } catch (...) {
                        std::cerr << "error: could not parse " << argv[2] << " to numeric" << std::endl;
                        return 1;
                }
        }
       	
        size_t n = read_sequence(file, seq);
        dyn_graph_access* G = create_graph(n);
	
        bool check = true;
        
        // first construct a graph from all edges, which are missing in the final graph, but were
        // inserted at some previous point
	for (size_t i = 0; i < seq.size() && i < until; ++i) {
                check = true;
                
                if (seq[i].second.first == seq[i].second.second) {
                        std::cerr << "self loop found" << std::endl;
                }
                
                if (seq[i].first == INSERTION) {
                        check = check and G->remove_edge(seq[i].second.first, seq[i].second.second);
                        check = check and G->remove_edge(seq[i].second.second, seq[i].second.first);
                } else {
                        check = check and G->new_edge(seq[i].second.first, seq[i].second.second);
                        check = check and G->new_edge(seq[i].second.second, seq[i].second.first);
                }
                
                if (check) { // will always be false, because we are trying to remove edges, that may not exist in the graph -> easy example: an insertion-only sequence
                        std::cerr << "double edge detected" << std::endl;
                }
        }
       
        EdgeID e;
        NodeID u, v;
        NodeID count = 0;
        
        for (size_t i = 0; i < seq.size() && i < until; ++i) {
                u = seq[i].second.first;
                v = seq[i].second.second;
                
                if (!G->isEdge(u, v)) {
                        ++count;
                }
        }
        
        std::cout << "%%MatrixMarket matrix coordinate integer symmetric" << std::endl;
        std::cout << n << " " << n << " " << count << std::endl;
        
        for (size_t i = 0; i < seq.size() && i < until; ++i) {
                u = seq[i].second.first;
                v = seq[i].second.second;
                
                if (!G->isEdge(u, v)) {
                        std::cout << u+1 << " " << v+1 << " " << 1 << std::endl;
                }
        }
        
        //~ for (u = 0; u < G->number_of_nodes(); ++u) {
                //~ for (e = 0; e < G->getNodeDegree(u); ++e) {
                        //~ v = G->getEdgeTarget(u,e);
                        //~ if (u < v) {
                                //~ edgelist.push_back(u, v);
                                //~ std::cout << u << " " << v << std::endl;
                        //~ }
                //~ }
        //~ }
        
        //~ std::shuffle()
}

