/******************************************************************************
 * definitions.h 
 *
 * Christian Schulz <christian.schulz.phone@gmail.com>
 *****************************************************************************/

#ifndef DEFINITIONS_H_CHR_MATCH
#define DEFINITIONS_H_CHR_MATCH

#include <limits>
#include <queue>
#include <vector>

#include "limits.h"
#include "macros_assertions.h"
#include "stdio.h"


// allows us to disable most of the output during partitioning
#ifdef MATCHOUTPUT
        #define PRINT(x) x
#else
        #define PRINT(x) do {} while (false);
#endif

/**********************************************
 * Constants
 * ********************************************/
//Types needed for the graph ds
typedef unsigned int 	NodeID;
typedef double 		EdgeRatingType;
typedef unsigned int 	PathID;
typedef unsigned int 	PartitionID;
typedef unsigned int 	NodeWeight;
typedef int 		EdgeWeight;
typedef EdgeWeight 	Gain;
#ifdef MODE64BITEDGES
typedef uint64_t 	EdgeID;
#else
typedef unsigned int 	EdgeID;
#endif
typedef int 		Color;
typedef unsigned int 	Count;
typedef std::vector<NodeID> boundary_starting_nodes;
typedef long FlowType;

const EdgeID UNDEFINED_EDGE            = std::numeric_limits<EdgeID>::max();
const NodeID UNDEFINED_NODE            = std::numeric_limits<NodeID>::max();
const NodeID UNASSIGNED                = std::numeric_limits<NodeID>::max();
const NodeID ASSIGNED                  = std::numeric_limits<NodeID>::max()-1;
const PartitionID INVALID_PARTITION    = std::numeric_limits<PartitionID>::max();
const PartitionID BOUNDARY_STRIPE_NODE = std::numeric_limits<PartitionID>::max();
const int NOTINQUEUE 		       = std::numeric_limits<int>::max();
const Count UNDEFINED_COUNT            = std::numeric_limits<Count>::max();
const NodeID NOMATE                    = std::numeric_limits<NodeID>::max();
const int ROOT 			       = 0;
const NodeID EVEN                      = 0;
const NodeID ODD                       = 1;
const NodeID UNLABELED                 = 2;


//struct DynNode {
    //EdgeID firstEdge;
//};

struct DynEdge {
    NodeID target;
};

struct Node {
    EdgeID firstEdge;
    NodeWeight weight;
};

struct Edge {
    NodeID target;
    EdgeWeight weight;
};


typedef std::vector<NodeID> Matching;

//AlgorithmType
typedef enum {
        RANDOM_WALK, 
        BASWANA_GUPTA_SENG, 
 	NEIMAN_SOLOMON, 
        NAIVE,
 	MV, 
 	DYNBLOSSOM, 
 	DYNBLOSSOMNAIVE, 
        BOOSTBLOSSOM,
        BLOSSOM	
} AlgorithmType;

typedef enum {
        BLOSSOMEMPTY,
        BLOSSOMGREEDY,
        BLOSSOMEXTRAGREEDY
} BlossomInitType;

typedef enum {
        PERMUTATION_QUALITY_NONE, 
	PERMUTATION_QUALITY_FAST,  
	PERMUTATION_QUALITY_GOOD
} PermutationQuality;


#endif

