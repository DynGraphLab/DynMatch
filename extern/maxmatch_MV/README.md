# maxmatch_MV
Micali Vazirani Maximum Cardinality Matching Algorithm for symmetric (undirected) graphs.
Adapted from https://github.com/jorants/MV-Matching-V2

Adapted to C++; the main motivation was to use less memory to allow significantly
larger graphs to process (>1 billion edges and a few million nodes).


### Usage:

compiling with g++:

g++ -o mmmv *.cpp -O3 -march=native -std=gnu++14

running:

./mmmv < graph_edges.dat > maxmatch_edges.dat


Note: the graph is expected as a list of edges, which are assumed to be undirected.
If the input is already sorted / partitioned (i.e. all appearances of a node in the
first column are grouped together), the -p switch can be used to halve the memory
usage while reading the graph (from 8 bytes / edge to 4 bytes / edge).


### Changes:

Main changes from the original version:
- Nodes are referenced by IDs (i.e. 32-bit indices to a node array) instead of
pointers (which can be 64-bit on typical systems); this limits the size of the
graph to < 2^32-2 nodes, but halves the memory needed to store the edges. Note
that the number of edges is not limited by this, i.e. there can be > 2^32 edges,
but individual node degrees have to be < 2^32 as well.
- All edges are stored in a flat array instead of separate vectors for each
node. This way, memory is used more efficiently.
- C++ std::vector is used for all dynamic arrays with the exception of the edge
array (the main reason for that is to not require doubling the size while
reading). This simplifies memory management.
- All macros are replaced by inline functions, making the code more readable
(in my opinion :) ).
- Efficient functions for reading the graph, focusing on limiting excess
memory usage. Reading an already sorted graph as an egde list requires no extra
storage, while reading edges in arbitrary order requires only the minimal,
4 bytes / edge extra storage corresponding to having to store all edges first
to sort them.


