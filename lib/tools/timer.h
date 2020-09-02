/******************************************************************************
 * timer.h 
 *
 *****************************************************************************/

#ifndef TIMER_9KPDEP
#define TIMER_9KPDEP

#include <sys/time.h>
#include <sys/resource.h> 
#include <unistd.h> 

class timer {
        public:
                static void restart() { 
                        m_start = timestamp(); 
                } 

                static double elapsed() { 
                        return timestamp()-m_start;
                }

                void _restart() { 
                        _m_start = _timestamp(); 
                } 

                double _elapsed() { 
                        return _timestamp()-_m_start;
                }

        private:

                /** Returns a timestamp ('now') in seconds (incl. a fractional part). */
                static inline double timestamp() {
                        struct timeval tp;
                        gettimeofday(&tp, NULL);
                        return double(tp.tv_sec) + tp.tv_usec / 1000000.;
                }

                static double m_start;

                inline double _timestamp() {
                        struct timeval tp;
                        gettimeofday(&tp, NULL);
                        return double(tp.tv_sec) + tp.tv_usec / 1000000.;
                }

                double _m_start;

}; 

#endif /* end of include guard: TIMER_9KPDEP */
