#include "dynamic_matching.h"
#include "executer.h"

#include <iostream>
#include <string>

bool match (std::string a, std::string b);
std::vector<std::string> split (const std::string& input, const char& mark);

int main (int argc, char ** argv) {
        std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;
        size_t n;

        if (argc > 1) {
                std::string file(argv[1]);
                n = read_sequence(file, edge_sequence);
        }

        std::cout << "read sequence of length " << edge_sequence.size() << ", with n=" << n << std::endl;

        bool cont = true;

        std::string buf;
        size_t stepsize = 1;
        size_t iterator = 0;

        dyn_graph_access * G = create_graph(n);

        while (cont && (std::cout << "> ", std::getline(std::cin, buf))) {
                std::vector<std::string> input = split(buf, ' ');

                if (match(input[0], "quit")) {
                        cont = false;
                } else if (match(input[0], "iterate")) {
                        for (size_t i = 0; i < stepsize; ++i) {
                                if (iterator+1 < edge_sequence.size()) {
                                        bool add = edge_sequence.at(iterator).first;
                                        std::pair<NodeID, NodeID> edge = edge_sequence.at(iterator).second;
                                        std::cout << (add ? "inserting" : "removing")
                                                  << " edge (" << edge.first << ", " << edge.second << ")" << std::endl;

                                        if (add) {
                                                G->new_edge(edge.first, edge.second);
                                                G->new_edge(edge.second, edge.first);
                                        } else {
                                                G->remove_edge(edge.first, edge.second);
                                                G->remove_edge(edge.second, edge.first);
                                        }

                                        iterator++;
                                } else {
                                        std::cout << "reached last sequence step." << std::endl;
                                }
                        }
                } else if (match(input[0], "reverse")) {
                        for (size_t i = 0; i < stepsize; ++i) {
                                if (iterator > 0) {
                                        iterator--;

                                        bool add = !edge_sequence.at(iterator).first;
                                        std::pair<NodeID, NodeID> edge = edge_sequence.at(iterator).second;
                                        std::cout << (add ? "inserting" : "removing")
                                                  << " edge (" << edge.first << ", " << edge.second << ")" << std::endl;

                                        if (add) {
                                                G->new_edge(edge.first, edge.second);
                                                G->new_edge(edge.second, edge.first);
                                        } else {
                                                G->remove_edge(edge.first, edge.second);
                                                G->remove_edge(edge.second, edge.first);
                                        }
                                } else {
                                        std::cout << "reached first sequence step." << std::endl;
                                }
                        }
                } else if (match(input[0], "stepsize")) {
                        try {
                                stepsize = std::stoi(input[1]);

                                std::cout << "set step size to " << stepsize << std::endl;
                        } catch (...) {
                                std::cout << "could not parse second argument to int." << std::endl;
                        }
                } else if (match(input[0], "nodes") || match(input[0], "vertices")) {
                        std::cout << "n=" << G->number_of_nodes() << std::endl;
                } else if (match(input[0], "m") || match(input[0], "edges")) {
                        std::cout << "m=" << G->number_of_edges() << std::endl;
                } else if (match(input[0], "neighbours")) {
                        NodeID u;

                        try {
                                u = std::stoi(input[1]);
                        } catch (...) {
                                std::cout << "could not parse second argument to int." << std::endl;
                                continue;
                        }

                        if (u >= G->number_of_nodes()) {
                                std::cout << "invalid node id." << std::endl;
                                continue;
                        }

                        if (G->getNodeDegree(u) > 0) {
                                std::cout << "neighbours of vertex " << u << " are: " << std::endl;
                                //for (auto e : G->getEdgesFromNode(u)) {
                                for( int e = 0; e < G->getNodeDegree(u); e++) {
                                        std::cout << G->getEdgeTarget(u,e) << " ";
                                }
                                std::cout << std::endl;
                        } else {
                                std::cout << "vertex " << u << " is isolated." << std::endl;
                        }
                }
        }
}

bool match (std::string a, std::string b) {
        bool m = true;

        unsigned i = 0, j = 0;

        while (i < a.length() && j < b.length()) {
                if (a.at(i++) != b.at(j++)) {
                        return false;
                }
        }

        return m;
}
