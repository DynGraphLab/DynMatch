/******************************************************************************
 * en_dyn_matching.h
 *
 *
 ******************************************************************************
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef EN_DYN_MATCHING
#define EN_DYN_MATCHING

#include <bitset>
#include <cassert>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>

//#include "mrw_dyn_matching.h"
#include "rw_dyn_matching.h"
#include "../tools/random_functions.h"
#include "../tools/timer.h"


class en_dyn_matching : public rw_dyn_matching {
        public:
                en_dyn_matching (dyn_graph_access* G, double eps, unsigned multiple_rws = 1);

                virtual void counters_next();

        protected:
                virtual void handle_insertion (NodeID source, NodeID target);
                virtual void handle_deletion (NodeID source, NodeID target);

                virtual bool settle (NodeID u);
};

#endif // EN_DYN_MATCHING
