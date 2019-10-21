#ifndef LIB_MISC_H
#define LIB_MISC_H

template <typename K, typename V>
void set (std::unordered_map<K, V>& map, const K& key, const V& value) {
        // if already exists in map, update
        if (map.find(key) != map.end()) {
                map.at(key) = value;
                // otherwise insert
        } else {
                map.insert({key, value});
        }
}

template <typename K, typename V>
void insert_at (std::unordered_map<K, std::vector<V> >& map, const K& key, const V& value) {
        // if already exists in map, update
        if (map.find(key) != map.end()) {
                map.at(key).push_back(value);
                // otherwise insert
        } else {
                map.insert({key, {value}});
        }
}

namespace misc {
        template <typename K, typename V>
        void drop (std::unordered_map<K, std::vector<V> >& map, const K& key) {
                map.erase(key);
        }
};

#endif // LIB_MISC_H
