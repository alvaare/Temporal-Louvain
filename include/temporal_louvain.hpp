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

const int WIND_CONST = 5;

class mod_tracker : public weightedGraph, public partition {
    protected:
        int links_sum = 0;
        int weights_squared_sum = 0;
        unordered_map<community*, int> community_weight;
        unordered_map<community*, int> community_links;
    public:
        double get_modularity() const;
        int get_weight(community*) const;
        int get_links(community*) const;
        void fill();
        int wind_size() const;
        void insert_edge(tempEdge);
        void erase_edge(tempEdge);
        void increase_weight(community*, int = 1);
        void increase_links(community*, int = 1);
        double mod_inc(community*);
        double mod_inc(community*, community*);
        double mod_inc(int, community*);
        void try_edge_is_coherent(tempEdge);
        void change_community(int, community*);
        int relative_weight_node(int, community*);
        int get_links_sum() const;
};

void temporal_louvain(history&, const tempGraph&);

void temporal_louvain_window(history&, const tempGraph&);

#endif