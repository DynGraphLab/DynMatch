//
// Author: Christian Schulz <christian.schulz@kit.edu>
// 

#ifndef MV_ALGORITHM_ZZNOLFV0
#define MV_ALGORITHM_ZZNOLFV0
 
#include "dyn_matching.h"
#include "Graph.h"
#include "graph_simple.h"
#include "read_table.h"


class mv_algorithm : public dyn_matching{
public:
        mv_algorithm(dyn_graph_access* G, MatchConfig & config); 
        virtual ~mv_algorithm();
  
        virtual bool new_edge(NodeID source, NodeID target);
        virtual bool remove_edge(NodeID source, NodeID target) ;
        virtual void postprocessing();
        virtual NodeID getMSize ();
        virtual std::vector< NodeID > & getM();

        void mv (dyn_graph_access& G, unsigned long& matching_size, std::vector<NodeID> & matching) {
                graph g1;
                std::unordered_map<unsigned int, unsigned int> ids;
                std::vector<std::pair<unsigned int, unsigned int> > _matching;

                std::vector<unsigned int> e1 = std::vector<unsigned int>(0), 
                        e2 = std::vector<unsigned int>(0);

                NodeID v;

                for (NodeID u = 0; u < G.number_of_nodes(); ++u) {
                        for (EdgeID e = 0; e < G.getNodeDegree(u); ++e) {
                                v = G.getEdgeTarget(u, e);

                                e1.push_back(u);
                                e2.push_back(v);
                        }
                }

                g1.create_graph_vectors(e1, e2, &ids);

                MVGraph g(g1);

                for(size_t i = 0; i < matching.size(); ++i) {
                        if (matching[i] != NOMATE) {
                                _matching.push_back({ids[i], ids[matching[i]]});
                        }
                }

                if( matching.size() > 0)
                        g.init(_matching);
                else g.greedy_init();

                g.max_match();
                matching_size = g.matchnum;

                std::vector< NodeID > output_matching(G.number_of_nodes(), NOMATE);
                g.get_matches(output_matching);
                matching = output_matching;
        }

        unsigned long matching_size;
        std::vector< NodeID> mv_matching;
};



#endif /* end of include guard: MV_ALGORITHM_ZZNOLFV0 */
