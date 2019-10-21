#include "counters.h"

std::vector<std::pair<std::string, counters::counter_set> > counters::counter_sets;



//===// counters::counter methods //===//

counters::counter::counter () {
        records = std::vector<size_t>();
        iterator = 0;
        restarted = false;
        next();
        ASSERT_TRUE(records.size() != 0);
}

void counters::counter::inc () {
        if (restarted) {
                records.at(iterator) = records.at(iterator) + 1;
        } else {
                records.back() = records.back() + 1;
        }
}

void counters::counter::dec () {
        if (restarted) {
                records.at(iterator) = records.at(iterator) - 1;
        } else {
                records.back() = records.back() - 1;
        }
}

void counters::counter::next () {
        if (restarted) {
                iterator++;
        } else {
                records.push_back(0);
        }
}

void counters::counter::next (size_t value) {
        if (restarted) {
                records.at(iterator) = value;
        } else {
                records.push_back(value);
        }
}

void counters::counter::add (size_t value) {
        ASSERT_TRUE(records.size() != 0);
        if (restarted) {
                records.at(iterator) = records.at(iterator) + value;
        } else {
                records.back() = records.back() + value;
        }
}

void counters::counter::put (size_t value) {
        if (restarted) {
                records.at(iterator) = value;
        } else {
                records.back() = value;
        }
}

size_t counters::counter::get () {
        if (restarted) {
                return records.at(iterator);
        } else {
                return records.back();
        }
}

std::vector<size_t>& counters::counter::all () {
        return records;
}

void counters::counter::reset () {
        records.resize(0);
        next();
}

void counters::counter::restart () {
        restarted = true;
        iterator = 0;
}



//===// counters::counter_d methods //===//

counters::counter_d::counter_d () {
        records = std::vector<double>();
        iterator = 0;
        restarted = false;
        next();
        ASSERT_TRUE(records.size() != 0);
}

void counters::counter_d::inc () {
        if (restarted) {
                records.at(iterator) = records.at(iterator) + 1;
        } else {
                records.back() = records.back() + 1;
        }
}

void counters::counter_d::dec () {
        if (restarted) {
                records.at(iterator) = records.at(iterator) - 1;
        } else {
                records.back() = records.back() - 1;
        }
}

void counters::counter_d::next () {
        if (restarted) {
                iterator++;
        } else {
                records.push_back(0);
        }
}

void counters::counter_d::next (double value) {
        if (restarted) {
                records.at(iterator) = value;
        } else {
                records.push_back(value);
        }
}

void counters::counter_d::add (double value) {
        ASSERT_TRUE(records.size() != 0);
        if (restarted) {
                records.at(iterator) = records.at(iterator) + value;
        } else {
                records.back() = records.back() + value;
        }
}

void counters::counter_d::put (double value) {
        if (restarted) {
                records.at(iterator) = value;
        } else {
                records.back() = value;
        }
}

double counters::counter_d::get () {
        if (restarted) {
                return records.at(iterator);
        } else {
                return records.back();
        }
}

std::vector<double>& counters::counter_d::all () {
        return records;
}

void counters::counter_d::reset () {
        records.resize(0);
        next();
}

void counters::counter_d::restart () {
        restarted = true;
        iterator = 0;
}



//===// counters::counter_set methods //===//

std::vector<std::pair<std::string, counters::counter> > counters::counter_set::all () {
        return registered_counters;
}

counters::counter& counters::counter_set::create (const std::string& name) {
        if (exists(name)) {
                get(name).restart();
                return get(name);
        }

        registered_counters.push_back({name, counter()});
        return registered_counters.back().second;
}

bool counters::counter_set::exists (const std::string& name) {
        for (size_t i = 0; i < registered_counters.size(); ++i) {
                if (registered_counters.at(i).first == name) {
                        return true;
                }
        }

        return false;
}

counters::counter& counters::counter_set::get (const std::string& name) {
        for (size_t i = 0; i < registered_counters.size(); ++i) {
                if (registered_counters.at(i).first == name) {
                        return registered_counters.at(i).second;
                }
        }

        throw std::string("counter \"" + name + "\" not found.");
}

void counters::counter_set::divide_by (size_t divisor) {
        for (size_t i = 0; i < registered_counters.size(); ++i) {
                for (size_t j = 0; j < registered_counters.at(i).second.records.size(); ++j) {
                        registered_counters.at(i).second.records.at(j) = (1.0 * registered_counters.at(i).second.records.at(j)) / divisor;
                }
        }
}


std::vector<std::pair<std::string, counters::counter_d> > counters::counter_set::all_d () {
        return registered_counters_d;
}

counters::counter_d& counters::counter_set::create_d (const std::string& name) {
        if (exists_d(name)) {
                get_d(name).restart();
                return get_d(name);
        }

        registered_counters_d.push_back({name, counter_d()});
        return registered_counters_d.back().second;
}

bool counters::counter_set::exists_d (const std::string& name) {
        for (size_t i = 0; i < registered_counters_d.size(); ++i) {
                if (registered_counters_d.at(i).first == name) {
                        return true;
                }
        }

        return false;
}

counters::counter_d& counters::counter_set::get_d(const std::string& name) {
        for (size_t i = 0; i < registered_counters_d.size(); ++i) {
                if (registered_counters_d.at(i).first == name) {
                        return registered_counters_d.at(i).second;
                }
        }

        throw std::string("counter \"" + name + "\" not found.");
}

void counters::counter_set::divide_by_d (double divisor) {
        for (size_t i = 0; i < registered_counters_d.size(); ++i) {
                for (size_t j = 0; j < registered_counters_d.at(i).second.records.size(); ++j) {
                        registered_counters_d.at(i).second.records.at(j) = (1.0 * registered_counters_d.at(i).second.records.at(j)) / divisor;
                }
        }
}


void counters::counter_set::println (std::ostream& o, size_t i) {
        for (auto c : all() ) {
                o << c.second.all().at(i) << " ";
        }

        for (auto c : all_d() ) {
                o << c.second.all().at(i) << " ";
        }
}



//===// static counters method //===//

std::vector<std::pair<std::string, counters::counter_set> > counters::all () {
        return counter_sets;
}

counters::counter_set& counters::create (const std::string& name) {
        if (exists(name)) {
                //            get(name).restart();
                return get(name);
        }

        counter_sets.push_back({name, counter_set()});
        return counter_sets.back().second;
}

bool counters::exists (const std::string& name) {
        for (size_t i = 0; i < counter_sets.size(); ++i) {
                if (counter_sets.at(i).first == name) {
                        return true;
                }
        }

        return false;
}

counters::counter_set& counters::get (const std::string& name) {
        for (size_t i = 0; i < counter_sets.size(); ++i) {
                if (counter_sets.at(i).first == name) {
                        return counter_sets.at(i).second;
                }
        }

        throw std::string("counter set \"" + name + "\" not found.");
}

void counters::divide_by (size_t divisor) {
        for (auto cs : all()) {
                cs.second.divide_by(divisor);
        }
}

void counters::divide_by_d (double divisor) {
        for (auto cs : all()) {
                cs.second.divide_by_d(divisor);
        }
}


void counters::print_names(std::ostream& o) {
        for (auto cs : all()) {
                for (auto c : cs.second.all()) {
                        o << cs.first << "_" << c.first << std::endl;
                }

                for (auto c : cs.second.all_d()) {
                        o << cs.first << "_" << c.first << std::endl;
                }
        }
}

void counters::print (std::ostream& o) {
        size_t min_size = -1;
        size_t min_counter = 0;

        for (auto cs : all()) {
                for (auto c : cs.second.all()) {
                        if (c.second.all().size() < min_size) {
                                min_size = c.second.all().size();
                                min_counter++;
                        }
                }

                for (auto c : cs.second.all_d()) {
                        if (c.second.all().size() < min_size) {
                                min_size = c.second.all().size();
                                min_counter++;
                        }
                }
        }

        if (min_counter > 1) {
                std::cout << min_counter << " counter records were smaller than the other counter record sets." << std::endl;
        }

        if (min_counter == 0) { // if min_counter is zero, min_size is still at max value for min_size.
                min_size = 0;
        }

        o << "# ";
        for (auto cs : all()) {
                o << cs.first << ":{";
                for (auto c : cs.second.all()) {
                        if (c.first == cs.second.all().back().first) {
                                o << c.first;
                        } else {
                                o << c.first << ", ";
                        }
                }

                if (cs.second.all_d().size() == 0) {
                        o << "}";
                } else {
                        o << ", ";
                }

                for (auto c : cs.second.all_d()) {
                        if (c.first == cs.second.all_d().back().first) {
                                o << c.first << "}";
                        } else {
                                o << c.first << ", ";
                        }
                }

                if (cs.first != all().back().first) {
                        o << ", ";
                }
        }
        o << std::endl;

        for (size_t i = 0; i < min_size; ++i) { // go through lines
                for (auto cs : all()) { // go through counters. each line contains one record from each counter
                        cs.second.println(o, i);
                }

                o << std::endl;
        }
}
