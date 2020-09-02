/******************************************************************************
 * random_functions.cpp 
 *
 *****************************************************************************/

#include "random_functions.h"

MersenneTwister random_functions::m_mt;
std::mt19937_64 random_functions::mt64;
int random_functions::m_seed = 0;
/*
random_functions::random_functions()  {
}

random_functions::~random_functions() {
}
*/
