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

#include "graph_access.h"

/* graph build methods */
/*inline*/ void graph_access::start_construction(NodeID nodes, EdgeID edges) {
        graphref->start_construction(nodes, edges);
}

/*inline*/ NodeID graph_access::new_node() {
        return graphref->new_node();
}

/*inline*/ EdgeID graph_access::new_edge(NodeID source, NodeID target) {
        return graphref->new_edge(source, target);
}

/*inline*/ void graph_access::finish_construction() {
        graphref->finish_construction();
}

/* graph access methods */
/*inline*/ NodeID graph_access::number_of_nodes() {
        return graphref->number_of_nodes();
}

/*inline*/ EdgeID graph_access::number_of_edges() {
        return graphref->number_of_edges();
}

/*inline*/ void graph_access::resizeSecondPartitionIndex(unsigned no_nodes) {
        m_second_partition_index.resize(no_nodes);
}

/*inline*/ EdgeID graph_access::get_first_edge(NodeID node) {
#ifdef NDEBUG
        return graphref->m_nodes[node].firstEdge;
#else
        return graphref->m_nodes.at(node).firstEdge;
#endif
}

/*inline*/ EdgeID graph_access::get_first_invalid_edge(NodeID node) {
        return graphref->m_nodes[node+1].firstEdge;
}

/*inline*/ PartitionID graph_access::get_partition_count() {
        return m_partition_count;
}

/*inline*/ PartitionID graph_access::getSecondPartitionIndex(NodeID node) {
#ifdef NDEBUG
        return m_second_partition_index[node];
#else
        return m_second_partition_index.at(node);
#endif
}

/*inline*/ void graph_access::setSecondPartitionIndex(NodeID node, PartitionID id) {
#ifdef NDEBUG
        m_second_partition_index[node] = id;
#else
        m_second_partition_index.at(node) = id;
#endif
}


/*inline*/ PartitionID graph_access::getSeparatorBlock() {
        return m_separator_block_ID;
}

/*inline*/ void graph_access::setSeparatorBlock(PartitionID id) {
        m_separator_block_ID = id;
}

/*inline*/ PartitionID graph_access::getPartitionIndex(NodeID node) {
#ifdef NDEBUG
        return graphref->m_refinement_node_props[node].partitionIndex;
#else
        return graphref->m_refinement_node_props.at(node).partitionIndex;
#endif
}

/*inline*/ void graph_access::setPartitionIndex(NodeID node, PartitionID id) {
#ifdef NDEBUG
        graphref->m_refinement_node_props[node].partitionIndex = id;
#else
        graphref->m_refinement_node_props.at(node).partitionIndex = id;
#endif
}

/*inline*/ NodeWeight graph_access::getNodeWeight(NodeID node) {
#ifdef NDEBUG
        return graphref->m_nodes[node].weight;
#else
        return graphref->m_nodes.at(node).weight;
#endif
}

/*inline*/ void graph_access::setNodeWeight(NodeID node, NodeWeight weight) {
#ifdef NDEBUG
        graphref->m_nodes[node].weight = weight;
#else
        graphref->m_nodes.at(node).weight = weight;
#endif
}

/*inline*/ EdgeWeight graph_access::getEdgeWeight(EdgeID edge) {
#ifdef NDEBUG
        return graphref->m_edges[edge].weight;
#else
        return graphref->m_edges.at(edge).weight;
#endif
}

/*inline*/ void graph_access::setEdgeWeight(EdgeID edge, EdgeWeight weight) {
#ifdef NDEBUG
        graphref->m_edges[edge].weight = weight;
#else
        graphref->m_edges.at(edge).weight = weight;
#endif
}

/*inline*/ NodeID graph_access::getEdgeTarget(EdgeID edge) {
#ifdef NDEBUG
        return graphref->m_edges[edge].target;
#else
        return graphref->m_edges.at(edge).target;
#endif
}

/*inline*/ EdgeRatingType graph_access::getEdgeRating(EdgeID edge) {
#ifdef NDEBUG
        return graphref->m_coarsening_edge_props[edge].rating;
#else
        return graphref->m_coarsening_edge_props.at(edge).rating;
#endif
}

/*inline*/ void graph_access::setEdgeRating(EdgeID edge, EdgeRatingType rating) {
#ifdef NDEBUG
        graphref->m_coarsening_edge_props[edge].rating = rating;
#else
        graphref->m_coarsening_edge_props.at(edge).rating = rating;
#endif
}

/*inline*/ EdgeWeight graph_access::getNodeDegree(NodeID node) {
        return graphref->m_nodes[node+1].firstEdge-graphref->m_nodes[node].firstEdge;
}

/*inline*/ EdgeWeight graph_access::getWeightedNodeDegree(NodeID node) {
        EdgeWeight degree = 0;
        for( unsigned e = graphref->m_nodes[node].firstEdge; e < graphref->m_nodes[node+1].firstEdge; ++e) {
                degree += getEdgeWeight(e);
        }
        return degree;
}

/*inline*/ EdgeWeight graph_access::getMaxDegree() {
        if(!m_max_degree_computed) {
                //compute it
                basicGraph& ref = *graphref;
                forall_nodes(ref, node) {
                        EdgeWeight cur_degree = 0;
                        forall_out_edges(ref, e, node) {
                                cur_degree += getEdgeWeight(e);
                        }
                        endfor
                        if(cur_degree > m_max_degree) {
                                m_max_degree = cur_degree;
                        }
                }
                endfor
                m_max_degree_computed = true;
        }

        return m_max_degree;
}

/*inline*/ int* graph_access::UNSAFE_metis_style_xadj_array() {
        int * xadj      = new int[graphref->number_of_nodes()+1];
        basicGraph& ref = *graphref;

        forall_nodes(ref, n) {
                xadj[n] = graphref->m_nodes[n].firstEdge;
        }
        endfor
        xadj[graphref->number_of_nodes()] = graphref->m_nodes[graphref->number_of_nodes()].firstEdge;
        return xadj;
}


/*inline*/ int* graph_access::UNSAFE_metis_style_adjncy_array() {
        int * adjncy    = new int[graphref->number_of_edges()];
        basicGraph& ref = *graphref;
        forall_edges(ref, e) {
                adjncy[e] = graphref->m_edges[e].target;
        }
        endfor

        return adjncy;
}


/*inline*/ int* graph_access::UNSAFE_metis_style_vwgt_array() {
        int * vwgt      = new int[graphref->number_of_nodes()];
        basicGraph& ref = *graphref;

        forall_nodes(ref, n) {
                vwgt[n] = (int)graphref->m_nodes[n].weight;
        }
        endfor
        return vwgt;
}

/*inline*/ int* graph_access::UNSAFE_metis_style_adjwgt_array() {
        int * adjwgt    = new int[graphref->number_of_edges()];
        basicGraph& ref = *graphref;

        forall_edges(ref, e) {
                adjwgt[e] = (int)graphref->m_edges[e].weight;
        }
        endfor

        return adjwgt;
}

/*inline*/ void graph_access::set_partition_count(PartitionID count) {
        m_partition_count = count;
}

/*inline*/ int graph_access::build_from_metis(int n, int* xadj, int* adjncy) {
        graphref = new basicGraph();
        start_construction(n, xadj[n]);

        for( unsigned i = 0; i < (unsigned)n; i++) {
                NodeID node = new_node();
                setNodeWeight(node, 1);
                setPartitionIndex(node, 0);

                for( unsigned e = xadj[i]; e < (unsigned)xadj[i+1]; e++) {
                        EdgeID e_bar = new_edge(node, adjncy[e]);
                        setEdgeWeight(e_bar, 1);
                }

        }

        finish_construction();
        return 0;
}

/*inline*/ int graph_access::build_from_metis_weighted(int n, int* xadj, int* adjncy, int * vwgt, int* adjwgt) {
        graphref = new basicGraph();
        start_construction(n, xadj[n]);

        for( unsigned i = 0; i < (unsigned)n; i++) {
                NodeID node = new_node();
                setNodeWeight(node, vwgt[i]);
                setPartitionIndex(node, 0);

                for( unsigned e = xadj[i]; e < (unsigned)xadj[i+1]; e++) {
                        EdgeID e_bar = new_edge(node, adjncy[e]);
                        setEdgeWeight(e_bar, adjwgt[e]);
                }
        }

        finish_construction();
        return 0;
}

/*inline*/ void graph_access::copy(graph_access & G_bar) {
        G_bar.start_construction(number_of_nodes(), number_of_edges());

        basicGraph& ref = *graphref;
        forall_nodes(ref, node) {
                NodeID shadow_node = G_bar.new_node();
                G_bar.setNodeWeight(shadow_node, getNodeWeight(node));
                forall_out_edges(ref, e, node) {
                        NodeID target                   = getEdgeTarget(e);
                        EdgeID shadow_edge              = G_bar.new_edge(shadow_node, target);
                        G_bar.setEdgeWeight(shadow_edge, getEdgeWeight(e));
                }
                endfor
        }
        endfor

        G_bar.finish_construction();
}

