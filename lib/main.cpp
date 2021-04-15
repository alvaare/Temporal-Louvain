#include <iostream>
#include <string>
#include "graph.hpp"
#include "partition.hpp"
#include "louvain.hpp"
using namespace std;

const string DATA_PATH = "data/";

/*
//=============================================================================

bool from_same_class(int id_u, int id_v, map<int, string>& groundtruth) {
    return groundtruth[id_u] == groundtruth[id_v];
}

bool good_pair(pair<const int, community*>& u, pair<const int, community*>& v, map<int, string>& groundtruth) {
    int id_u = u.first;
    int id_v = v.first;
    if (from_same_class(id_u, id_v, groundtruth))
        return u.second == v.second;
    else   
        return u.second != v.second;
}

double rand_index(partition* classes, map<int, string> groundtruth) {
    int res = 0;
    int n = groundtruth.size();
    for (auto u : classes->community_of_node) {
        for (auto v : classes->community_of_node) {
            if (good_pair(u, v, groundtruth)) {
                res++;
            }
        }
    }
    return double(res)/(n*n);
}

//=============================================================================

void get_real_partition(map<int, string>& groundtruth, partition* real_partition) {
    unordered_map<string, community*> from_string_to_point;
    for (auto i : groundtruth) {
        int id_u = i.first;
        string comm_u = i.second;
        community* comm_point;
        if (from_string_to_point.find(comm_u)==from_string_to_point.end()) {
            comm_point = new community;
            real_partition->communities.insert(comm_point);
            from_string_to_point[comm_u] = comm_point;
        }
        else {
            comm_point = from_string_to_point[comm_u];
        }
        real_partition->community_of_node[id_u] = comm_point;
        comm_point->insert(id_u);
    }
}

double groundtruth_performance(map<int, string>& groundtruth, weightedGraph* G) {
    partition real_partition;
    get_real_partition(groundtruth, &real_partition);
    return modularity(&real_partition, G);
}


*/
//=============================================================================

int main(int argc, char* argv[]) {
    string filename = argv[1];
    string filepath = DATA_PATH + filename + ".csv";
    cout << "Read file...\n";
    tempGraph temp_G = readTempGraph(filepath);
    cout << "End read.\n";

    cout << "Construct Graph...\n";
    weightedGraph w_G = from_temp_to_weight(temp_G);
    cout << "End construct.\n";

    cout << "Start Louvain...\n";
    partition classes = louvain(w_G);
    cout << "End Louvain.\n";
    /*//classes.print();

    for (auto c : classes.communities) {
            for (auto id : *c) {
                cout << id << " " << temp_G.groundtruth[id] << "\t";
            }
            if (!c->empty())
                cout << "\n";
        }

    cout << "Score: " << rand_index(&classes, temp_G.groundtruth) << "\n";
    cout << groundtruth_performance(temp_G.groundtruth, &w_G) << "\n";*/
}