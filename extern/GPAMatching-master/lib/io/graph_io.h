/******************************************************************************
 * graph_io.h 
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

#ifndef GRAPHIO_H_
#define GRAPHIO_H_

#include <iostream>
#include <fstream>
#include <ostream>
#include <vector>
#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include "../definitions.h"
#include "../data_structure/static_graph.h"
#include "../data_structure/graph_access.h"

#define MAXLINE	1280000

class graph_io {
        public:
                graph_io();
                virtual ~graph_io () ;

                template<typename graphType> 
                        static int readGraphWeighted(graphType & Graph, std::string filename);

};


template<typename graphType> int graph_io::readGraphWeighted(graphType & Graph, std::string filename) {
        char* line;
        line = new char[MAXLINE+1];
        std::ifstream in(filename.c_str());
        if (!in) {
                std::cerr << "Error opening " << filename << std::endl;
                return 1;
        }

        NodeID nmbNodes;
        EdgeID nmbEdges;

        in.getline(line, MAXLINE);

        int ew;
        sscanf(line, "%d %d %d", &nmbNodes, &nmbEdges, &ew );

        bool read_ew = false;
        bool read_nw = false;

        if(ew == 1) {
                read_ew = true;
        } else if (ew == 11) {
                read_ew = true;
                read_nw = true;
        } else if (ew == 10) {
                read_nw = true;
        }
        nmbEdges *= 2; //since we have forward and backward edges

        Graph.start_construction(nmbNodes, nmbEdges);
        char *oldstr, *newstr;

        for (NodeID i = 0; i < nmbNodes; ++i) {
                NodeID n = Graph.new_node();

                //Graph.node[n].partitionIndex = 0;
                in.getline(line, MAXLINE);

                //******************************************
                // insert the edges given in this line
                // Parse the Line
                // *****************************************
                oldstr = line;
                newstr = NULL;

                // First Number in this Line is the Nodeweight
                if(read_nw) {
                        int weight = (NodeID) strtol(oldstr, &newstr, 10);
                        oldstr = newstr;
                        Graph.nodes[n].weight = weight;
                } else {
                        Graph.nodes[n].weight = 1;
                }

                NodeID source = i;
                NodeID target = 0;
                int edgeWeight = 0;
                for (;;) {
                        target = (NodeID) strtol(oldstr, &newstr, 10);
                        oldstr = newstr;


                        if (target == 0)
                                break;

                        if(read_ew) {
                                edgeWeight = (NodeID) strtol(oldstr, &newstr, 10);
                                oldstr = newstr;
                        }

                        target -= 1; // -1 since there are no nodes with id 0 in the file
                        ASSERT_NEQ(source, target);
                        ASSERT_LEQ(target, nmbNodes);
                        EdgeID e = Graph.new_edge(source, target);
                        if(read_ew)
                        {
                                Graph.edges[e].weight = edgeWeight;
                        } else {

                                Graph.edges[e].weight = 1;
                        }
                }
        }
        //cout <<  "counter " <<  counter  << endl;

        Graph.finish_construction();
        delete[] line;
        return 0;
}

#endif /*GRAPHIO_H_*/
