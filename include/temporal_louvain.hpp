#ifndef __TEMPORAL_LOUVAIN_HPP_INCLUDED__
#define __TEMPORAL_LOUVAIN_HPP_INCLUDED__

#include <vector>
#include "partition.hpp"
#include "graph.hpp"

class temporal_partition : public partition {
    protected:
        int begin = 0;
        int end = 0;
    public:
        int get_begin();
        int get_end();
        int get_duration();
        void set_begin(int);
        void set_end(int);
        void print();
};

class history {
    protected:
        vector<temporal_partition*> content;
    public:
        ~history();
        vector<temporal_partition*>& get_content();
        void insert_partition(temporal_partition*);
        void print();
        int get_size();
};

void temporal_louvain(history*, tempGraph&);

#endif