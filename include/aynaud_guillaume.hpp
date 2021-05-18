#pragma once
#include "graph.hpp"
#include "partition.hpp"
#include <vector>
#include <unordered_set>
using namespace std;

typedef pair<int, int> interval;

class snapshot : public weightedGraph, public partition {
    protected:
        vector<interval> time;
    public:
        void insert_interval(interval);
        const vector<interval>& get_time() const;
};

void aynaud_guillaume(const tempGraph&);