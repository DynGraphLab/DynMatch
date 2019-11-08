/******************************************************************************
 * dyn_graph_access.cpp
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

#include "dyn_graph_access.h"

dyn_graph_access::dyn_graph_access() : m_building_graph(false) {
        edge_count = 0;
        node_count = 0;
}

dyn_graph_access::dyn_graph_access(NodeID n) : m_building_graph(false) {
        edge_count = 0;
        node_count = n;
        start_construction(node_count, edge_count);
        finish_construction();
}

dyn_graph_access::~dyn_graph_access() {};

void dyn_graph_access::start_construction(NodeID nodes, EdgeID edges) {
        m_building_graph = true;
        m_edges.resize(nodes);
        m_edges_maps.resize(nodes);

        for( unsigned i = 0; i < nodes; i++) {
                m_edges_maps[i].set_deleted_key(std::numeric_limits<NodeID>::max());
                m_edges_maps[i].set_empty_key(std::numeric_limits<NodeID>::max()-1);
                m_edges_maps[i].min_load_factor(0.0);
        }
}

NodeID dyn_graph_access::new_node() {
        ASSERT_TRUE(m_building_graph);
        return node_count++;
}

void dyn_graph_access::finish_construction() {
        m_building_graph = false;
        m_edges.resize(node_count);
        m_edges_maps.resize(node_count);
}

void dyn_graph_access::convert_from_graph_access(graph_access& H) {
        dyn_graph_access(H.number_of_nodes());

        forall_nodes(H,n)
                forall_out_edges(H,e,n)
                        new_edge(n,H.getEdgeTarget(e));
                endfor
        endfor

        ASSERT_TRUE(number_of_nodes() == H.number_of_nodes());
        ASSERT_TRUE(number_of_edges() == H.number_of_edges());
}

void dyn_graph_access::convert_to_graph_access(graph_access& H) {
        H.start_construction(number_of_nodes(), number_of_edges());

        for (unsigned int n = 0; n < number_of_nodes(); ++n) {
                H.new_node();

                for (unsigned int e = 0; e < getNodeDegree(n); ++e) {
                        H.new_edge(n, getEdgeTarget(n, e));
                }
        }

        H.finish_construction();

        ASSERT_TRUE(number_of_nodes() == H.number_of_nodes());
        ASSERT_TRUE(number_of_edges() == H.number_of_edges());
}



