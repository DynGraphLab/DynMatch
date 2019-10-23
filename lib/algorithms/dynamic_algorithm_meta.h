//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#ifndef DYNAMIC_ALGORITHM_META_2LKHAHEY
#define DYNAMIC_ALGORITHM_META_2LKHAHEY


void run_dynamic_algorithm(dyn_graph_access * G, std::vector<std::pair<int, std::pair<NodeID, NodeID> > > &edge_sequence, dyn_matching * algorithm, MatchConfig & match_config ) {
        timer t; 
        t._restart(); 

        unsigned long matching_size = 0;
        for (size_t i = 0; i < edge_sequence.size(); ++i) { 
                std::pair<NodeID, NodeID> & edge = edge_sequence.at(i).second;

                if (edge_sequence.at(i).first) {
                        algorithm->new_edge(edge.first, edge.second);
                } else {
                        algorithm->remove_edge(edge.first, edge.second);
                }
        } 
        algorithm->postprocessing(); 

        if(match_config.post_mv || match_config.post_blossom) {
                std::cout <<  "running post processing with optimal algorithm. current matching size " << algorithm->getMSize()  << std::endl;
        }

        //TODO: matching is not output atm
        if (match_config.post_mv) {
                mv_algorithm mv(G, match_config);
                mv.mv(*G, matching_size, algorithm->getM());
        } else if (match_config.post_blossom) {
                blossom_untouched(edge_sequence, matching_size, algorithm->getM());
        } else {
                matching_size = algorithm->getMSize();
        }

        std::cout << matching_size << " " << " " << t._elapsed() << std::endl;
        check_matching(G, algorithm, matching_size); 
}
        

#endif /* end of include guard: DYNAMIC_ALGORITHM_META_2LKHAHEY */
