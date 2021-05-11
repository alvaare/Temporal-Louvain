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
        int get_begin() const;
        int get_end() const;
        int get_duration() const;
        void set_begin(int);
        void set_end(int);
        void print() const;
};

class history {
    protected:
        vector<temporal_partition*> content;
    public:
        ~history();
        const vector<temporal_partition*>& get_content() const;
        void insert_partition(temporal_partition*);
        void print() const;
        int get_size() const;
};

void temporal_louvain(history&, const tempGraph&);


#endif