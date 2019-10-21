/******************************************************************************
 * graph_access.h 
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


#ifndef GRAPH_ACCESS_EFRXO4X2
#define GRAPH_ACCESS_EFRXO4X2
#include "static_graph.h"

class graph_access {
        public:
                graph_access(basicGraph * G) : graphref(G) { max_degree_computed = false; max_degree = 0; }
                graph_access() : graphref(NULL) { max_degree_computed = false; max_degree = 0; }
                virtual ~graph_access(){ };

                /* graph access methods */
                NodeID number_of_nodes();
                EdgeID number_of_edges();

                EdgeID get_first_edge(NodeID node);
                EdgeID get_first_invalid_edge(NodeID node);

                PartitionID get_partition_count(); 
                void set_partition_count(PartitionID count); 
                
                NodeWeight getNodeWeight(NodeID node);
                void setNodeWeight(NodeID node, NodeWeight weight);

                EdgeWeight getNodeDegree(NodeID node);
                EdgeWeight getWeightedNodeDegree(NodeID node);
                EdgeWeight getMaxDegree();

                EdgeWeight getEdgeWeight(EdgeID edge);
                void setEdgeWeight(EdgeID edge, EdgeWeight weight);

                NodeID getEdgeTarget(EdgeID edge);

                void setGraphReference(basicGraph * graph_reference);

                basicGraph * graphref;     

        private:
                bool max_degree_computed;
                EdgeWeight max_degree;
};

/* graph access methods */
inline NodeID graph_access::number_of_nodes() {
        return graphref->number_of_nodes();
}

inline EdgeID graph_access::number_of_edges() {
        return graphref->number_of_edges();
}

inline EdgeID graph_access::get_first_edge(NodeID node) {
#ifdef NDEBUG
        return graphref->nodes[node].firstEdge;
#else
        return graphref->nodes.at(node).firstEdge;
#endif
}

inline EdgeID graph_access::get_first_invalid_edge(NodeID node) {
        return graphref->firstInvalidEdge(node);
}

inline PartitionID graph_access::get_partition_count() {
        return graphref->get_partitionCount();
}



inline NodeWeight graph_access::getNodeWeight(NodeID node){
#ifdef NDEBUG
        return graphref->nodes[node].weight;        
#else
        return graphref->nodes.at(node).weight;        
#endif
}

inline void graph_access::setNodeWeight(NodeID node, NodeWeight weight){
#ifdef NDEBUG
        graphref->nodes[node].weight = weight;        
#else
        graphref->nodes.at(node).weight = weight;        
#endif
}

inline EdgeWeight graph_access::getEdgeWeight(EdgeID edge){
#ifdef NDEBUG
        return graphref->edges[edge].weight;        
#else
        return graphref->edges.at(edge).weight;        
#endif
}

inline void graph_access::setEdgeWeight(EdgeID edge, EdgeWeight weight){
#ifdef NDEBUG
        graphref->edges[edge].weight = weight;        
#else
        graphref->edges.at(edge).weight = weight;        
#endif
}

inline NodeID graph_access::getEdgeTarget(EdgeID edge){
#ifdef NDEBUG
        return graphref->edges[edge].targetNode;        
#else
        return graphref->edges.at(edge).targetNode;        
#endif
}

inline EdgeWeight graph_access::getNodeDegree(NodeID node) {
        return graphref->nodes[node+1].firstEdge-graphref->nodes[node].firstEdge;
}

inline EdgeWeight graph_access::getWeightedNodeDegree(NodeID node) {
	EdgeWeight degree = 0;
	for( unsigned e = graphref->nodes[node].firstEdge; e < graphref->nodes[node+1].firstEdge; ++e) {
		degree += getEdgeWeight(e);
	}
        return degree;
}

inline EdgeWeight graph_access::getMaxDegree() {
        
        if(!max_degree_computed) {
                //compute it
                basicGraph& ref = *graphref;
                forall_nodes(ref, node) {
                        EdgeWeight cur_degree = 0;
                        forall_out_edges(ref, e, node) {
                                cur_degree += getEdgeWeight(e);
                        } endfor
                        if(cur_degree > max_degree) {
                                max_degree = cur_degree;
                        }
                } endfor
                max_degree_computed = true;
        }

        return max_degree;
}


inline void graph_access::setGraphReference(basicGraph * graph_reference) {
        graphref = graph_reference;
}

#endif /* end of include guard: GRAPH_ACCESS_EFRXO4X2 */
