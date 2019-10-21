/******************************************************************************
 * definitions.h 
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

#ifndef DEFINITIONS_H_CHR
#define DEFINITIONS_H_CHR

#include <limits>
#include "limits.h"
#include "stdio.h"
#include <vector>
#include <queue>
#include "tools/macros_assertions.h"

using namespace std;
/**********************************************
 * Constants
 * ********************************************/
//Types needed for the graph ds
typedef unsigned int NodeID;
typedef unsigned int EdgeID;
typedef unsigned int PathID;
typedef unsigned int PartitionID;
typedef unsigned int NodeWeight;
typedef int EdgeWeight;
const EdgeID UNDEFINED_EDGE = std::numeric_limits<EdgeID>::max();
const NodeID UNDEFINED_NODE = std::numeric_limits<NodeID>::max();

//for the gpa algorithm
struct edge_source_pair {
        EdgeID e;
        NodeID source;       
};

struct source_target_pair {
        NodeID source;       
        NodeID target;       
};

//matching array has size (no_of_nodes), so for entry in this table we get the matched neighbor
typedef std::vector<NodeID> Matching;

typedef enum {
        PERMUTATION_QUALITY_NONE, PERMUTATION_QUALITY_FAST,  PERMUTATION_QUALITY_GOOD
} PermutationQuality;

typedef unsigned int Count;
#endif

