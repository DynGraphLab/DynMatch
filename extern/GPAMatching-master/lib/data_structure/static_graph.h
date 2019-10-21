/******************************************************************************
 * static_graph.h 
 *
 * Source of GPA Matching Code 
 *
 ******************************************************************************
 * Copyright (C) 2013 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
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

#ifndef STATICGRAPHMETAHEUR_H
#define STATICGRAPHMETAHEUR_H

#include "../definitions.h"
#include <vector>
#include <cassert>
#include <iostream>
#include <bitset>

struct basicNode {
    EdgeID firstEdge;
    NodeWeight weight;
};

struct basicEdge {
    NodeID targetNode;
    EdgeWeight weight;
};

/**
 * static graph in forward star representation
 */

template<typename NodeSlot, typename EdgeSlot> class staticGraph {

public:
    typedef NodeSlot NodeType;
    typedef EdgeSlot EdgeType;

    //Static Part of our Graph
    std::vector<NodeSlot> nodes;
    std::vector<EdgeSlot> edges;
    
protected:
    bool construction;
private:
    // Construction

    NodeID constructionNode;
    EdgeID constructionEdge;
    int lastSource;
    unsigned int partionCount;
    bool lastNodeSingleton;

public:

    staticGraph() : construction(false), lastNodeSingleton(true) {
    }

    void set_partitionCount(unsigned int count) {
        this->partionCount = count;
    }

    unsigned int get_partitionCount() {
        return partionCount;
    }

    // ------------------------------------------------------------------------
    // construction of the graph
    void start_construction(NodeID n, EdgeID m) {
        construction = true;
        constructionNode = 0;
        constructionEdge = 0;
        lastSource = -1;
        nodes.resize(n+1);
        edges.resize(m);
        nodes[constructionNode].firstEdge = constructionEdge;
    }

    EdgeID new_edge(NodeID s, NodeID t) {
        assert(construction);
        assert(constructionEdge < edges.size());
        edges[constructionEdge].targetNode = t;
        EdgeID oldEdge = constructionEdge;
        ++constructionEdge;
        assert(s+1 < nodes.size());
        nodes[s+1].firstEdge = constructionEdge;
        //fill isolated sources
        if ((unsigned int)(lastSource+1) < s) {
            for (NodeID i = s; i>(unsigned int)(lastSource+1); i--) {
                nodes[i].firstEdge = nodes[lastSource+1].firstEdge;
            }
        }
        lastSource = s;
        return oldEdge;
    }

    NodeID new_node() {
        assert(construction);
        return constructionNode++;
    }

    void finish_construction() {
        // dummy node
        nodes.resize(constructionNode+1);
        edges.resize(constructionEdge);
        construction = false;

        //fill isolated sources
        if ((unsigned int)(lastSource) != constructionNode-1) {
                //in that case at least the last node was an isolated node
                for (NodeID i = constructionNode; i>(unsigned int)(lastSource+1); i--) {
                        nodes[i].firstEdge = nodes[lastSource+1].firstEdge;
                }
        }

#ifndef NDEBUG
        for( NodeID node = 0; node < nodes.size()-1; node++) {        
                if((int)nodes[node+1].firstEdge-(int)nodes[node].firstEdge<0) {
                        cout << "node with negative degree " << node  << endl;
                }
                assert((int)nodes[node+1].firstEdge-(int)nodes[node].firstEdge>=0);
        } 
#endif
    }

    void set_construction(bool b) {
        construction = b;
    }

    // ------------------------------------------------------------------------
    EdgeID number_of_edges() {
        assert(!construction);
        return edges.size();
    }

    EdgeID number_of_valid_edges() {
        assert(!construction);
        return edges.size();
    }
    NodeID number_of_nodes() {
        //assert(!construction);
        return nodes.size()-1;
    }

    //return u --> v, careful, this edge must exist
    inline EdgeID getEdge(const NodeID & u, const NodeID & v) {
        EdgeID e = nodes[u].firstEdge;
        while (edges[e].targetNode != v || !edges[e].valid)
            e++;
        return e;
    }

    //checks whether u --> v exists in this Graph
    inline bool checkEdge(const NodeID & u, const NodeID & v) {
        EdgeID e = nodes[u].firstEdge;
        while (edges[e].targetNode != v || !edges[e].valid) {
            e++;
            if (e>=nodes[u+1].firstEdge)
                return false;
        }
        return true;
    }

    inline int maxDegree() {
        int maxDeg= INT_MIN;
        int degree = 0;
        for (NodeID n = 0; n < number_of_nodes(); ++n) {
            degree = firstInvalidEdge(n) - nodes[n].firstEdge;
            if (degree > maxDeg) {
                maxDeg = degree;
            }
        }
        return maxDeg;
    }

    inline bool valid(const EdgeID & e, NodeID &s) {
        if (e<number_of_edges()) {
            while (nodes[s+1].firstEdge == e)
                s++;
            return true;
        }
        return false;
    }

    inline EdgeID get_first_edge(const NodeID & node) {
        return nodes[node].firstEdge;
    }

    inline EdgeID get_first_invalid_edge(const NodeID & node) {
            return firstInvalidEdge(node);
    }

    //TODO: remove name duplicates
    inline EdgeID firstInvalidEdge(const NodeID& n) {
        return nodes[n+1].firstEdge;
    }

    void dump_info() {
        cout << "graph has " << number_of_nodes() << " nodes and " << number_of_edges() << " edges" << endl;
    }


    /** Prints the total memory usage. */
    void printMemoryUsage(ostream& out) const {
        out << "** memory usage on hard disk [MB (bytes per node)] **" << endl;

        unsigned int memoryTotal = 0;
        memoryTotal += printMemoryUsage(out, "nodes", (nodes.size()-1) * sizeof(NodeType));
        memoryTotal += printMemoryUsage(out, "edges", (edges.size()-1) * sizeof(EdgeType));

        printMemoryUsage(out, "TOTAL", memoryTotal);
        out << endl;
    }

    /** Prints the memory usage of one particular data structure of this UpdateableGraph. */
    unsigned int printMemoryUsage(ostream& out, const string descr, const unsigned int mem) const {
        double megaBytes = (mem / (double)(1024*1024)) + 0.5;
        unsigned int bytesPerNode = (unsigned int)((mem / (double)(nodes.size()-1)) + 0.5);
        out << "   " << descr << ": " << megaBytes << " (" << bytesPerNode << ")" << endl;
        return mem;
    }
};

//different Graph definitions
typedef staticGraph<basicNode, basicEdge> basicGraph;


//makros - graph access
//iterate all edges in Graph
#define forall_edges(G,e) { for(EdgeID e = 0; e < G.number_of_edges(); ++e) {
//iterate all nodes in Graph
#define forall_nodes(G,n) { for(NodeID n = 0; n < G.number_of_nodes(); ++n) {
//iterate over all out edges
//#define forall_out_edges(G,e,n) { for(EdgeID e = G.get_first_edge(n); e < G.get_first_invalid_edge(n); ++e) {
#define forall_out_edges(G,e,n) { for(EdgeID e = G.get_first_edge(n), end = G.get_first_invalid_edge(n); e < end; ++e) {

#define forall_blocks(G,p) { for (PartitionID p = 0; p < G.get_partition_count(); p++) {
//makros - graph
//iterate all incoming and outgoing edges of a node
#define forall_in_out_edges(G,e,n) { for(EdgeID e = G.nodes[n].firstEdge; e < G.firstInvalidEdge(n); ++e) {

#define endfor }}


using namespace std;
template<typename graphType> void printGraph(graphType & G) {
    cout << "%%%%%%%%%%%% Printing Graph %%%%%%%%%%%%%%%" << endl;
    forall_nodes(G, n) 
        cout << "Node: " << n << endl;
        cout << "Edges: ";
        forall_in_out_edges(G, e, n) 
            cout << G.edges[e].targetNode << " ";
        endfor
        cout << endl;
    endfor
    cout << "%%%%%%%%%% End Printing Graph %%%%%%%%%%%%%" << endl;
}
#endif // STATICGRAPH_H
