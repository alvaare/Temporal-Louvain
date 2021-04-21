#include <iostream>
#include <string>
#include "graph.hpp"
#include "partition.hpp"
#include "louvain.hpp"
using namespace std;

const string DATA_PATH = "data/";
const char DELIMITER = ',';


//=============================================================================

bool from_same_class(int id_u, int id_v, unordered_map<int, string>& groundtruth) {
    return groundtruth[id_u] == groundtruth[id_v];
}

bool good_pair(int id_u, int id_v, partition& classes, unordered_map<int, string>& groundtruth) {
    community* u_comm = classes.get_community(id_u);
    community* v_comm = classes.get_community(id_v);
    if (from_same_class(id_u, id_v, groundtruth))
        return u_comm == v_comm;
    else   
        return u_comm != v_comm;
}

double rand_index(partition& classes, unordered_map<int, string>& groundtruth) {
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

//=============================================================================

void get_real_partition(unordered_map<int, string>& groundtruth, partition* real_partition) {
    unordered_map<string, community*> from_string_to_point;
    for (auto i : groundtruth) {
        int id_u = i.first;
        string comm_u = i.second;
        community* comm_point;
        if (from_string_to_point.find(comm_u)==from_string_to_point.end()) {
            comm_point = new community;
            real_partition->insert_community(comm_point);
            from_string_to_point[comm_u] = comm_point;
        }
        else {
            comm_point = from_string_to_point[comm_u];
        }
        real_partition->insert_pair(id_u, comm_point);
    }
}

double groundtruth_performance(unordered_map<int, string>& groundtruth, weightedGraph& G) {
    partition real_partition;
    get_real_partition(groundtruth, &real_partition);
    return modularity(real_partition, G);
}

//=============================================================================

int main(int argc, char* argv[]) {
    string filename = argv[1];
    string filepath = DATA_PATH + filename + ".csv";
    cout << "Read file...\n";
    tempGraph temp_G = readTempGraph(filepath, DELIMITER);
    cout << "End read.\n";

    cout << "Construct Graph...\n";
    weightedGraph w_G = from_temp_to_weight(temp_G);
    cout << "End construct.\n";

    cout << "Start Louvain...\n";
    partition classes;
    louvain(w_G, &classes);
    cout << "End Louvain.\n";
    //classes.print();
    cout << classes.get_communities().size() << "\n";

    for (auto c : classes.get_communities()) {
            for (auto id : *c) {
                cout << id << " " << temp_G.get_community(id) << "\t";
            }
            cout << "\n";
    }
    cout << "Score: " << rand_index(classes, temp_G.get_groundtruth()) << "\n";
    cout << "Groundtruth modularity: " << groundtruth_performance(temp_G.get_groundtruth(), w_G) << "\n";
    cout << "Solution's modularity: " << modularity(classes, w_G) << "\n";
}