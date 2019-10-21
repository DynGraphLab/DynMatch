/******************************************************************************
 * graph_access.h
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

#ifndef GRAPH_ACCESS_EFRXO4X2
#define GRAPH_ACCESS_EFRXO4X2

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>

#include "../definitions.h"


struct refinementNode {
        PartitionID partitionIndex;
        //Count queueIndex;
};

struct coarseningEdge {
        EdgeRatingType rating;
};

class graph_access;

//construction etc. is encapsulated in basicGraph / access to properties etc. is encapsulated in graph_access
class basicGraph {
                friend class graph_access;

        public:
                basicGraph() : m_building_graph(false) {
                }

        private:
                //methods only to be used by friend class
                EdgeID number_of_edges() {
                        return m_edges.size();
                }

                NodeID number_of_nodes() {
                        return m_nodes.size()-1;
                }

                inline EdgeID get_first_edge(const NodeID & node) {
                        return m_nodes[node].firstEdge;
                }

                inline EdgeID get_first_invalid_edge(const NodeID & node) {
                        return m_nodes[node+1].firstEdge;
                }

                // construction of the graph
                void start_construction(NodeID n, EdgeID m) {
                        m_building_graph = true;
                        node             = 0;
                        e                = 0;
                        m_last_source    = -1;

                        //resizes property arrays
                        m_nodes.resize(n+1);
                        m_refinement_node_props.resize(n+1);
                        m_edges.resize(m);
                        m_coarsening_edge_props.resize(m);

                        m_nodes[node].firstEdge = e;
                }

                EdgeID new_edge(NodeID source, NodeID target) {
                        ASSERT_TRUE(m_building_graph);
                        ASSERT_TRUE(e < m_edges.size());

                        m_edges[e].target = target;
                        EdgeID e_bar = e;
                        ++e;

                        ASSERT_TRUE(source+1 < m_nodes.size());
                        m_nodes[source+1].firstEdge = e;

                        //fill isolated sources at the end
                        if ((NodeID)(m_last_source+1) < source) {
                                for (NodeID i = source; i>(NodeID)(m_last_source+1); i--) {
                                        m_nodes[i].firstEdge = m_nodes[m_last_source+1].firstEdge;
                                }
                        }
                        m_last_source = source;
                        return e_bar;
                }

                NodeID new_node() {
                        ASSERT_TRUE(m_building_graph);
                        return node++;
                }

                void finish_construction() {
                        // inert dummy node
                        m_nodes.resize(node+1);
                        m_refinement_node_props.resize(node+1);

                        m_edges.resize(e);
                        m_coarsening_edge_props.resize(e);

                        m_building_graph = false;

                        //fill isolated sources at the end
                        if ((unsigned int)(m_last_source) != node-1) {
                                //in that case at least the last node was an isolated node
                                for (NodeID i = node; i>(unsigned int)(m_last_source+1); i--) {
                                        m_nodes[i].firstEdge = m_nodes[m_last_source+1].firstEdge;
                                }
                        }
                }

                // %%%%%%%%%%%%%%%%%%% DATA %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
                // split properties for coarsening and uncoarsening
                std::vector<Node> m_nodes;
                std::vector<Edge> m_edges;

                std::vector<refinementNode> m_refinement_node_props;
                std::vector<coarseningEdge> m_coarsening_edge_props;

                // construction properties
                bool m_building_graph;
                int m_last_source;
                NodeID node; //current node that is constructed
                EdgeID e;    //current edge that is constructed
};

//makros - graph access
#define forall_edges(G,e) { for(EdgeID e = 0, end = G.number_of_edges(); e < end; ++e) {
#define forall_nodes(G,n) { for(NodeID n = 0, end = G.number_of_nodes(); n < end; ++n) {
#define forall_out_edges(G,e,n) { for(EdgeID e = G.get_first_edge(n), end = G.get_first_invalid_edge(n); e < end; ++e) {
#define forall_out_edges_starting_at(G,e,n,e_bar) { for(EdgeID e = e_bar, end = G.get_first_invalid_edge(n); e < end; ++e) {
#define forall_blocks(G,p) { for (PartitionID p = 0, end = G.get_partition_count(); p < end; p++) {
#define endfor }}


class complete_boundary;

class graph_access {
                friend class complete_boundary;
        public:
                graph_access() {
                        m_max_degree_computed = false;
                        m_max_degree = 0;
                        graphref = new basicGraph();
                        m_separator_block_ID = 2;
                }
                virtual ~graph_access() {
                        delete graphref;
                };

                /* ============================================================= */
                /* build methods */
                /* ============================================================= */
                void start_construction(NodeID nodes, EdgeID edges);
                NodeID new_node();
                EdgeID new_edge(NodeID source, NodeID target);
                void finish_construction();

                /* ============================================================= */
                /* graph access methods */
                /* ============================================================= */
                NodeID number_of_nodes();
                EdgeID number_of_edges();

                EdgeID get_first_edge(NodeID node);
                EdgeID get_first_invalid_edge(NodeID node);

                PartitionID get_partition_count();
                void set_partition_count(PartitionID count);

                PartitionID getSeparatorBlock();
                void setSeparatorBlock(PartitionID id);

                PartitionID getPartitionIndex(NodeID node);
                void setPartitionIndex(NodeID node, PartitionID id);

                PartitionID getSecondPartitionIndex(NodeID node);
                void setSecondPartitionIndex(NodeID node, PartitionID id);

                //to be called if combine in meta heuristic is used
                void resizeSecondPartitionIndex(unsigned no_nodes);

                NodeWeight getNodeWeight(NodeID node);
                void setNodeWeight(NodeID node, NodeWeight weight);

                EdgeWeight getNodeDegree(NodeID node);
                EdgeWeight getWeightedNodeDegree(NodeID node);
                EdgeWeight getMaxDegree();

                EdgeWeight getEdgeWeight(EdgeID edge);
                void setEdgeWeight(EdgeID edge, EdgeWeight weight);

                NodeID getEdgeTarget(EdgeID edge);

                EdgeRatingType getEdgeRating(EdgeID edge);
                void setEdgeRating(EdgeID edge, EdgeRatingType rating);

                int* UNSAFE_metis_style_xadj_array();
                int* UNSAFE_metis_style_adjncy_array();

                int* UNSAFE_metis_style_vwgt_array();
                int* UNSAFE_metis_style_adjwgt_array();

                int build_from_metis(int n, int* xadj, int* adjncy);
                int build_from_metis_weighted(int n, int* xadj, int* adjncy, int * vwgt, int* adjwgt);

                //void set_node_queue_index(NodeID node, Count queue_index);
                //Count get_node_queue_index(NodeID node);

                void copy(graph_access & Gcopy);
        private:
                basicGraph * graphref;
                bool         m_max_degree_computed;
                unsigned int m_partition_count;
                EdgeWeight   m_max_degree;
                PartitionID  m_separator_block_ID;
                std::vector<PartitionID> m_second_partition_index;
};

#endif /* end of include guard: GRAPH_ACCESS_EFRXO4X2 */
