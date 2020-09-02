/******************************************************************************
 * quality_metrics.h
 *
 *****************************************************************************/


#ifndef QUALITY_METRICS_10HC2I5M
#define QUALITY_METRICS_10HC2I5M

#include "../definitions.h"

class quality_metrics {
        public:
                static std::vector<std::pair<NodeID, NodeID> > edgeset_intersect (std::vector<std::pair<NodeID, NodeID> > m_one, std::vector<std::pair<NodeID, NodeID> > m_two, int& union_size);
                static bool matching_validation (std::vector<std::pair<NodeID, NodeID> > matching);
};

#endif /* end of include guard: QUALITY_METRICS_10HC2I5M */
