/******************************************************************************
 * gpa_matching.cpp
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

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string.h> 

#include <argtable2.h>
#include <regex.h>

#include "../lib/config.h"
#include "../lib/data_structure/graph_access.h"
#include "../lib/io/graph_io.h"
#include "../lib/matching/gpa_matching.h"
#include "../lib/tools/timer.h"
#include "../lib/tools/macros_assertions.h"
#include "../lib/tools/random_functions.h"

#include "parse_parameters.h"
#include <math.h>

using namespace std;
int main(int argn, char **argv) {

        Config config;
        string graph_filename;

        int ret_code = parse_parameters(argn, argv, config); 

        if(ret_code) {
                return 0;
        }

        basicGraph realGraph;

        skappa_timer t;
        graph_io::readGraphWeighted(realGraph, config.graph_filename);
       
        graph_access G(&realGraph);     
 
        srand(config.seed);
        random_functions::setSeed(config.seed);

        cout <<  "graph has " <<  G.number_of_nodes() <<  " nodes and " <<  G.number_of_edges() <<  " edges"  << endl;
        // ***************************** perform partitioning ***************************************       

        t.restart();
        
        gpa_matching match;
        Matching matching;
        match.match(config, G, matching);
        
        cout <<  "time elapsed (gpa matching algorithm):  " <<  t.elapsed()  << endl;
        double accumulated_weight = 0.0;
        int cardinality = 0;

        forall_nodes(G, node) {
                if(matching[node] != node) {
                        cardinality++;
                        NodeID target = matching[node];
                        forall_out_edges(G, e, node) {
                                NodeID _target = G.getEdgeTarget(e);
                                if(target == _target) {
                                        accumulated_weight += G.getEdgeWeight(e);
                                        break;
                                }
                        } endfor
                } 
        } endfor
        cout <<  "number of matched nodes: " <<  cardinality  << endl;
        cout <<  "accumulated weight (undirected matching weight) : " <<  fixed <<  accumulated_weight/2 << endl;

        string outputfilename("matching");
        ofstream f(outputfilename.c_str());
        forall_nodes(G, node) {
                if(matching[node] != node) {
                        NodeID target = matching[node];
                        if( node < target ) 
                                f << node << " " << target << endl;
                        } 
        } endfor

        f.close();
}
