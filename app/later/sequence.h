/*
 * the sequencer creates sequences of edge insertions and deletions.
 * this can be done randomly, using some specific method or by reading
 * dynamic graphs from file.
 *
 * initially, it will support creating
 * - sequences with n nodes, k steps and only random additions
 * - sequences with n nodes, k steps and random additions and deletions
 * - sequences with n nodes, k steps and an average size of m edges (sliding window), constructed using random additions and deletions
 * - sequences with n nodes, k steps and an average size of m edges, where additions/deletions follow the idea of bergamini and meyerhenke.
 * - sequences from koblenz-files, where n and k are read from the file (only additions)
 * - sequences from koblenz-files, where n and k are read from the file and which maintains an average size of m edges (sliding window)
 * - sequences from koblenz-files, where n and k are read from the file and which maintains an average size of m edges (bergamini, meyerhenke)
 * - sequences from koblenz-files with n nodes, k steps, where n and k are given (only additions)
 * - sequences from koblenz-files with n nodes, k steps, where n and k are given and which maintains an average size of m edges (sliding window)
 * - sequences from koblenz-files with n nodes, k steps, where n and k are give and which maintains an average size of m edges (bergamini, meyerhenke)
 *
 */

#ifndef SEQUENCE_H
#define SEQUENCE_H

#include "dynamic_matching.h"

#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <climits>
#include <algorithm>
#include <chrono>

enum MODE {only_addition, random_step, sliding_window, meyerhenke, pooled_meyerhenke, native, batched};
extern const std::vector<std::string> MODE_NAMES;

enum FORMAT {konect, metis};
extern const std::vector<std::string> FORMAT_NAMES;

struct seq_config {
        size_t n;
        size_t k;
        MODE mode;
        size_t window;
        size_t poolsize;
        size_t repetitions;
        int base;
        long seed;
        FORMAT inputf;
        std::string ifile;

        bool read_all;
        bool all_n;
        bool all_w;
        bool shuffle;

        seq_config () {
                n = 0;
                k = 0;
                base = 0;
                mode = MODE::only_addition;
                window = 0;
                poolsize = 0;
                repetitions = 1;
                seed = -1;
                inputf = FORMAT::konect;
                ifile = "";
                read_all = true;
                all_n = true;
                all_w = true;
                shuffle = false;
        }
};

class sequence {
        public:
                sequence (const seq_config& conf);

                std::vector<std::string> split (const std::string& input, const char& mark);
                std::pair<NodeID, NodeID> load_from_file (FORMAT f);
                FORMAT determine_input_format ();

                std::pair<NodeID, NodeID> load_konect ();
                std::pair<NodeID, NodeID> load_metis ();

                std::pair<int, std::pair<NodeID, NodeID> > create_edge ();

                void create();
                void finish();

                void create_only_addition_seq();
                void create_random_seq();
                void create_sliding_window_seq();
                void create_meyerhenke_seq();
                void create_pooled_meyerhenke_seq();
                void create_native_seq();
                void create_batched_seq();

                std::string get_name();
                void print_last();

                bool check_break(size_t i);

        private:
                NodeID count_nodes();

                bool read_all;
                bool all_n;
                bool all_w;

                bool shuffle;

                size_t n;
                size_t k;
                MODE mode;
                
                int base;

                size_t window;
                size_t poolsize;
                size_t repetitions;

                long seed;

                std::string ifile;
                std::ostream o;

                FORMAT inputf;

                size_t add_count;
                size_t del_count;

                int helper;

                bool built;

                std::vector<bool> nodes;
                size_t edges_used;

                std::vector<std::pair<int, std::pair<NodeID, NodeID> > > buf;
                size_t it;

                std::vector<std::pair<int, std::pair<NodeID, NodeID> > > edge_sequence;

                bool ew_supported;
                bool nw_supported;
};

#endif // SEQUENCE_H
