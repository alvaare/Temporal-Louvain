#ifndef __LOUVAIN_HPP_INCLUDED__
#define __LOUVAIN_HPP_INCLUDED__

#include "graph.hpp"
#include "partition.hpp"

void louvain(weightedGraph&, partition*);

bool louvain_iteration(weightedGraph&, partition*);

double modularity(partition&, weightedGraph&);

int louvain_inc(int, community*, partition*, weightedGraph&);

int weight_community(community&, weightedGraph&);

int relative_weight_node(int, community&, weightedGraph&);

#endif