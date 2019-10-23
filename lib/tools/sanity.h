//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#ifndef SANITY_AEH1IUTO
#define SANITY_AEH1IUTO

void check_matching(dyn_graph_access * G, dyn_matching * algorithm, unsigned int matching_size) {
        std::cout << "************ checking matching ****************"  << std::endl;
        // check if matching is really a matching:
        std::vector< NodeID > matchingp = algorithm->getM();
        std::vector< bool > is_matched(G->number_of_nodes(), false);
        unsigned int internal_matching_size = 0;
        forall_nodes_d((*G), n) {
                NodeID matching_partner = matchingp[n];
                if( matching_partner == NOMATE ) {
                        continue;
                }

                if( matchingp[matching_partner] != n) {
                        std::cout <<  "not a matching " <<  n <<  " " << matching_partner << " " << matchingp[matching_partner]  << std::endl;
                }
                if( matching_partner > n ) continue; 
                if( is_matched[n] || is_matched[matching_partner] ) {
                        std::cout <<  "not a matching, vertex already matched"  << std::endl;
                }

                internal_matching_size++;
                is_matched[n] = true;
                is_matched[matching_partner] = true;
        } endfor
        if( internal_matching_size != matching_size ) {
                std::cout <<  "matching size does not match"  << std::endl;
        }
}

#endif /* end of include guard: SANITY_AEH1IUTO */
