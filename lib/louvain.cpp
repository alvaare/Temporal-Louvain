#include <iostream>
#include "graph.hpp"
#include "partition.hpp"
#include "louvain.hpp"
using namespace std;

double modularity(partition& classes, weightedGraph& G) {
    double q = 0;
    int m = G.get_weight();
    for (auto id_u : G.get_nodes()) {
        int du = G.weight_node(id_u);
        community* u_comm = classes.get_community(id_u);
        for (auto id_v : G.get_nodes()) {
            int dv = G.weight_node(id_v);
            community *v_comm = classes.get_community(id_v);
            if (u_comm == v_comm) {
                int uv_weight = G.edge_weight(edge(id_u, id_v));
                q += uv_weight;
                q -= double(du) * dv / (2 * m);
            }
        }
    }
    return q / (2 * m);
}

//=============================================================================

void initialise_partition(const weightedGraph& G, partition& classes) {
    for (auto id_u : G.get_nodes()) {
        community* comm_point = new community;
        comm_point->insert(id_u);
        classes.insert_community(comm_point);
    }
}

//=============================================================================

int weight_community(const community& C, const weightedGraph& G) {
    int weight = 0;
    for (auto id : C) {
        weight += G.weight_node(id);
    }
    return weight;
}

int relative_weight_node(int id_u, const community& C, const weightedGraph& G) {
    int weight = 0;
    for (auto id_v : C) {
        weight += G.edge_weight(edge(id_u, id_v));
    }
    return weight;
}

int louvain_inc(int id_u, const community& target_comm, const partition& classes, const weightedGraph& G) {
    int m = G.get_weight();
    community *comm_of_u = classes.get_community(id_u);
    int dcuu = relative_weight_node(id_u, *comm_of_u, G);
    int du = G.weight_node(id_u);
    int dcu = weight_community(*comm_of_u, G);
    int dcvu = relative_weight_node(id_u, target_comm, G);
    int dcv = weight_community(target_comm, G);
    return 2 * m * (dcvu - dcuu + G.edge_weight(edge(id_u, id_u)) ) + double(du) * (dcu - dcv - du);
}

bool scan_node_louvain(partition& classes, const weightedGraph& G, int id_u) {
    community* best_community = nullptr;
    int best_inc = 0;
    community* comm_of_u = classes.get_community(id_u);
    for (auto const& v : G.get_neighbors(id_u)) {
        int id_v = v.first;
        community* comm_of_v = classes.get_community(id_v);
        if (comm_of_u != comm_of_v) {
            int inc = louvain_inc(id_u, *comm_of_v, classes, G);
            if (inc > best_inc) {
                best_inc = inc;
                best_community = comm_of_v;
            }
        }
    }
    if (best_inc > 0) {
        classes.change_community(id_u, best_community);
        return true;
    }
    return false;
}

bool louvain_iteration(const weightedGraph& G, partition& classes) {
    bool some_movement = false;
    initialise_partition(G, classes);
    bool vertex_movement;
    do {
        vertex_movement = false;
        for (auto id_u : G.get_nodes()) {
            if (scan_node_louvain(classes, G, id_u)) {
                vertex_movement = true;
                some_movement = true;
            }
        }
    } while (vertex_movement);
    return some_movement;
}

//=============================================================================

struct id_dic {
    unordered_map<community*, int> from_comm_to_id;
    unordered_map<int, community*> from_id_to_comm;

    void insert_pair(community* comm, int id) {
        from_comm_to_id.insert({comm, id});
        from_id_to_comm.insert({id, comm});
    }

    int get_id(community* comm) const {
        return from_comm_to_id.at(comm);
    }

    community* use_id(int id) const {
        return from_id_to_comm.at(id);
    }

    void print() const {
        for (auto i : from_id_to_comm) {
            cout << i.first << " " << i.second << "\n";
        }
        cout << "\n";
    }

    void clear() {
        from_comm_to_id.clear();
        from_id_to_comm.clear();
    }
};

void get_communities_pointers(community& comm, const id_dic& dic, unordered_set<community*>& communities) {
    for (auto id : comm) {    
        communities.insert(dic.use_id(id));
    }
}

community* set_union(unordered_set<community*>& comms_to_union) {
    community* new_comm = new community;
    for (auto comm : comms_to_union) {
        new_comm->insert(comm->begin(), comm->end());
    }
    return new_comm;
}

community* collapse_community(partition& classes, community& comm, const id_dic& dic) {
    unordered_set<community*> comms_to_union;
    get_communities_pointers(comm, dic, comms_to_union);
    community* new_comm = set_union(comms_to_union);
    classes.erase_communities(comms_to_union);
    classes.insert_community(new_comm);
    return new_comm;
}

void create_nodes(const partition& new_classes, weightedGraph& G, partition& classes, const id_dic& old_dic, id_dic& dic_classes, id_dic& dic_new_classes) {
    int used_id = 0;
    for (auto comm : new_classes.get_communities()) {
        G.add_node(used_id);
        community* new_comm = collapse_community(classes, *comm, old_dic);
        dic_classes.insert_pair(new_comm, used_id);
        dic_new_classes.insert_pair(comm, used_id);
        used_id++;
    }
}

int get_new_id(const partition& classes, int id_u, const id_dic& dic) {
    community* u_comm = classes.get_community(id_u);
    return dic.get_id(u_comm);
}

void create_edges(const partition& classes, const weightedGraph& G, weightedGraph& new_G, const id_dic& dic) {
    for (auto id_u : G.get_nodes()) {
        int new_id_u = get_new_id(classes, id_u, dic);
        for (auto v : G.get_neighbors(id_u)) {
            if (id_u <= v.first) {
                int new_id_v = get_new_id(classes, v.first, dic);
                int uv_weight = v.second;
                weightEdge w_e(new_id_u, new_id_v, uv_weight);
                new_G.add_edge(w_e);
            }
        }
    }
}

void update(weightedGraph& G, const partition& new_classes, partition& classes, id_dic& dic) {
    weightedGraph new_G;
    id_dic dic_classes;
    id_dic dic_new_classes;

    create_nodes(new_classes, new_G, classes, dic, dic_classes, dic_new_classes);
    create_edges(new_classes, G, new_G, dic_new_classes);

    dic = dic_classes;
    G = new_G;
}

//=============================================================================

void create_new_nodes(weightedGraph& temp_G, const partition& classes, id_dic& dic) {
    int used_id = 0;
    for (auto comm : classes.get_communities()) {
        temp_G.add_node(used_id);
        dic.insert_pair(comm, used_id);
        used_id++;
    }   
}

void initialise_temp_G(weightedGraph& w_G, const weightedGraph& G, const partition& classes, id_dic& dic) {
    create_new_nodes(w_G, classes, dic);
    create_edges(classes, G, w_G, dic);
}

//=============================================================================

void louvain(const weightedGraph& G, partition& classes) {
    partition temp_classes;
    bool some_movement = louvain_iteration(G, classes);
    id_dic dic;
    weightedGraph w_G;
    initialise_temp_G(w_G, G, classes, dic);

    while (some_movement) {
        temp_classes.clear();
        some_movement = louvain_iteration(w_G, temp_classes);

        update(w_G, temp_classes, classes, dic);
    }
}