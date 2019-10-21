#include "sequence.h"

const std::vector<std::string> MODE_NAMES = {"addition-only", "random-step", "sliding-window", "simple-outage", "pooled-outage", "native", "batched"};
const std::vector<std::string> FORMAT_NAMES = {"konect", "metis"};

sequence::sequence (const seq_config& conf) :
        o { NULL }
{
        this->n = conf.n;
        this->k = conf.k;
        this->mode = conf.mode;
        this->base = base;

        this->read_all = conf.read_all;
        this->all_n = conf.all_n;
        this->all_w = conf.all_w;

        this->shuffle = conf.shuffle;

        this->window = conf.window;
        this->poolsize = conf.poolsize;
        this->repetitions = conf.repetitions;

        if (seed < 0) {
                this->seed = static_cast<long unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        } else {
                this->seed = conf.seed;
        }

        random_functions::setSeed(this->seed);

        helper = 0;

        this->ifile = conf.ifile;

        this->inputf = conf.inputf;

        o.rdbuf(std::cout.rdbuf());

        add_count = 0;
        del_count = 0;

        nodes.resize(n);
        edges_used = 0;

        built = false;

        it = 0;
}

std::vector<std::string> sequence::split (const std::string& input, const char& mark) {
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

std::pair<NodeID, NodeID> sequence::load_from_file(FORMAT f) {
        if (f == FORMAT::konect) {
                return load_konect();
        } else if (FORMAT::metis) {
                return load_metis();
        }
}

FORMAT sequence::determine_input_format () {
        return inputf;
}


std::pair<NodeID, NodeID> sequence::load_konect() {
        std::ifstream input(ifile);
        std::string line;

        int lineCount = 0;
        std::vector<std::pair<std::pair<int, int>, std::pair<NodeID, NodeID> > > from_file;

        //    input.open();
        long n_buf = -1;
        long k_buf = -1;

        NodeID max_u = 0;
        NodeID max_v = 0;

        size_t i = 0;
        //    int skipped = 0;

        if (input.is_open()) {
                while (std::getline(input, line)) {
                        /* if (line.size() != 0 && line.at(0) == '%' && lineCount == 1) {
                                // first valid line has to be the n
                                if (n_buf < 0 && k_buf < 0) {
                                        std::vector<std::string> substrings = split(line, ' ');

                                        k_buf = atoi(substrings.at(1).c_str());
                                        n_buf = atoi(substrings.at(2).c_str());

                                        if (substrings.size() > 3) {
                                                long check = atoi(substrings.at(3).c_str());

                                                if (n_buf != check) {
                                                        n_buf += check;
                                                }
                                        }
                                }
                        } else */
                        if (line.size() != 0 && line.at(0) != '%' && line.at(0) != '#') {
                                if(check_break(i)) {
                                        break;
                                }
                                std::vector<std::string> substrings = split(line, ' ');

                                // n counts from 0 to n, the koblenz format starts counting from 1, therefore we substract 1
                                NodeID u = atoi(substrings.at(0).c_str()) - base;
                                NodeID v = atoi(substrings.at(1).c_str()) - base;

                                if (u == v) continue;

                                if (!all_n) {
                                        if (u > max_u && u < n) max_u = u;
                                        if (v > max_v && v < n) max_v = v;
                                } else {
                                        if (u > max_u) max_u = u;
                                        if (v > max_v) max_v = v;
                                }

                                if (substrings.size() >= 4) {
                                        int addition = (atoi(substrings.at(2).c_str()) < 0 ? 0 : 1);
                                        int timestamp = atoi(substrings.at(3).c_str());

                                        if (!all_n) {
                                                if (u < n && v < n && u != v) {
                                                        from_file.push_back({{timestamp, addition}, {u, v}});
                                                        i++;
                                                }
                                        } else {
                                                if (u != v) {
                                                        from_file.push_back({{timestamp, addition}, {u, v}});
                                                        i++;
                                                }
                                        }
                                } else {
                                        if (!all_n) {
                                                if (u < n && v < n && u != v) {
                                                        from_file.push_back({{i, 1}, {u, v}});
                                                        i++;
                                                }
                                        } else {
                                                if (u != v) {
                                                        from_file.push_back({{i, 1}, {u, v}});
                                                        i++;
                                                }
                                        }
                                }
                        }

                        lineCount++;
                }

                input.close();
        } else {
                throw new std::string("could not find file " + ifile);
        }
        
        // now we sort edges_buf by timestamp using std::sort
        std::sort(
                from_file.begin(),
                from_file.end(),
        [](const std::pair<std::pair<int, int>, std::pair<NodeID, NodeID> >& a, const std::pair<std::pair<int, int>, std::pair<NodeID, NodeID> >& b) {
                return a.first.first < b.first.first;
        }
        );

        for (size_t i = 0; i < from_file.size(); ++i) {
                buf.push_back({from_file.at(i).first.second, from_file.at(i).second});
        }

        //    std::cout << "read " << buf.size() << " edges." << std::endl;

        // adjust sequence size to size of sequence in file
        if (!read_all) {
                if (mode == MODE::only_addition || mode == MODE::random_step || mode == MODE::native) {
                        if (buf.size() < k) {
                                k = buf.size();
                        }
                } else if (mode == MODE::sliding_window) {
                        if (buf.size() < k/2) {
                                k = 2 * buf.size();
                        }
                } else {
                        if (buf.size() < window) {
                                window = buf.size();
                        }
                }
        } else {
                if (mode == MODE::only_addition || mode == MODE::random_step || mode == MODE::native) {
                        k = buf.size();
                } else if (mode == MODE::sliding_window) {
                        k = 2 * buf.size();
                } else {
                        window = buf.size();
                }
        }

        nodes.resize((max_u > max_v ? max_u : max_v) + 1);

        //    std::cout << "k after file read is " << k << std::endl;

        return {nodes.size(), nodes.size()};
} // load_konect()

std::pair<NodeID, NodeID> sequence::load_metis () {
        std::ifstream in(ifile);
        std::string line;

        long nmbNodes;
        long nmbEdges;

        std::getline(in,line);
        //skip comments
        while( line[0] == '%' ) {
                std::getline(in, line);
        }

        int ew = 0;
        std::stringstream ss(line);
        ss >> nmbNodes;
        ss >> nmbEdges;
        ss >> ew;

        if( 2*nmbEdges > std::numeric_limits<long>::max() || nmbNodes > std::numeric_limits<long>::max()) {
                throw std::string("The graph is too large. Currently only 32bit supported!");
        }

        if( n > (NodeID) nmbNodes) {
                std::cerr <<  "less nodes available in input file than specified."  << std::endl;
        }

        bool read_ew = false;
        bool read_nw = false;

        if(ew == 1) {
                read_ew = true;
        } else if (ew == 11) {
                read_ew = true;
                read_nw = true;
        } else if (ew == 10) {
                read_nw = true;
        }

        nmbEdges *= 2;

        NodeID node_counter   = 0;
        EdgeID edge_counter   = 0;
        long long total_nodeweight = 0;

        while (std::getline(in, line)) {
                if (check_break (edge_counter)) {
                        break;
                }

                if (line[0] == '%') { // a comment in the file
                        continue;
                }

                // ignore edges where one endpoint exceeds the given limit of n vertices
                if (!all_n && node_counter >= n) {
                        continue;
                }

                NodeID node = node_counter++;

                std::stringstream ss(line);

                NodeWeight weight = 1;
                if( read_nw && nw_supported && ew_supported ) {
                        ss >> weight;
                        total_nodeweight += weight;
                        if( total_nodeweight > (long long) std::numeric_limits<NodeWeight>::max()) {
                                throw std::string("The sum of the node weights is too large (it exceeds the node weight type).\n"
                                                  "Currently not supported. Please scale your node weights.");
                        }
                }

                NodeID target;
                while( ss >> target ) {
                        // change 1-based counting to 0-based
                        target -= 1;

                        //check for self-loops
                        if(target == node) {
                                throw std::string("The graph file contains self-loops. This is not supported. Please remove them from the file.");
                        }

                        // ignore edges where one endpoint exceeds the given limit of n vertices
                        if (!all_n && target >= n) {
                                continue;
                        }

                        // ignore edges where endpoint has id lower than starting point.
                        // like this we prevent double addition of same edge.
                        if (target < node) {
                                continue;
                        }

                        EdgeWeight edge_weight = 1;
                        if( read_ew ) {
                                ss >> edge_weight;
                        }
                        edge_counter++;

                        buf.push_back({1, {node, target}});
                }

                if(in.eof()) {
                        break;
                }
        }
        
        std::cerr << "read " << buf.size() << " edges from file." << std::endl;

        nodes.resize(node_counter);
        k = buf.size();

        return {node_counter, node_counter};
} // load_metis()

bool sequence::check_break (size_t i) {
        if (read_all) {
                return false;
        }

        if (inputf == FORMAT::konect) {
                if (mode == MODE::meyerhenke || mode == MODE::pooled_meyerhenke) {
                        if (i >= window) return true;
                } else if (mode == MODE::only_addition || mode == MODE::random_step || mode == MODE::native) {
                        if (i >= k) return true;
                } else if (mode == MODE::sliding_window) {
                        if (2 * i >= k) return true;
                }
        } else {
                if (i >= k) {
                        return true;
                }
        }

        return false;
}

// only_addition, random, sliding_window, meyerhenke
void sequence::create () {
        if (built) return;

        std::pair<NodeID, NodeID> max_n;

        if (ifile != "") {
                FORMAT f = determine_input_format();
                max_n = load_from_file(f);
        } else {
                max_n = {n, n};
        }
    
        if (all_w) {
                window = buf.size();
        }

        if (shuffle) {
                std::cerr << "shuffling..." << std::endl;
                std::shuffle(std::begin(buf), std::end(buf), random_functions::getRNG());
        }

        o << "# " << (max_n.first > max_n.second ? max_n.first : max_n.second) << " " << k << std::endl;

        if (mode == MODE::only_addition) {
                create_only_addition_seq();
        } else if (mode == MODE::random_step) {
                create_random_seq();
        } else if (mode == MODE::sliding_window) {
                create_sliding_window_seq();
        } else if (mode == MODE::meyerhenke) {
                create_meyerhenke_seq();
        } else if (mode == MODE::pooled_meyerhenke) {
                create_pooled_meyerhenke_seq();
        } else if (mode == MODE::native) {
                create_native_seq();
        } else if (mode == MODE::batched) {
                create_batched_seq();
        } else {
                return;
        }

        built = true;
}

void sequence::finish () {
        //    o.close();
}

void sequence::print_last () {
        std::pair<int, std::pair<NodeID, NodeID> > edge = edge_sequence.at(edge_sequence.size() - 1);
        o << edge.first << " " << edge.second.first << " " << edge.second.second << std::endl;
}

std::pair<int, std::pair<NodeID, NodeID> > sequence::create_edge () {
        if (ifile == "") {
                int u = random_functions::nextInt(0, n-1);
                int v;

                do { // avoid that u == v
                        v = random_functions::nextInt(0, n-1);
                } while (v == u);

                nodes.at(u) = true;
                nodes.at(v) = true;

                return std::pair<int, std::pair<NodeID, NodeID> >({1, {u, v}});
        } else {
                if (it < buf.size()) {
                        std::pair<int, std::pair<NodeID, NodeID> > edge = buf.at(it++);

                        nodes.at(edge.second.first) = true;
                        nodes.at(edge.second.second) = true;

                        edges_used++;

                        return edge;
                } else {
                        throw std::string("no more edge insertions in file: " + std::to_string(it));
                }
        }
}

void sequence::create_native_seq() {
        /* creates a sequence of length k, containing only random additions
         */

        for (size_t i = 0; i < k; ++i) {
                std::pair<int, std::pair<NodeID, NodeID> > edge = create_edge();
                edge_sequence.push_back(edge);
                add_count++;
                print_last();
        }
}

void sequence::create_only_addition_seq() {
        /* creates a sequence of length k, containing only random additions
         */
        for (size_t i = 0; i < k; ++i) {
                std::pair<NodeID, NodeID> edge = create_edge().second;
                edge_sequence.push_back({1, edge});
                add_count++;
                print_last();
        }
}

void sequence::create_random_seq() {
        std::vector<std::pair<NodeID, NodeID> > additions;

        for (size_t i = 0; i < k; ++i) {
                bool addition;

                if (additions.size() >= 1) { // only allow possibility of deletion, when there are added edges
                        addition = random_functions::nextInt(0,1);
                } else {
                        addition = true;
                }

                if (addition) { // if it's an addition, I simply create a random edge with u,v \in [0,n-1]
                        std::pair<NodeID, NodeID> edge = create_edge().second;
                        edge_sequence.push_back({1, edge});
                        additions.push_back(edge);
                        add_count++;
                } else { // if it's an deletion, I take a random edge from the vector of additions and add it as deletion.
                        int index = random_functions::nextInt(0, additions.size()-1);
                        edge_sequence.push_back({0, additions.at(index)});
                        additions.at(index) = additions.back();
                        additions.pop_back();
                        del_count++;
                }
                print_last();
        }
}

void sequence::create_sliding_window_seq() {
        std::vector<std::pair<NodeID, NodeID> > additions;

        size_t i = 0;
        size_t j = 0;

        for ( ; i < window && i < k; i++) {
                std::pair<NodeID, NodeID> edge = create_edge().second;
                edge_sequence.push_back({1, edge});
                additions.push_back(edge);
                add_count++;
                print_last();
        }

        for ( ; i < k - window; i += 2, j++) {
                std::pair<NodeID, NodeID> edge = create_edge().second;
                edge_sequence.push_back({1, edge});
                additions.push_back(edge);
                add_count++;
                print_last();

                edge = additions.at(j);
                edge_sequence.push_back({0, edge});
                del_count++;
                print_last();
        }

        for ( ; j < additions.size(); j++) {
                std::pair<NodeID, NodeID> edge = additions.at(j);
                edge_sequence.push_back({0, edge});
                del_count++;
                print_last();
        }
}

void sequence::create_meyerhenke_seq() {
        std::vector<std::pair<NodeID, NodeID> > additions;

        size_t i = 0;

        for (; i < window;) {
                std::pair<NodeID, NodeID> edge = create_edge().second;
                edge_sequence.push_back({1, edge});
                additions.push_back(edge);
                i++;
                add_count++;
                print_last();
        }

        for (; i < k - window;) {
                int index = random_functions::nextInt(0, additions.size()-1);
                std::pair<NodeID, NodeID> edge = additions.at(index);

                edge_sequence.push_back({0, edge});
                i++;
                del_count++;
                print_last();

                edge_sequence.push_back({1, edge});
                i++;
                add_count++;
                print_last();
        }

        for (int j = 0; i < k;) {
                std::pair<NodeID, NodeID> edge = additions.at(j++);
                edge_sequence.push_back({0, edge});
                i++;
                del_count++;
                print_last();
        }
}

void sequence::create_pooled_meyerhenke_seq() {
        std::vector<std::pair<NodeID, NodeID> > additions;
        std::vector<std::pair<NodeID, NodeID> > pool;

        size_t i = 0;

        for (; i < window;) {
                std::pair<NodeID, NodeID> edge = create_edge().second;
                edge_sequence.push_back({1, edge});
                additions.push_back(edge);
                i++;
                add_count++;
                print_last();
        }

        for (; i < window * (1 + poolsize/100.0);) {
                int index = random_functions::nextInt(0, additions.size()-1);
                std::pair<NodeID, NodeID> edge = additions.at(index);
                pool.push_back(edge);

                edge_sequence.push_back({0, edge});
                i++;
                del_count++;
                print_last();
        }

        for (; i < k - window;) {
                if(additions.size() == 0 || (random_functions::nextInt(0, 1) && pool.size() != 0)) { // deletion
                        int index = random_functions::nextInt(0, pool.size()-1);
                        std::pair<NodeID, NodeID> edge = pool.at(index);

                        // remove edge from pool by overwriting taken edge with last edge, then deleting last edge
                        pool.at(index) = pool.back();
                        pool.pop_back();

                        additions.push_back(edge);

                        edge_sequence.push_back({1, edge});
                        i++;
                        add_count++;
                        print_last();
                } else { // insertion
                        int index = random_functions::nextInt(0, additions.size()-1);
                        std::pair<NodeID, NodeID> edge = additions.at(index);

                        // remove edge from additions by overwriting taken edge with last edge, then deleting last edge
                        additions.at(index) = additions.back();
                        additions.pop_back();

                        pool.push_back(edge);

                        edge_sequence.push_back({0, edge});
                        i++;
                        del_count++;
                        print_last();
                }
        }

        for (size_t j = 0; j < additions.size(); j++ ) {
                std::pair<NodeID, NodeID> edge = additions.at(j);
                edge_sequence.push_back({0, edge});

                del_count++;
                print_last();
        }
}

void sequence::create_batched_seq() {
        std::vector<std::pair<NodeID, NodeID> > additions;
        std::vector<std::pair<NodeID, NodeID> > pool;

        size_t i = 0;

        for (; i < window; ) {
                std::pair<NodeID, NodeID> edge = create_edge().second;
                edge_sequence.push_back({1, edge});
                additions.push_back(edge);
                i++;
                add_count++;
                print_last();
        }

        size_t r = 0;

        for (; r < repetitions; r++) {
                size_t j = 0;

                for (; j < window * (poolsize/100.0); ) {
                        int index = random_functions::nextInt(0, additions.size()-1);
                        std::pair<NodeID, NodeID> edge = additions.at(index);

                        pool.push_back(edge);

                        additions.at(index) = additions.back();
                        additions.pop_back();

                        edge_sequence.push_back({0, edge});
                        i++;
                        j++;
                        del_count++;
                        print_last();
                }

                std::shuffle(std::begin(pool), std::end(pool), random_functions::getRNG());
                j = pool.size();

                for (; j > 0; ) {
                        std::pair<NodeID, NodeID> edge = pool.at(j-1);

                        pool.pop_back();
                        additions.push_back(edge);

                        edge_sequence.push_back({1, edge});
                        i++;
                        j--;
                        add_count++;
                        print_last();
                }
        }
}

std::string sequence::get_name() {
        std::string name = "";

        if (ifile == "") {
                name = name + "fromseed_";
        } else {
                std::vector<std::string> foo = split(ifile, '/');
                name = name + foo.at(foo.size() - 1) + "_";
        }

        name = name + std::to_string(seed) + "_";
        name = name + std::to_string(n) + "_";
        name = name + std::to_string(k) + "_";
        name = name + MODE_NAMES.at(mode);

        if (mode == MODE::sliding_window || mode == MODE::meyerhenke || mode == MODE::pooled_meyerhenke) {
                name = name + "_" + std::to_string(window);
        }

        name = name + ".sequence";

        return name;
}

NodeID sequence::count_nodes() {
        NodeID node_count = 0;

        for (auto n : nodes) {
                if (n) node_count++;
        }

        return node_count;
}
