#include <iostream>
#include <string>
#include "graph.hpp"
#include "partition.hpp"
#include "louvain.hpp"
#include "temporal_louvain.hpp"
#include "log.hpp"
#include "sbm.hpp"
#include "performance.hpp"
using namespace std;

const string DATA_PATH = "data/";
const char DELIMITER = ',';

void test_louvain(const tempGraph& temp_G) {
    cout << "Construct Graph...\n";
    weightedGraph w_G = from_temp_to_weight(temp_G);
    cout << "End construct.\n";

    cout << "Start Louvain...\n";
    partition classes;
    louvain(w_G, classes);
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

void test_temp_louvain(const tempGraph& temp_G) {
    history H;
    temporal_louvain(H, temp_G);
    //H.print();
}


int main(int argc, char* argv[]) {
    string filename = argv[1];
    string filepath = DATA_PATH + filename + ".csv";
    cout << "Read file...\n";
    tempGraph temp_G = readTempGraph(filepath, DELIMITER);
    cout << "End read.\n";

    get_logfile();

    //tempGraph G;
    //tsbm(&G, 2, 100, 10000, 0.6, 0.1);
    history H;
    temporal_louvain_window(H, temp_G);

    //test_louvain(temp_G);
    
}