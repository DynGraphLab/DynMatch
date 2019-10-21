
#ifndef MVGRAPH_H
#define MVGRAPH_H
//~ #include "list.h"
#include "Node.h"
//~ #include "lists.h"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <utility>
#include "graph_simple.h"

#define DDFS_EMPTY 0
#define DDFS_PETAL 1
#define DDFS_PATH 2
#define DDFS_OK 3
#define DDFS_BOTTLENECK 4
#define DDFS_ERROR -1


//needs lists and so can not be defined earlier
struct DDFS_result {
  std::vector<nodeid> nodes_seen;
  nodeid bottleneck;
  DDFS_result():bottleneck(0) { }
};


struct MVGraph{
	std::vector<MVNode> nodes;
	std::vector<nodeid> v_edges; /* note: nodes were either allocated as an array or as the edges vector */
	nodeid* edges;
	bool edges_owned; /* if true, the edges array should be freed in the destructor */
	size_t edges_size;
	std::vector<std::vector<nodeid> > levels;
	std::vector<std::vector<MVBridge> > bridges;
	std::vector<MVEdge> green_stack;
	std::vector<MVEdge> red_stack;
	std::vector<nodeid> path_found;
	DDFS_result last_ddfs;
	unsigned int matchnum;
	int bridgenum;
	int todonum;
	
	//~ int make_symmetric();
	
	//~ int load_graph(int size, std::vector<std::pair<int,int> >& edgelist);
	//~ int load_graph(edgelist<int>& l);
	void greedy_init();
        void init (const std::vector< std::pair< unsigned int, unsigned int > >& matching);
        
        void check ();
        
	MVGraph():edges(0),edges_size(0),matchnum(0),bridgenum(0),todonum(0) { }
	MVGraph(graph& g); /* default mode of construction is to convert a simple graph class */
	~MVGraph() { if(edges && edges_owned) free(edges); }
	void reset();
	
	void add_to_level(unsigned int level, nodeid node);
	void add_to_bridges(unsigned int level, nodeid n1, nodeid n2);
	
	/* main matching functions */
	void max_match();
	bool max_match_phase();
	
	void MIN(unsigned int i);
	bool MAX(unsigned int i);
	
	void step_to(nodeid to, nodeid from, unsigned int level);
	
	/* output matches found */
	void write_matches(FILE* f) const;
	/* same, but replace node IDs using the function supplied by the caller */
	void write_matches(FILE* f, std::function<unsigned int(nodeid)> n) const;
	
	/* DDFS function */
	int DDFS(nodeid green_top, nodeid red_top);
	
	/* path functions */
	
	//TODO expand with blossoms -- ??
	void find_path(nodeid n1,nodeid n2);
	
	//~ void reverse(list_MVNodeP *list,int from, int to);
	//function for the finding of a path, not for walking in a blossom
	void walk_down_path(nodeid start);
	
	nodeid jump_bridge(nodeid cur);
	
	//Walks a blossom from the entry node to the bud.
	//It does not add the bud, it does add the entry node
	nodeid walk_blossom(nodeid cur);
	//walks down until it reacheds the bud of a blossom
	//It does not add the bud, it does add the first node
	nodeid walk_blossom_down(nodeid cur,nodeid bud);
	//Walks up untill it reaches a bridge
	//It does add the bridge and the node it starts on
	nodeid walk_blossom_up(nodeid cur);
	
	void augment_path();
	void remove_path();
	
	
	/* Helper functions used by DDFS */
	void add_pred_to_stack(nodeid cur, std::vector<MVEdge>& stack);
	inline void prepare_next(MVEdge& Nx);
	void step_into(nodeid& C, MVEdge& Nx, std::vector<MVEdge>& S, nodeid green_top, nodeid red_top);
	/* gives the level of the node you would step into */
	inline unsigned int L(const MVEdge& e) const {
		nodeid n = bud_star(e.second);
		return nodes[n].min_level;
	}

	inline nodeid bud_star(nodeid c) const {
		nodeid b = nodes[c].bud;
		if(b == UNSET) return c;
		return bud_star(b);
	}
	inline bool bud_star_includes(nodeid c, nodeid goal) const {
		if(c == goal) return true;
		nodeid b = nodes[c].bud;
		if(b == UNSET) return false;
		return bud_star_includes(b,goal);
	}
	/* helper functions for paths */
	inline bool outer(nodeid n) const { return nodes[n].outer(); }
	inline bool inner(nodeid n) const { return nodes[n].inner(); }
	inline nodeid bud(nodeid n) const { return nodes[n].bud; }
	
	
	inline unsigned int tenacity(nodeid n1, nodeid n2) const {
		if(nodes[n1].match == n2) { /* matched bridge */
			if(nodes[n1].odd_level != UNSET && nodes[n2].odd_level!= UNSET)
				return nodes[n1].odd_level + nodes[n2].odd_level + 1;
		}
		else /* unmatched bridge */
			if(nodes[n1].even_level != UNSET && nodes[n2].even_level!= UNSET)
				return nodes[n1].even_level + nodes[n2].even_level + 1;
		return UNSET;
	}
	
	size_t real_deg(nodeid n);
};



#endif


