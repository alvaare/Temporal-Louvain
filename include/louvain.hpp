#ifndef __LOUVAIN_HPP_INCLUDED__
#define __LOUVAIN_HPP_INCLUDED__

#include "graph.hpp"
#include "partition.hpp"

void louvain(const weightedGraph&, partition&);

bool louvain_iteration(const weightedGraph&, partition&);

double modularity(const partition&, const weightedGraph&);

int louvain_inc(int, const community&, const partition&, const weightedGraph&);

int weight_community(const community&, const weightedGraph&);

int relative_weight_node(int, const community&, const weightedGraph&);

bool scan_node_louvain(partition&, const weightedGraph&, int);

#endif