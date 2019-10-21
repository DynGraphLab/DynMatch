#include <vector>
#include <utility> /* for std::pair */
#include <stdint.h>
#include <stddef.h> /* for size_t */

/* auxiliary typedefs */
/* Used to store bridges is found. */
typedef std::pair<NodeID,NodeID> MVBridge;

/* Used for the DDFS, otherwise edges are not stored in there own datastructure. */
typedef std::pair<NodeID,NodeID> MVEdge;

/* Used to make a list of PredecessorTo relations.
 * These require the node you are predecessor to but also the possition in its list, for quick removal. */
typedef std::pair<NodeID,uint32_t> MVNodePos;

/* Some trivial constants. */
const static NodeID UNSET = std::numeric_limits<NodeID>::max()-1;
//~ #define oo 1<<30
const static NodeID UNMATCHED = std::numeric_limits<NodeID>::max();

struct MVNode{
	std::vector<NodeID> preds;
	std::vector<MVNodePos> pred_to;
	std::vector<NodeID> hanging_bridges;
	/* do not store a pointer to the edges, only an index into the edges array stored in the MVGraph struct */
	size_t edges_idx;
	unsigned int degree;
	unsigned int min_level;
	unsigned int max_level;
	unsigned int odd_level;
	unsigned int even_level;
	NodeID match;
	NodeID bud;
	NodeID above;
	NodeID below;
	NodeID ddfs_green;
	NodeID ddfs_red;
	int number_preds;
	bool deleted;
	bool visited;
	
	MVNode():edges_idx(0),degree(0),match(UNMATCHED) {
		set_defaults();
	}
	
	inline void set_min_level(unsigned int level) {
		min_level = level;
		if(level%2) odd_level = level;
		else even_level = level;
	}
	inline void set_max_level(unsigned int level) {
		max_level = level;
		if(level%2) odd_level = level;
		else even_level = level;
	}
	inline bool outer() const { return even_level < odd_level; }
	inline bool inner() const { return even_level >= odd_level; }
	
	void set_defaults() {
		min_level = UNSET;
		max_level = UNSET;
		odd_level = UNSET;
		even_level = UNSET;
		bud = UNSET;
		above  = UNSET;
		below  = UNSET;
		ddfs_green  = UNSET;
		ddfs_red  = UNSET;
		deleted = false;
		number_preds = 0;
		visited = false;
	}
	void reset() {
		preds.clear();
		pred_to.clear();
		hanging_bridges.clear();
		set_defaults();
	}
};

inline void set_min_level(MVNode& n, unsigned int level) { n.set_min_level(level); }
inline void set_max_level(MVNode& n, unsigned int level) { n.set_max_level(level); }
inline bool outer(const MVNode& n) { return n.outer(); }
inline bool inner(const MVNode& n) { return n.inner(); }
inline NodeID bud(const MVNode& n) { return n.bud; }

//~ MVNodeP bud_star(MVNodeP c);
//~ int bud_star_includes(MVNodeP c,MVNodeP goal);

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

