#include "Graph.h"
#include <algorithm>
#include <stdexcept>
/*
static void handle_error(const char* err) {
	fprintf(stderr,"%s",err);
	throw new std::runtime_error(err);
}
*/
void MVGraph::find_path(nodeid n1, nodeid n2) {
	//~ debug("find path: %i %i\n",n1->N,n2->N);
	path_found.clear();
	walk_down_path(n1);
	std::reverse(path_found.begin(), path_found.end());  
	walk_down_path(n2);    
}


//function for the finding of a path, not for walking in a blossom
void MVGraph::walk_down_path(nodeid start) {
	//~ debug("Walk down path: %i\n",start->N);  
	nodeid cur = start;
	while(cur != UNSET){
		if(bud(cur) != UNSET) cur = walk_blossom(cur); //Step into a blossom
		else {
			path_found.push_back(cur);
			cur = nodes[cur].below;
		}
	}
}


inline nodeid MVGraph::jump_bridge(nodeid cur) {
	if(nodes[cur].ddfs_green == cur) return nodes[cur].ddfs_red;
	else if(nodes[cur].ddfs_red == cur) return nodes[cur].ddfs_green;
	else if(bud_star_includes(nodes[cur].ddfs_green,cur)) { //have to jump to get to the bridge
		size_t before = path_found.size();
		nodeid bud = nodes[cur].ddfs_green;
		while(bud != cur) bud = walk_blossom(bud);
		std::reverse(path_found.begin() + before,path_found.end());
		return nodes[cur].ddfs_red;
	}
	else { //jump and then step from red
		size_t before = path_found.size();
		nodeid bud = nodes[cur].ddfs_red;
		while(bud != cur) bud = walk_blossom(bud);
		std::reverse(path_found.begin() + before,path_found.end());
		return nodes[cur].ddfs_green;
	}
}



//Walks a blossom from the entry node to the bud.
//It does not add the bud, it does add the entry node
nodeid MVGraph::walk_blossom(nodeid cur) {
	if(cur == UNSET) handle_error("MVGraph::walk_blossom(): cur is UNSET!\n");
  //~ debug("Walk blossom from: %i\n",cur->N);
	if(outer(cur)) cur = walk_blossom_down(cur,UNSET); //just walk down
    else {
		//walk up, then down
		cur = walk_blossom_up(cur);
		nodeid before = cur;
		cur = jump_bridge(cur);    
		cur = walk_blossom_down(cur,before);
	}
	return cur;
}

//walks down until it reacheds the bud of a blossom
//It does not add the bud, it does add the first node
nodeid MVGraph::walk_blossom_down(nodeid cur, nodeid before) {
	if(cur == UNSET) handle_error("MVGraph::walk_blossom_down(): cur is UNSET!\n");
  //~ debug("Walk blossom down: %i\n",cur->N);  
	if(before==UNSET) before = cur;
	nodeid b = bud(cur);
 	while(cur != UNSET && cur != b) {
		if( nodes[cur].ddfs_green != nodes[before].ddfs_green ||
				nodes[cur].ddfs_red != nodes[before].ddfs_red)
			//Step into a blossom
			cur = walk_blossom(cur);
		else {
			path_found.push_back(cur);
			cur = nodes[cur].below;
		}
	}
	return cur;
}



//Walks up untill it reaches a bridge
//It does add the bridge and the node it starts on
nodeid MVGraph::walk_blossom_up(nodeid cur) {
	if(cur == UNSET) handle_error("MVGraph::walk_blossom_up(): cur is UNSET!\n");
  //~ debug("Walk blossom up: %i\n",cur->N);  
	while(/*cur != UNSET*/ true) { // note: cur != UNSET when calling this function
		path_found.push_back(cur);
		if(nodes[cur].above == UNSET) break;
		nodeid bud = nodes[nodes[cur].above].below;
		if(bud == UNSET) handle_error("MVGraph::walk_blossom_up(): bud is UNSET!\n");
		if(bud != cur && bud_star_includes(bud,cur)) { //at a crossing or a jump 
			//About to jump over something
			size_t before = path_found.size();
			while(bud != cur) bud = walk_blossom(bud);
			std::reverse(path_found.begin() + before,path_found.end());
		}
		cur = nodes[cur].above;
	}
	return cur;
}



void MVGraph::augment_path() {
	for(size_t i=0;i+1<path_found.size();i+=2){
		//~ debug("%i --- %i\n",lll[i]->N,lll[i+1]->N);
		nodeid n1 = path_found[i];
		nodeid n2 = path_found[i+1];
		if(n1 == UNSET || n2 == UNSET) handle_error("MVGraph::augment_path(): nodes in path UNSET!\n");
		nodes[n1].match = n2;
		nodes[n2].match = n1;
	}
	matchnum++;
}

void MVGraph::remove_path() {
	while(path_found.size() > 0) {
		nodeid current = path_found.back();
		path_found.pop_back();
		if(!nodes[current].deleted) {
			//avoid double work
			nodes[current].deleted = true;
			
			for(auto& itt : nodes[current].pred_to) {
				MVNode& n = nodes[itt.first];
				if(!n.deleted) {
					n.preds[itt.second] = UNSET;
					n.number_preds--;
					if(n.number_preds <= 0) path_found.push_back(itt.first);
				}
			}
	    }
	}
}

