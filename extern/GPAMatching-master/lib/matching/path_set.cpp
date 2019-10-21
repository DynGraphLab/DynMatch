/******************************************************************************
 * path_set.cpp 
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

#include "path_set.h"

path_set::path_set( graph_access * G_, const Config * config_ ): pG(G_), config(config_),
                                         no_of_paths(pG->number_of_nodes()), 
                                         vertex_to_path(no_of_paths),
                                         paths(no_of_paths),
                                         next(no_of_paths), 
                                         prev(no_of_paths),
                                         next_edge(no_of_paths, UNDEFINED_EDGE),
                                         prev_edge(no_of_paths, UNDEFINED_EDGE) {
       
       graph_access & G = *pG;        
       forall_nodes(G, n) {
               vertex_to_path[n] = n;
               paths[n].init(n);
               next[n] = n;
               prev[n] = n;
       } endfor
       
}

path_set::~path_set() {
                
}

