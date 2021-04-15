#include <iostream>
#include "partition.hpp"
using namespace std;

partition::~partition() {
    cout << "entering destructor of partition\n";
    print();
    cout << "End printing\n";
    while (!communities.empty()) {
        auto comm = communities.begin();
        cout << *comm << "\n";
        delete *comm;
        communities.erase(comm);
    }
    community_of_node.clear();
}

void partition::print() {
    for (auto c : communities) {
        for (auto id : *c) {
            cout << id << " ";
        }
        if (!c->empty()) {
            cout << "\n";
        }
    }
}

unordered_set<community*>& partition::get_communities() {
    return communities;
}

void partition::insert_community(community* comm) {
    communities.insert(comm);
    for (auto id : *comm) {
        community_of_node[id] = comm;
    }
}

void partition::erase_community(community* comm) {
    communities.erase(comm);
    for (auto id : *comm) {
        community_of_node[id] = nullptr;
    }
    delete comm;
}

void partition::erase_communities(unordered_set<community*>& comm_set) {
    for (auto comm : comm_set) {
        erase_community(comm);
    }
}

community* partition::get_community(int id) {
    return community_of_node[id];
}

void partition::change_community(int id_u, community* comm) {
    community* old_comm = get_community(id_u);
    old_comm->erase(id_u);
    comm->insert(id_u);
    community_of_node[id_u] = comm;
    if (old_comm->empty()) {
        communities.erase(old_comm);
        delete old_comm;
    }
}