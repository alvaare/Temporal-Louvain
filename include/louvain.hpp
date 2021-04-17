#ifndef __LOUVAIN_HPP_INCLUDED__
#define __LOUVAIN_HPP_INCLUDED__

#include "graph.hpp"
#include "partition.hpp"

void louvain(weightedGraph&, partition*);

bool louvain_iteration(weightedGraph&, partition*);

#endif