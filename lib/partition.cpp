#include <iostream>
#include "partition.hpp"
using namespace std;

void print_community(community& comm) {
    for (auto id : comm) {
        cout << id << " ";
    }
    cout << "\n";
}

partition::~partition() {
    clear();
}

void partition::clear() {
    while (!communities.empty()) {
        auto comm = communities.begin();
        delete *comm;
        communities.erase(comm);
    }
    community_of_node.clear();
}

void partition::print() {
    for (auto c : communities) {
        print_community(*c);
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
        community_of_node.erase(id);
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

void partition::insert_pair(int id, community* comm) {
    community_of_node[id] = comm;
    comm->insert(id);
}

void partition::change_community(int id_u, community* comm) {
    community* old_comm = get_community(id_u);
    old_comm->erase(id_u);
    comm->insert(id_u);
    community_of_node[id_u] = comm;
    if (old_comm->empty()) {
        erase_community(old_comm);
    }
}

void partition::merge_communities(community* c1, community* c2) {
    community* new_comm = new community;
    for (int id : *c1) {
        new_comm->insert(id);
        community_of_node[id] = new_comm;
    }
    for (int id : *c2) {
        new_comm->insert(id);
        community_of_node[id] = new_comm;
    }
    communities.insert(new_comm);
    communities.erase(c1);
    communities.erase(c2);
    delete c1;
    delete c2;
}