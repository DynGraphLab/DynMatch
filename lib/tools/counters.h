#ifndef COUNTERS_H
#define COUNTERS_H

#include <vector>

#include "../definitions.h"
#include "macros_assertions.h"

class counters {
        public:
                struct counter {
                        bool restarted;

                        std::vector<size_t> records;
                        size_t iterator;

                        counter ();

                        void inc ();
                        void dec ();
                        void next ();
                        void next (size_t value);
                        void add (size_t value);
                        void put (size_t value);
                        size_t get ();
                        std::vector<size_t>& all ();

                        void reset ();
                        void restart ();
                };

                struct counter_d {
                        bool restarted;

                        std::vector<double> records;
                        size_t iterator;

                        counter_d ();

                        void inc ();
                        void dec ();
                        void next ();
                        void next (double value);
                        void add (double value);
                        void put (double value);
                        double get ();
                        std::vector<double>& all ();

                        void reset ();
                        void restart ();
                };

                struct counter_set {
                        std::vector<std::pair<std::string, counter> > registered_counters;
                        std::vector<std::pair<std::string, counter_d> > registered_counters_d;

                        std::vector<std::pair<std::string, counter> > all ();
                        counter& create (const std::string& name);
                        bool exists (const std::string& name);
                        counter& get(const std::string& name);
                        void divide_by (size_t divisor);

                        std::vector<std::pair<std::string, counter_d> > all_d ();
                        counter_d& create_d (const std::string& name);
                        bool exists_d (const std::string& name);
                        counter_d& get_d(const std::string& name);
                        void divide_by_d (double divisor);

                        void println (std::ostream& o, size_t i);
                };

                static std::vector<std::pair<std::string, counter_set> > all();
                static counter_set& create (const std::string& name);
                static bool exists (const std::string& name);
                static counter_set& get(const std::string& name);

                static void divide_by (size_t divisor);
                static void divide_by_d (double divisor);

                static void print_names(std::ostream& o);
                static void print(std::ostream& o);

        private:
                static std::vector<std::pair<std::string, counter_set> > counter_sets;
};

#endif // COUNTERS_H
