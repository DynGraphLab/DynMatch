#include <iostream>
#include <fstream>
#include <vector>

#include "dynamic_matching.h"

struct fileio {
        int index = 0;
        int algorithm = 0; //
        int num_of_algorithms = 5;

        std::string filename;
        std::ifstream input;

        fileio (std::string filename, int num_of_algorithms = 1);

        void reset ();

        bool next_matching (std::vector<std::pair<NodeID, NodeID> >& matching);
        bool next_gpa_matching (std::vector<std::pair<NodeID, NodeID> >& matching);
};

std::vector<std::string> split (const std::string& input, const char& mark);
bool parse_to_matching (std::vector<std::pair<NodeID, NodeID> >& matching, std::string line);


int main (int argc, char ** argv) {
        std::string prefix;

        if (argc >= 2) {
                prefix = argv[1];
        } else {
                std::cout << "error. no directory path given." << std::endl;
                return 1;
        }

        std::string line;
        std::ifstream input(std::string(prefix + "matchings"));
        int num_of_algorithms = -1;

        do {
                getline(input, line);
                num_of_algorithms++;
        } while (line != "");

        std::cout << "read " << num_of_algorithms << " algorithms." << std::endl;

        // initialize fileio access
        fileio matchings(std::string(prefix + "matchings"), num_of_algorithms);
        fileio gpa_matchings_io(std::string(prefix + "matchings_gpa"));


        // buffer matching
        std::vector<std::pair<NodeID, NodeID> > matching;

        // result vector
        std::vector<std::vector<int> > results;

        // (x,y[,z]) : xth algorithm, yth matching, zth edge in matching
        std::vector<std::vector<std::vector<std::pair<NodeID, NodeID> > > > all_matchings;


        // get all gpa_matchings into vector
        std::vector<std::vector<std::pair<NodeID, NodeID> > > gpa_matchings;

        if (gpa_matchings_io.input.good()) {
                while (gpa_matchings_io.next_gpa_matching(matching)) {
                        gpa_matchings.push_back(matching);
                }

                results = std::vector<std::vector<int> >(gpa_matchings.size());

                for (int i = 0; i < matchings.num_of_algorithms; ++i) {
                        matchings.algorithm = i;
                        size_t j = 0;

                        all_matchings.push_back(std::vector<std::vector<std::pair<NodeID, NodeID> > >());

                        while (matchings.next_matching(matching) && j < gpa_matchings.size()) {
                                all_matchings.at(i).push_back(matching);

                                int union_size = 1;
                                int intersect_size = quality_metrics::edgeset_intersect(gpa_matchings.at(j), matching, union_size).size();

                                results.at(j).push_back(gpa_matchings.at(j).size()/2);
                                results.at(j).push_back(matching.size()/2);
                                results.at(j).push_back(intersect_size/2);
                                j++;
                        }

                        matchings.reset();
                }
        } else {
                int i = 0;
                while (matchings.next_matching(matching)) {
                        i++;
                }

                results = std::vector<std::vector<int> >(i);

                matchings.reset();

                for (int i = 0; i < matchings.num_of_algorithms; ++i) {
                        matchings.algorithm = i;
                        all_matchings.push_back(std::vector<std::vector<std::pair<NodeID, NodeID> > >());

                        while(matchings.next_matching(matching)) {
                                all_matchings.at(i).push_back(matching);
                        }

                        matchings.reset();
                }
        }

        for (size_t i = 0; i < all_matchings.size(); ++i) {
                for (size_t j = i+1; j < all_matchings.size(); ++j) {
                        ASSERT_TRUE(all_matchings.at(i).size() == all_matchings.at(j).size());
                        for (size_t k = 0; k < all_matchings.at(i).size(); ++k) {
                                int union_size = 1;
                                int intersect_size = quality_metrics::edgeset_intersect(all_matchings.at(i).at(k), all_matchings.at(j).at(k), union_size).size();

                                results.at(k).push_back(all_matchings.at(i).at(k).size()/2);
                                results.at(k).push_back(all_matchings.at(j).at(k).size()/2);
                                results.at(k).push_back(intersect_size/2);
                        }
                }
        }

        std::ofstream output(std::string(prefix + "intersects"));

        for (size_t i = 0; i < results.size(); ++i) {
                for (size_t j = 0; j < results.at(i).size(); ++j) {
                        output << results.at(i).at(j) << " ";
                }

                output << std::endl;
        }

        return 0;
}


fileio::fileio (std::string filename, int num_of_algorithms) {
        this->filename = filename;
        this->num_of_algorithms = num_of_algorithms;
        input = std::ifstream(filename);
}

void fileio::reset () {
        input.clear();
        input.seekg(0, std::ios::beg);
        index = 0;
}

bool fileio::next_matching (std::vector<std::pair<NodeID, NodeID> >& matching) {
        matching.resize(0);
        std::string line;

        if (input.is_open()) {
                // from each block of num_of_algorithms lines (=matchings) we only want one.
                // num_of_algorithms +1 for the blank line.
                while (index % (num_of_algorithms+1) != algorithm) {
                        getline(input, line);
                        index++;

                        if (input.eof()) return false;
                } // as soon as the condition doesn't match, we stop at the right position.

                if (input.eof()) return false;
                getline(input, line);
                index++;

                if (line == "" && input.eof()) return false;
        }

        return parse_to_matching(matching, line);
}

bool fileio::next_gpa_matching (std::vector<std::pair<NodeID, NodeID> >& matching) {
        matching.resize(0);
        std::string line;

        if (input.is_open()) {
                getline(input, line);
        }

        if (input.eof()) return false;

        return parse_to_matching(matching, line);
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

bool parse_to_matching (std::vector<std::pair<NodeID, NodeID> >& matching, std::string line) {
        std::vector<std::string> substrings = split(line, ';');

        for (size_t i = 0; i < substrings.size(); ++i) {
                std::vector<std::string> edge = split(substrings.at(i), ' ');
                size_t j = (edge.at(0) == "" ? 1 : 0);

                if (edge.size() > j+1) {
                        matching.push_back({
                                std::stoi(edge.at(j)),
                                std::stoi(edge.at(j+1))
                        });
                }
        }

        return true;
}
