#pragma once
#include "partition.hpp"
#include "graph.hpp"
#include "louvain.hpp"

double rand_index(const partition&, const groundtruth_type&);

double groundtruth_performance(const groundtruth_type&, const weightedGraph&);