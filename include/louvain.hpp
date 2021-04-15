#ifndef __LOUVAIN_HPP_INCLUDED__
#define __LOUVAIN_HPP_INCLUDED__

#include "graph.hpp"
#include "partition.hpp"

partition louvain(weightedGraph&);

pair<partition, bool> louvain_iteration(weightedGraph& G);

#endif