#ifndef __TEMPORAL_LOUVAIN_HPP_INCLUDED__
#define __TEMPORAL_LOUVAIN_HPP_INCLUDED__

#include <vector>
#include "partition.hpp"
#include "graph.hpp"

class temporal_partition : public partition {
    protected:
        int begin;
        int end;
    public:
        int get_begin();
        int get_end();
        int get_duration();
        void set_begin(int);
        void set_end(int);
};

class history {
    protected:
        vector<temporal_partition*> content;
    public:
        ~history();
        vector<temporal_partition*>& get_content();
        void insert_partition(temporal_partition*);
};

void temporal_louvain(history*, tempGraph&);

#endif