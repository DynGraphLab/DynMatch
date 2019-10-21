/******************************************************************************
 * compare_rating.h 
 *
 * Source of GPA Matching Code 
 *
 ******************************************************************************
 * Copyright (C) 2013 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
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

#ifndef COMPARE_RATING_750FUZ7Z
#define COMPARE_RATING_750FUZ7Z

class compare_rating : public std::binary_function<double, double, bool> {
        public:
                compare_rating(graph_access * pG) : G(pG) {};
                virtual ~compare_rating() {};

                bool operator() (const double left, const double right ) {
                        return G->getEdgeWeight(left) > G->getEdgeWeight(right);
                }

        private:
                graph_access * G;
};


#endif /* end of include guard: COMPARE_RATING_750FUZ7Z */
