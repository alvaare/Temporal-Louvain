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

void initialise_partition(weightedGraph& G, partition* classes) {
    for (auto id_u : G.get_nodes()) {
        community* comm_point = new community;
        comm_point->insert(id_u);
        classes->insert_community(comm_point);
    }
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

int louvain_inc(int id_u, community* target_comm, partition* classes, weightedGraph& G) {
    int m = G.get_weight();
    community *comm_of_u = classes->get_community(id_u);
    int dcuu = relative_weight_node(id_u, *comm_of_u, G);
    int du = G.weight_node(id_u);
    int dcu = weight_community(*comm_of_u, G);
    int dcvu = relative_weight_node(id_u, *target_comm, G);
    int dcv = weight_community(*target_comm, G);
    return 4 * m * (dcvu - dcuu + G.edge_weight(edge(id_u, id_u)) ) + double(du) * (dcu - dcv - du);
}

bool scan_node_louvain(partition* classes, weightedGraph& G, int id_u) {
    community* best_community = nullptr;
    int best_inc = 0;
    community* comm_of_u = classes->get_community(id_u);
    for (auto v : G.get_neighbors(id_u)) {
        int id_v = v.first;
        community* comm_of_v = classes->get_community(id_v);
        if (comm_of_u != comm_of_v) {
            int inc = louvain_inc(id_u, comm_of_v, classes, G);
            if (inc > best_inc) {
                best_inc = inc;
                best_community = comm_of_v;
            }
        }
    }
    if (best_inc > 0) {
        classes->change_community(id_u, best_community);
        return true;
    }
    return false;
}

bool louvain_iteration(weightedGraph& G, partition* classes) {
    bool some_movement = false;
    initialise_partition(G, classes);
    bool vertex_movement;
    do {
        vertex_movement = false;
        cout << modularity(*classes, G) << "\n";
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

    int get_id(community* comm) {
        return from_comm_to_id[comm];
    }

    community* use_id(int id) {
        return from_id_to_comm[id];
    }

    void print() {
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

void get_communities_pointers(community* comm, id_dic& dic, unordered_set<community*>* communities) {
    for (auto id : *comm) {    
        communities->insert(dic.use_id(id));
    }
}

community* set_union(unordered_set<community*> &comms_to_union) {
    community* new_comm = new community;
    for (auto comm : comms_to_union) {
        new_comm->insert(comm->begin(), comm->end());
    }
    return new_comm;
}

community* collapse_community(partition* classes, community* comm, id_dic& dic) {
    unordered_set<community*> comms_to_union;
    get_communities_pointers(comm, dic, &comms_to_union);
    community* new_comm = set_union(comms_to_union);
    classes->erase_communities(comms_to_union);
    classes->insert_community(new_comm);
    return new_comm;
}

void create_nodes(partition& new_classes, weightedGraph *G, partition* classes, id_dic& old_dic, id_dic* new_dic) {
    int used_id = 0;
    for (auto comm : new_classes.get_communities()) {
        G->add_node(used_id);
        community* new_comm = collapse_community(classes, comm, old_dic);
        new_dic->insert_pair(new_comm, used_id);
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
            if (id_u <= v.first) {
                int new_id_v = get_new_id(classes, v.first, dic);
                int uv_weight = v.second;
                weightEdge w_e(new_id_u, new_id_v, uv_weight);
                new_G->increase_weight(w_e);
            }
        }
    }
}

weightedGraph weighted_graph_from_partition(partition& new_classes, weightedGraph& G, partition* classes, id_dic* dic){
    weightedGraph new_G;
    id_dic new_dic;
    create_nodes(new_classes, &new_G, classes, *dic, &new_dic);
    create_edges(new_classes, G, &new_G, *dic);
    *dic = new_dic;
    return new_G;
}

//=============================================================================



void update_classes(partition* classes, partition& new_classes, id_dic& dic) {
    for (auto comm : new_classes.get_communities()) {
        collapse_community(classes, comm, dic);
    }
}

void update(weightedGraph* G, partition& new_classes, partition* classes, id_dic* dic) {

}

void initialisation(weightedGraph& G, partition* classes, partition* new_classes, weightedGraph* new_G, id_dic* dic) {

}

void create_new_nodes(weightedGraph* temp_G, partition& classes, id_dic* dic) {
    int used_id = 0;
    for (auto comm : classes.get_communities()) {
        temp_G->add_node(used_id);
        dic->insert_pair(comm, used_id);
        used_id++;
    }   
}

void initialise_temp_G(weightedGraph* temp_G, weightedGraph& G, partition& classes, id_dic* dic) {
    create_new_nodes(temp_G, classes, dic);
    create_edges(classes, G, temp_G, *dic);
}

//=============================================================================

void louvain(weightedGraph& G, partition* classes) {
    partition temp_classes;
    bool some_movement = louvain_iteration(G, classes);
    id_dic dic;
    weightedGraph temp_G;
    initialise_temp_G(&temp_G, G, *classes, &dic);
    //temp_G.print();
    cout << temp_G.get_nodes().size() << " " << G.get_nodes().size() << "\n";
/*
    while (some_movement) {
        cout << "New iteration ";
        double mod = modularity(*classes, G);
        cout << mod << "\n";

        //dic = make_dic_&_update_classe(temp_classes, classes)

        cout << "Begin update\n";
        update(&temp_G, temp_classes, classes, &dic);
        cout << "End update\n";

        temp_classes.clear();
        some_movement = louvain_iteration(temp_G, &temp_classes);
    }

    do {
        cout << "New iteration ";
        double mod = modularity(*classes, G);
        cout << mod << "\n";

        //temp_G = weighted_graph_from_partition(temp_classes, temp_G, classes, &dic);
        cout << "End construct weight from partition\n";
        temp_G.print();

        cout << "Begin louvain_iteration\n";
        temp_classes.clear();
        some_movement = louvain_iteration(temp_G, &temp_classes);
        temp_classes.print();
        cout << "End louvain_iteration\n";

        cout << "Begin update_classes\n";
        //update_classes(classes, temp_classes, dic_orig_graph);
        cout << "End update_classes\n";

        //dic.clear();

    } while (false/*some_movement/);*/

}