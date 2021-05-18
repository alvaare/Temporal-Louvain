#pragma once
#include "partition.hpp"
#include "graph.hpp"
#include "louvain.hpp"
#include "aynaud_guillaume.hpp"

double rand_index(const partition&, const groundtruth_type&);

double groundtruth_performance(const groundtruth_type&, const weightedGraph&);

double similarity(const snapshot&, const snapshot&);