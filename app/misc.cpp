#include "misc.h"

std::string simplify (long secs) {
        int mins = (secs / 60) % 60;
        secs -= mins * 60;
        int hours = (secs / (60 * 60)) % 24;
        secs -= hours * 60 * 60;
        int days = (secs / (60 * 60 * 24));

        return std::string(std::to_string(days) + "d " + std::to_string(hours) + "h " + std::to_string(mins) + "m");
}

std::vector<std::string> split (const std::string& input, const char& mark) {
        std::vector<std::string> substrings;
        std::string buf = "";

        for (size_t i = 0; i < input.size(); ++i) {
                if (input.at(i) != mark) {
                        buf = buf + input.at(i);
                } else {
                        substrings.push_back(buf);
                        buf = "";
                }
        }

        if (buf != "") {
                substrings.push_back(buf);
        }

        return substrings;
}

int index_of (std::vector<std::string> arr, std::string el) {
        for (size_t i = 0; i < arr.size(); ++i) {
                if (el == arr.at(i)) {
                        return i;
                }
        }

        return -1;
}
