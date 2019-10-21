/*
 * graph.cpp -- simple graph implementation storing it as a list of edges
 * 	includes implementation for finding connected components of a symmetric graph
 * 	and an implementation of finding a maximum matching for a bipartite graph
 * 		or DAG using the Hopcroft-Karp algorithm
 * 
 * Copyright 2018 Daniel Kondor <kondor.dani@gmail.com>
 * 
 * Hopcroft-Karp algorithm adapted from
 * http://www.geeksforgeeks.org/hopcroft-karp-algorithm-for-maximum-matching-set-2-implementation/
 * (no license provided for it)
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of the  nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */



#include "graph_simple.h"

/* 3-4. common interface for creating the edges after copying the out-edges to the edges array and using a separate in-edges array */
int graph::create_graph(unsigned int* in_edges, std::unordered_map<unsigned int,unsigned int>* ids_map) {
	/* note: clear has been called before at this point and out-edges are stored in the edges array */
	/* 1. sort edges using zip iterators */
	zi::zip_it<unsigned int*,unsigned int*> e = zi::make_zip_it(in_edges,edges);
	zi::zip_it<unsigned int*,unsigned int*> end = zi::make_zip_it(in_edges+nedges,edges+nedges);
	std::sort(e,end,[](const auto& a, const auto& b) {
			return a.first < b.first;
		});
	/* 2. call the general interface creating the node objects */
	return create_graph_sorted(e,end,ids_map,false);
}
		
		
/* 4. create graph of edges supplied as two C-style arrays
 * edges point as e1[i] -> e2[i]
 * the e2 array is "taken over" by this instance, it should not be freed by the caller later
 * both arrays are modified by sorting them */
int graph::create_graph_arrays(unsigned int* e1, unsigned int* e2, size_t e_size, std::unordered_map<unsigned int, unsigned int>* ids_map) {
	clear();
	if( ! (e1 && e2 && e_size) ) return 1;
	edges = e2;
	nedges = e_size;
	edges_size = e_size;
	return create_graph(e1,ids_map);
}
/* 4. create graph of edges supplied as two std::vector<unsiged int>
 * edges point as e1[i] -> e2[i]
 * the e2 vector is "taken over", i.e. it is swapped by a local vector */
int graph::create_graph_vectors(std::vector<unsigned int>& e1, std::vector<unsigned int>& e2, std::unordered_map<unsigned int, unsigned int>* ids_map) {
	clear();
	if(e1.size() != e2.size() || e1.size() == 0) return 0;
	edges_vect.swap(e2);
	edges_owned = false;
	nedges = edges_vect.size();
	edges_size = edges_vect.capacity();
	edges = edges_vect.data();
	return create_graph(e1.data(),ids_map);
}

/* Helper function for the above to handle reading the graph from a file.
 * If partitioned == true, it expects already partitioned input; otherwise
 * it sorts the input.
 * If ids != null, it replaces ids in the edges to form a continuous
 * range 0...N-1, and the real ids from the file are stored in the supplied
 * vector */
int graph::read_graph(FILE* f, bool partitioned, std::vector<unsigned int>* ids) {
	if(!f) return 1;
	if(ids) ids->clear();
	std::unordered_map<unsigned int,unsigned int> ids_map;
	std::unordered_map<unsigned int,unsigned int>* p_ids_map = 0;
	if(ids) p_ids_map = &ids_map;
	
	tsv_iterator itf(f,0,0,true);
	
	int ret;
	if(partitioned) ret = create_graph_partitioned(itf,tsv_iterator_sentinel(),p_ids_map);
	else ret = create_graph_copy_sort(itf,tsv_iterator_sentinel(),p_ids_map);
	if(ret || itf.is_error()) {/* check and signal parse errors */
		clear();
		fprintf(stderr,"graph::read_graph(): error reading the graph!\n");
		return 1;
	}
	
	if(ids) {
		ids->resize(nnodes,0);
		for(auto it = ids_map.begin();it != ids_map.end();++it) (*ids)[it->second] = it->first;
	}
	return 0;
}


unsigned int graph::real_deg(unsigned int n) const {
	uint64_t deg;
	if(n+1 < nnodes) deg = idx[n+1] - idx[n];
	else deg = nedges - idx[n];
	return (unsigned int)deg; /* note: assuming degrees are always < 2^32 */
}

/* make the graph symmetric by simply growing the edges structure */
int graph::make_symmetric() {
	/* 1. check if the graph is symmetric, increase node degrees */
	size_t extra_size = 0;
	for(unsigned int i=0;i<nnodes;i++) {
		/* note: degrees should not be relied during running, they are being updated
		 * calculate the current real degree */
		unsigned int deg = real_deg(i);
		for(unsigned int k=0;k<deg;k++) {
			unsigned int j = edges[idx[i] + k]; /* i -- j edge */
			/* check if there is a j -- i edge as well */
			unsigned int degj = real_deg(j);
			unsigned int* it = std::lower_bound(edges + idx[j], edges + idx[j] + degj, i);
			if(it >= edges + idx[j] + degj || *it != i) {
				/* not found, will need to add j -- i edge
				 * at this point only increase the degree of j */
				outdeg[j]++;
				extra_size++;
			}
		}
	}
	
	if(extra_size == 0) return 0; /* no extra edges need to be added */
	/* 2. realloc the edges array, alloc temporary space for old node degrees */
	size_t old_size = nedges;
	if(edges_owned) {
		if(nedges + extra_size > edges_size) {
			unsigned int* tmp1 = (unsigned int*)realloc(edges,(nedges + extra_size)*sizeof(unsigned int));
			if(!tmp1) return 1;
			edges = tmp1;
			nedges += extra_size;
			edges_size = nedges;
		}
		else nedges += extra_size;
	}
	else {
		edges_vect.resize(nedges + extra_size);
		edges = edges_vect.data();
		nedges += extra_size;
		edges_size = nedges;
	}
	
	
	
	unsigned int* tmp = (unsigned int*)malloc(sizeof(unsigned int)*nnodes);
	if(!tmp) return 1;
	
	/* 3. move all edges according to the new degrees,
	 * store old degrees to be able to add the new edges
	 * need to iterate in reverse
	 * 	-- note: i needs to be signed for the following loop to work! */
	size_t newidx = nedges;
	for(unsigned int i1 = nnodes; i1 > 0; i1--) {
		unsigned int i = i1-1;
		/* calculate the old degree of i */
		unsigned int deg = old_size - idx[i]; /* again, assuming that all degrees < 2^32 */
		tmp[i] = deg;
		if(outdeg[i] > newidx) {
			fprintf(stderr,"graph::make_symmetric(): inconsistency found while adding edges!\n");
			free(tmp);
			return 1;
		}
		newidx -= outdeg[i]; /* new start for edges */
		if(newidx != idx[i]) for(unsigned int j=deg;j>0;j--) edges[newidx+j-1] = edges[idx[i]+j-1];
		outdeg[i] = deg; /* reset the old degree so we know how to grow */
		old_size = idx[i];
		idx[i] = newidx; /* save the new start of edges */
	}
	
	/* 4. add missing edges */
	size_t edges_added = 0;
	for(unsigned int i = 0; i < nnodes; i++) {
		unsigned int deg = tmp[i]; /* degrees saved previously */
		for(unsigned int k = 0; k < deg; k++) {
			unsigned int j = edges[idx[i] + k];
			size_t degj = tmp[j];
			unsigned int* it = std::lower_bound(edges + idx[j], edges + idx[j] + degj, i);
			if(it >= edges + idx[j] + degj || *it != i) {
				/* add j -- i edge */
				unsigned int max_deg = real_deg(j);
				if(outdeg[j] >= max_deg) {
					fprintf(stderr,"graph::make_symmetric(): inconsistency found while adding edges!\n");
					free(tmp);
					return 1;
				}
				edges[idx[j] + outdeg[j]] = i;
				outdeg[j]++;
				edges_added++;
			}
		}
	}
	
	free(tmp);
	if(edges_added != extra_size) {
		fprintf(stderr,"graph::make_symmetric(): inconsistency found while adding edges!\n");
		return 1;
	}
	return 0;
}


