#ifndef NODE_H
#define NODE_H
#include <vector>
#include <utility> /* for std::pair */
#include <stdint.h>
#include <stddef.h> /* for size_t */

typedef uint32_t nodeid; /* use 32-bit unsigned integers for node IDs */

/* auxiliary typedefs */
/* Used to store bridges is found. */
typedef std::pair<nodeid,nodeid> MVBridge;

/* Used for the DDFS, otherwise edges are not stored in there own datastructure. */
typedef std::pair<nodeid,nodeid> MVEdge;

/* Used to make a list of PredecessorTo relations.
 * These require the node you are predecessor to but also the possition in its list, for quick removal. */
typedef std::pair<nodeid,uint32_t> MVNodePos;

/* Some trivial constants. */
const static nodeid UNSET = 0xffffffffu;
//~ #define oo 1<<30
const static nodeid UNMATCHED = 0xfffffffeu;

struct MVNode{
	std::vector<nodeid> preds;
	std::vector<MVNodePos> pred_to;
	std::vector<nodeid> hanging_bridges;
	/* do not store a pointer to the edges, only an index into the edges array stored in the MVGraph struct */
	size_t edges_idx;
	unsigned int degree;
	unsigned int min_level;
	unsigned int max_level;
	unsigned int odd_level;
	unsigned int even_level;
	nodeid match;
	nodeid bud;
	nodeid above;
	nodeid below;
	nodeid ddfs_green;
	nodeid ddfs_red;
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
inline nodeid bud(const MVNode& n) { return n.bud; }

//~ MVNodeP bud_star(MVNodeP c);
//~ int bud_star_includes(MVNodeP c,MVNodeP goal);


#endif

