
#include "Graph.h"

/* This will add all predecesors of a nodes bud_star to the stack. */
inline void MVGraph::add_pred_to_stack(nodeid cur, std::vector<MVEdge>& stack) {
	for(nodeid pred : nodes[cur].preds) if(pred != UNSET) {
		stack.push_back(MVEdge(cur,pred));
	}
}

/*
This sets the below pointer of the current node to the next one and calculates the bud_star.
It does not yet step, as we might need to backtrack of have found  a petal
*/
inline void MVGraph::prepare_next(MVEdge& Nx) {
	if(Nx.first != UNSET) {
		nodes[Nx.first].below = Nx.second;
	}						
    Nx.second = bud_star(Nx.second);			
}

inline bool edge_valid(const MVEdge& e) {
	if(e.first == UNSET && e.second == UNSET) return false;
	return true;
}

inline void node_from_stack(MVEdge& e, std::vector<MVEdge>& S) {
	if(S.size() > 0) {
		e = S.back();
		S.pop_back();
	}
	else e = MVEdge(UNSET,UNSET);
}

/*
  Steps to the next node
 */
void MVGraph::step_into(nodeid& C, MVEdge& Nx, std::vector<MVEdge>& S, nodeid green_top, nodeid red_top) {
	prepare_next(Nx); /* Set below pointer and calculate bud_star */
	if(nodes[Nx.second].visited == false) {
		nodes[Nx.second].above = Nx.first;
		C = Nx.second; /* Change center of DFS */
		MVNode& n = nodes[C];
		n.visited = true;
		n.ddfs_green = green_top;
		n.ddfs_red = red_top;
		last_ddfs.nodes_seen.push_back(C);
		add_pred_to_stack(C,S); /* Add all unseen predecesors */
    }
    node_from_stack(Nx,S);
}


int MVGraph::DDFS(nodeid green_top, nodeid red_top){

	nodeid reset_current;
	/* reset result */
	last_ddfs.nodes_seen.clear();
	last_ddfs.bottleneck = UNSET;
	
	std::vector<MVEdge>& Sr = red_stack; /* short names */
	std::vector<MVEdge>& Sg = green_stack;
	/* reset stacks */
	Sr.clear();
	Sg.clear();
	//Start at the bridges, no center yet
	nodeid G = UNSET;
	nodeid R = UNSET;
	
	if(bud_star(red_top) == bud_star(green_top)) return DDFS_EMPTY; //avoid the whole loop thing
	else if (nodes[green_top].min_level == 0 && nodes[red_top].min_level == 0)
		return DDFS_PATH; //return imidetly for one edge paths

	
	//there is no center yet
	//second is where we want to go 
	MVEdge Ng = MVEdge(UNSET,green_top);
	MVEdge Nr = MVEdge(UNSET,red_top);
	
	//used to keep track later if we backtrack.
	//if a backtrack fails then we need to jump back to the DDFS collision that started it
	MVEdge red_before = MVEdge(UNSET,UNSET);
	MVEdge green_before = MVEdge(UNSET,UNSET);
	while( R==UNSET || G==UNSET || //we are starting
			nodes[R].min_level > 0 || nodes[G].min_level>0 ) {    //we need some walking still



		while(edge_valid(Nr) && edge_valid(Ng) && L(Nr) != L(Ng)) {
			while(edge_valid(Nr) && L(Nr) > L(Ng)){ //quit if we have no stack left
				step_into(R,Nr,Sr,green_top,red_top);
			}
			
			if(!edge_valid(Nr)) { //backtracked till end of stack, jump back to lowest bottleneck
				if(!edge_valid(red_before)) {
					fprintf(stderr,"DDFS(): red_before invalid when used!\n");
				}
				Nr = red_before;
				nodeid tmp_itt = red_before.first;
				while(nodes[tmp_itt].above != UNSET) {
					reset_current = nodes[tmp_itt].above;
					for(const nodeid& reset_itt : nodes[reset_current].preds) {
						if(reset_itt == UNSET) continue; //??
						if(bud_star(reset_itt) == tmp_itt){
							nodes[reset_current].below = reset_itt;
							break;
						}
					}
					tmp_itt = nodes[tmp_itt].above;
				}
			}
			while(edge_valid(Ng) && L(Nr) < L(Ng)){
				step_into(G,Ng,Sg,green_top,red_top);
			}
			
			if(!edge_valid(Ng)){ //backtracked till end of stack, jump back tpo lowest bottleneck
				Ng = green_before;
				nodeid tmp_itt = green_before.first;
				while(nodes[tmp_itt].above != UNSET) {
					reset_current = nodes[tmp_itt].above;
					for(const nodeid& reset_itt : nodes[reset_current].preds) {
						if(reset_itt == UNSET) continue; //??
						if(bud_star(reset_itt) == tmp_itt){
							nodes[reset_current].below = reset_itt;
							break;
						}
					}
					tmp_itt = nodes[tmp_itt].above;
				}
			}
		}
		
		
		
		if(bud_star(Nr.second) == bud_star(Ng.second)){
			//backtrack
			if(Sr.size() > 0){
				red_before = Nr;
				prepare_next(Nr);
				node_from_stack(Nr,Sr);
				if(edge_valid(Nr)) R = Nr.first;
				else Nr = red_before;
			}
			else if(Sg.size() > 0) {
				green_before = Ng;
				prepare_next(Ng);
				node_from_stack(Ng,Sg);
				if(edge_valid(Ng)) G = Ng.first;
				else Ng = green_before;
			}
			else {
				prepare_next(Nr);
				prepare_next(Ng);
				last_ddfs.bottleneck = Nr.second;
				return DDFS_PETAL;
			}
		}
		else {
			//now Nr!=Ng
			step_into(R,Nr,Sr,green_top,red_top);
			step_into(G,Ng,Sg,green_top,red_top);
		}

	}
	return DDFS_PATH;
}


