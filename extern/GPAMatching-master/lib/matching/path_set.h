/******************************************************************************
 * path_set.h 
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

#ifndef PATH_SET_80E9CQT1
#define PATH_SET_80E9CQT1

#include "../data_structure/graph_access.h"
#include "../config.h"
#include "../tools/macros_assertions.h"
#include "path.h"

class path_set {
        public:

                path_set( graph_access * G, const Config * config );
                virtual ~path_set();

                //returns the path that v lies on iff v is an endpoint
                const path& get_path(const NodeID & v) const;

                //returns the number of paths in the set
                PathID path_count() const;

                // add the edge with given id to the path set if it is applicable
                // returns true iff the edge was applicable
                bool add_if_applicable(const NodeID & source, const EdgeID & e); 

                //**********
                //Navigation
                //**********

                //returns the if of vertex next to v on the path
                NodeID next_vertex( const NodeID & v ) const;       

                //returns the if of vertex previous to v on the path
                NodeID prev_vertex( const NodeID & v ) const;      

                //returns the id of the edge to the next vertex on the path
                EdgeID edge_to_next(const NodeID & v) const;

                //returns the id of the edge to the previous vertex on the path
                EdgeID edge_to_prev(const NodeID & v) const;
        private:
                graph_access * pG;

                const Config * config;


                // Number of Paths
                PathID no_of_paths;

                // for every vertex v, vertex_to_path[v] is the id of the path
                std::vector<PathID> vertex_to_path;

                // for every path id p, paths[p] is the path for this id
                std::vector<path> paths;

                // for every vertex v, next[v] is the id of the vertex that is next on its path.
                // for the head v of a path, next[v] == v
                std::vector<NodeID> next;


                // for every vertex v, prev[v] is the id of the vertex that is previouson its path.
                // for the tail v of a path, prev[v] == v
                std::vector<NodeID> prev;

                // for every vertex v, next_edge[v] is the id of the vertex that is used to
                // connect the vertex v to the next vertex in the path.
                // if next[v] == v the next_edge[v] = UNDEFINED_EDGE
                std::vector<EdgeID> next_edge;

                // for every vertex v, prev_edge[v] is the id of the vertex that is used to
                // connect the vertex v to the previous vertex in the path.
                // if prev[v] == v the prev_edge[v] = UNDEFINED_EDGE
                std::vector<EdgeID> prev_edge;

                inline bool is_endpoint(const NodeID & v) const {
                        return (next[v] == v or prev[v] == v);
                } 
};


inline const path& path_set::get_path(const NodeID & v) const {
        PathID path_id = vertex_to_path[v];
        ASSERT_TRUE(path_id < vertex_to_path.size());
        return paths[path_id];
}

inline PathID path_set::path_count() const {
        return no_of_paths;
}

inline NodeID path_set::next_vertex( const NodeID & v ) const {
        return next[v];
}       

inline NodeID path_set::prev_vertex( const NodeID & v ) const {
        return prev[v];
}      

inline EdgeID path_set::edge_to_next(const NodeID & v) const {
        return next_edge[v];
}

inline EdgeID path_set::edge_to_prev(const NodeID & v) const {
        return prev_edge[v];
}

inline bool path_set::add_if_applicable(const NodeID & source, const EdgeID & e) {
        graph_access & G = *pG;

        NodeID target = G.getEdgeTarget(e);

        PathID sourcePathID = vertex_to_path[source]; 
        PathID targetPathID = vertex_to_path[target]; 

        ASSERT_NEQ(source, target);

        path & source_path = paths[sourcePathID];
        path & target_path = paths[targetPathID];

        if(not is_endpoint(source) or not is_endpoint(target)) {
                // both vertices must be endpoints. otherwise, the edge is not applicable
                return false;
        }


        ASSERT_TRUE(source_path.is_active());
        ASSERT_TRUE(target_path.is_active());
        
         if(source_path.is_cycle() or target_path.is_cycle()) {
                // if one of the paths is a cycle then it is not applicable 
                return false;
        }

        if(sourcePathID != targetPathID) {
                // then we wont close a cycle, and we will join the paths
                // else case handles cycles
                no_of_paths--;
                source_path.set_length(source_path.get_length() + target_path.get_length() + 1);

                // first we update the path data structure
                // handle four cases / see implementation details for a picture
                if(source_path.get_head() == source && target_path.get_head() == target) {
                        vertex_to_path[target_path.get_tail()] = sourcePathID;
                        source_path.set_head(target_path.get_tail()); 
                } else if(source_path.get_head() == source && target_path.get_tail() == target) {
                        vertex_to_path[target_path.get_head()] = sourcePathID;                       
                        source_path.set_head(target_path.get_head()); 
                } else if(source_path.get_tail() == source && target_path.get_head() == target) {
                        vertex_to_path[target_path.get_tail()] = sourcePathID;
                        source_path.set_tail(target_path.get_tail()); 
                } else if(source_path.get_tail() == source && target_path.get_tail() == target) {
                        vertex_to_path[target_path.get_head()] = sourcePathID;                       
                        source_path.set_tail(target_path.get_head()); 
                }

                //update the double linked list so that we can navigate through the paths later
                //first handle the source node
                if(next[source] == source) {
                        ASSERT_TRUE(edge_to_next(source) == UNDEFINED_EDGE);
                        next[source]      = target;
                        next_edge[source] = e;
                } else {
                        ASSERT_TRUE(edge_to_prev(source) == UNDEFINED_EDGE);
                        prev[source]      = target;
                        prev_edge[source] = e;
                }

                //then handle the target node
                if(next[target] == target) {
                        ASSERT_TRUE(edge_to_next(target) == UNDEFINED_EDGE);
                        next[target]      = source;
                        next_edge[target] = e;
                } else {
                        ASSERT_TRUE(edge_to_prev(target) == UNDEFINED_EDGE);
                        prev[target]      = source;
                        prev_edge[target] = e;
                }

                target_path.set_active(false);

                return true;
        } else if(sourcePathID == targetPathID && source_path.get_length() % 2 == 1) {

                //first we update the path data structure
                source_path.set_length(source_path.get_length()+1);
                
                //close the cycle by updateing the doubly linked list
                if(next[source_path.get_head()] == source_path.get_head()) {
                        next[source_path.get_head()] = source_path.get_tail();
                        next_edge[source_path.get_head()] = e; 
                } else {
                        prev[source_path.get_head()] = source_path.get_tail();
                        prev_edge[source_path.get_head()] = e; 
                }

                if(next[source_path.get_tail()] == source_path.get_tail()) {
                        next[source_path.get_tail()] = source_path.get_head();
                        next_edge[source_path.get_tail()] = e; 
                } else {
                        prev[source_path.get_tail()] = source_path.get_head();
                        prev_edge[source_path.get_tail()] = e; 
                }

                source_path.set_tail(source_path.get_head());
                return true;
        }       
        return false;
} 


#endif /* end of include guard: PATH_SET_80E9CQT1 */
