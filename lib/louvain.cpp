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

partition initialise_partition(weightedGraph& G) {
    partition classes;
    for (auto id_u : G.get_nodes()) {
        community* comm_point = new community;
        comm_point->insert(id_u);
        classes.insert_community(comm_point);
    }
    return classes;
}

//=============================================================================

int weight_community(community& C, weightedGraph& G) {
    int weight = 0;
    for (auto id : C) {
        weight += G.weight_node(id);
    }
    return weight;
}

int relative_weight_node(int id_u, community& C, weightedGraph& G) {
    int weight = 0;
    for (auto id_v : C) {
        weight += G.edge_weight(edge(id_u, id_v));
    }
    return weight;
}

double louvain_inc(int id_u, community* target_comm, partition* classes, weightedGraph& G) {
    int m = G.get_weight();
    community *comm_of_u = classes->get_community(id_u);
    int dcuu = relative_weight_node(id_u, *comm_of_u, G);
    int du = G.weight_node(id_u);
    int dcu = weight_community(*comm_of_u, G);
    int dcvu = relative_weight_node(id_u, *target_comm, G);
    int dcv = weight_community(*target_comm, G);
    return dcvu - dcuu + double(du) / (2 * m) * (dcu - dcv - du);
}

bool scan_node_louvain(partition* classes, weightedGraph& G, int id_u) {
    community* best_community;
    double best_inc = 0;
    for (auto v : G.get_neighbors(id_u)) {
        int id_v = v.first;
        community* comm_of_v = classes->get_community(id_v);
        double inc = louvain_inc(id_u, comm_of_v, classes, G);
        if (inc > best_inc) {
            best_inc = inc;
            best_community = comm_of_v;
        }
    }
    if (best_inc > 0) {
        classes->change_community(id_u, best_community);
        return true;
    }
    return false;
}

pair<partition, bool> louvain_iteration(weightedGraph& G) {
    bool some_movement = false;
    partition classes = initialise_partition(G);
    bool vertex_movement;
    do {
        vertex_movement = false;
        cout << modularity(classes, G) << "\n";
        for (auto id_u : G.get_nodes()) {
            if (scan_node_louvain(&classes, G, id_u)) {
                vertex_movement = true;
                some_movement = true;
            }
        }
    } while (vertex_movement);
    classes.print();
    cout << "Exiting louvain_iteration\n";
    pair<partition, bool> p = {classes, some_movement};
    return p;
}

//=============================================================================

struct id_dic {
    unordered_map<community*, int> from_comm_to_id;
    unordered_map<int, community*> from_id_to_comm;

    void insert_pair(community* comm, int id) {
        from_comm_to_id.insert({comm, id});
        from_id_to_comm.insert({id, comm});
    }

    int get_id(community* comm) {
        return from_comm_to_id[comm];
    }

    community* use_id(int id) {
        return from_id_to_comm[id];
    }

    void clear() {
        from_comm_to_id.clear();
        from_id_to_comm.clear();
    }
};

void create_nodes(partition& classes, weightedGraph *G, id_dic *dic) {
    int used_id = 0;
    for (auto comm : classes.get_communities()) {
        G->add_node(used_id);
        dic->insert_pair(comm, used_id);
        used_id++;
    }
}

int get_new_id(partition& classes, int id_u, id_dic& dic) {
    community* u_comm = classes.get_community(id_u);
    return dic.get_id(u_comm);
}

void create_edges(partition& classes, weightedGraph& G, weightedGraph* new_G, id_dic& dic) {
    for (auto id_u : G.get_nodes()) {
        int new_id_u = get_new_id(classes, id_u, dic);
        for (auto v : G.get_neighbors(id_u)) {
            int new_id_v = get_new_id(classes, v.first, dic);
            int uv_weight = v.second;
            weightEdge w_e(new_id_u, new_id_v, uv_weight);
            new_G->increase_weight(w_e);
        }
    }
}

weightedGraph weighted_graph_from_partition(partition& classes, weightedGraph& G, id_dic* dic){
    weightedGraph new_G;
    create_nodes(classes, &new_G, dic);
    create_edges(classes, G, &new_G, *dic);
    return new_G;
}

//=============================================================================

unordered_set<community*> get_communities_pointers(community *comm, id_dic &dic) {
    unordered_set<community*> communities;
    for (auto id : *comm) {
        communities.insert(dic.use_id(id));
    }
    return communities;
}

community* set_union(unordered_set<community*> &comms_to_union) {
    community* new_comm = new community;
    for (auto comm : comms_to_union) {
        new_comm->insert(comm->begin(), comm->end());
    }
    return new_comm;
}

void collapse_community(partition* classes, community* comm, id_dic& dic) {
    unordered_set<community*> comms_to_union = get_communities_pointers(comm, dic);
    community* new_comm = set_union(comms_to_union);
    classes->erase_communities(comms_to_union);
    classes->insert_community(new_comm);
}

void update_classes(partition* classes, partition& new_classes, id_dic& dic) {
    for (auto comm : new_classes.get_communities()) {
        collapse_community(classes, comm, dic);
    }
}
//=============================================================================

partition louvain(weightedGraph& G) {
    partition classes = initialise_partition(G);
    partition temp_classes = initialise_partition(G);
    weightedGraph temp_G = G;
    id_dic dic;
    bool some_movement;
    cout << "End initialisation\n";

    do {
        cout << "New iteration ";
        double mod = modularity(classes, G);
        cout << mod << "\n";

        temp_G = weighted_graph_from_partition(temp_classes, temp_G, &dic);
        cout << "End construct weight from partition\n";

        cout << "Begin louvain_iteration\n";
        tie(temp_classes, some_movement) = louvain_iteration(temp_G);
        cout << "End louvain_iteration\n";
        temp_classes.print();
        cout << "Begin update_classes\n";
        update_classes(&classes, temp_classes, dic);
        cout << "End update_classes\n";

        dic.clear();

    } while (false/*some_movement*/);

    return classes;
}