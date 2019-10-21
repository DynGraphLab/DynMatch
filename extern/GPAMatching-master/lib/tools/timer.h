/******************************************************************************
 * timer.h 
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

#ifndef TIMER_INC_54
#define TIMER_INC_54

#include <sys/time.h>
#include <sys/resource.h> 
#include <unistd.h> 

class skappa_timer
{
 public:
  	skappa_timer() {
  		 start = timestamp(); 
  	} // postcondition: elapsed()==0
  	void   restart() { 
  		 start = timestamp(); 
  	} // post: elapsed()==0
  	
  	double elapsed()                  // return elapsed time in seconds
    { 
    	return timestamp()-start;
    }

 private:
  double start;
  
  


/** Returns a timestamp ('now') in seconds (incl. a fractional part). */
	inline double timestamp() {
    	struct timeval tp;
    	gettimeofday(&tp, NULL);
    	return double(tp.tv_sec) + tp.tv_usec / 1000000.;
	}
}; // timer

#endif  // TIMER_INC
