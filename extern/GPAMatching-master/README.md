GPAMatching
===========

This is an implementation of the global paths matching algorithm.

The Global Paths Algorithm (GPA), was proposed by Maue and Sanders in "Engineering Algorithms for Approximate Weighted Matching" (WEA'07) as a synthesis of Greedy and Path Growing algorithms by Drake et. al.
The greedy algorithm sorts the edges by descending weight (or rating) and then scans them. 
If an edge \{u,v\} and its end points are not matched yet, it is put into the matching. 

Similar to the Greedy approach, GPA scans the edges in order of decreasing weight (or rating);
but rather than immediately building a matching, it first constructs a collection of paths and even length cycles. To be more precise, these paths initially contain no edges.
While scanning the edges, the set is extended by successively adding applicable edges. 
An edge is called applicable if it connects two endpoints of different paths or the two endpoints of an odd length path. 
Afterwards, optimal solutions/matchings are computed for each of these paths and cycles using dynamic programming. 
Both algorithms achieve a half-approximation in the worst case, but empirically, GPA gives considerably better results. 

The program is licenced under GPL 3.0. 
If you publish results using our algorithms, please acknowledge our work by quoting the following paper:

@inproceedings{kaffpa,
        author    = {P. Sanders and C. Schulz},
        title     = {{Engineering Multilevel Graph Partitioning Algorithms}},
        booktitle = {Proceedings of the 19th European Symposium on Algorithms},
        year      = {2011},
        pages     = {469--480},
        publisher = {Springer},
        series    = {LNCS},
        volume    = {6942},
        isbn      = {978-3-642-23718-8}
}

Graph Format
===========

The graph format used is the same as used by Metis, Chaco and the graph format that has been used during the 10th DIMACS Implementation Challenge on Graph Clustering and Partitioning. 
The input graph has to be undirected, without self-loops and without parallel edges.

To give a description of the graph format, we follow the description of the Metis 4.0 user guide very closely. A graph G=(V,E) with n vertices and m edges is stored in a plain text file that contains n+1 lines (excluding comment lines). The first line contains information about the size and the type of the graph, while the remaining n lines contain information for each vertex of G. Any line that starts with \% is a comment line and is skipped.

The first line in the file contains either two integers, n m, or three integers, n m f. The first two integers are the number of vertices n and the number of undirected edges of the graph, respectively. Note that in determining the number of edges m, an edge between any pair of vertices v and u is counted _only once_ and not twice, i.e. we do not count the edge (v,u) from (u,v) separately. The third integer f is used to specify whether or not the graph has weights associated with its vertices, its edges or both. If the graph is unweighted then this parameter can be omitted. It should be set to 1 if the graph has edge weights, 10 if the graph has node weights and 11 if the graph has edge and node weights.

The remaining n lines of the file store information about the actual structure of the graph. In particular, the ith line (again excluding comment lines) contains information about the ith vertex. Depending on the value of f, the information stored in each line is somewhat different. In the most general form (when f=11, i.e. we have node and edge weights) each line has the following structure:

       c, v_1, w_1, v_2, w_2 .. v_k, w_k 

where c is the vertex weight associated with this vertex, v_1, ...,  v_k are the vertices adjacent to this vertex, and w_1, ..., w_k are the weights of the edges. Note that the vertices are numbered starting from 1 (not from 0). Furthermore, the vertex-weights must be integers greater or equal to 0, whereas the edge-weights must be strictly greater than 0.

Output Format
===========
After the algorithms computed a matching the output matching of the graph will be written in a file called _matching_.
Each line of the file contains a matching edge (node ids start from 0).
