#ifndef MISC_H
#define MISC_H

#include <string>
#include <vector>

// convert seconds to human-readable "d m h s"-format
std::string simplify (long secs);

// split single string on mark
std::vector<std::string> split (const std::string& input, const char& mark);

int index_of (std::vector<std::string>, std::string);

#endif // MISC_H
