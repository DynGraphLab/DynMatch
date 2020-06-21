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
                std::pair<NodeID, NodeID> & edge = edge_sequence[i].second;

                if (edge_sequence.at(i).first) {
                        algorithm->new_edge(edge.first, edge.second);
                } else {
                        algorithm->remove_edge(edge.first, edge.second);
                }
        } 
        algorithm->postprocessing(); 
        matching_size = algorithm->getMSize();

        if (match_config.post_blossom) {
                timer tpost; tpost._restart();
                static_blossom sblossom(G, match_config);
                sblossom.init(algorithm->getM());
                sblossom.postprocessing();
                matching_size = sblossom.getMSize();

                std::cout <<  "postprocessing took " <<  tpost._elapsed() << std::endl;
                std::cout << matching_size << " " << " " << t._elapsed() << std::endl;
                check_matching(G, &sblossom, matching_size); 
        } else {
                std::cout << matching_size << " " << " " << t._elapsed() << std::endl;
                check_matching(G, algorithm, matching_size); 
        }
}


#endif /* end of include guard: DYNAMIC_ALGORITHM_META_2LKHAHEY */
