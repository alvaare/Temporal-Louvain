#include "performance.hpp"
using namespace std;

void get_real_partition(const groundtruth_type& groundtruth, partition& real_partition) {
    unordered_map<string, community*> from_string_to_point;
    for (auto i : groundtruth) {
        int id_u = i.first;
        string comm_u = i.second;
        community* comm_point;
        if (from_string_to_point.find(comm_u)==from_string_to_point.end()) {
            comm_point = new community;
            real_partition.insert_community(comm_point);
            from_string_to_point[comm_u] = comm_point;
        }
        else {
            comm_point = from_string_to_point[comm_u];
        }
        real_partition.insert_pair(id_u, comm_point);
    }
}

double groundtruth_performance(const groundtruth_type& groundtruth, const weightedGraph& G) {
    partition real_partition;
    get_real_partition(groundtruth, real_partition);
    return modularity(real_partition, G);
}


//=============================================================================

bool from_same_class(int id_u, int id_v, const groundtruth_type& groundtruth) {
    return groundtruth.at(id_u) == groundtruth.at(id_v);
}

bool good_pair(int id_u, int id_v, const partition& classes, const groundtruth_type& groundtruth) {
    community* u_comm = classes.get_community(id_u);
    community* v_comm = classes.get_community(id_v);
    if (from_same_class(id_u, id_v, groundtruth))
        return u_comm == v_comm;
    else   
        return u_comm != v_comm;
}

double rand_index(const partition& classes, const groundtruth_type& groundtruth) {
    int res = 0;
    int n = groundtruth.size();
    for (auto u : groundtruth) {
        for (auto v : groundtruth) {
            if (good_pair(u.first, v.first, classes, groundtruth)) {
                res++;
            }
        }
    }
    return double(res)/(n*n);
}
