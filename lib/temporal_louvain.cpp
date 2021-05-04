#include <iostream>
#include "louvain.hpp"
#include "temporal_louvain.hpp"
#include "log.hpp"

int temporal_partition::get_begin() {
    return begin;
}

int temporal_partition::get_end() {
    return end;
}

int temporal_partition::get_duration() {
    return end-begin;
}

void temporal_partition::set_begin(int date) {
    begin = date;
}

void temporal_partition::set_end(int date) {
    end = date;
}

void temporal_partition::print() {
    cout << "Begin: " << begin << " End: " << end << "\n";
    partition::print();
}

vector<temporal_partition*>& history::get_content() {
    return content;
}

void history::insert_partition(temporal_partition* part) {
    content.push_back(part);
}

void history::print() {
    cout << "Number of stages: " << content.size() << "\n";
    for (auto part : content) {
        part->print();
    }
}

int history::get_size() {
    return content.size();
}

history::~history() {
    for (auto part : content) {
        delete part;
    }
}

//=============================================================================

void initialise_temp_partition(tempGraph& G, temporal_partition* part, int begin) {
    for (auto id_u : G.get_nodes()) {
        community* comm_point = new community;
        comm_point->insert(id_u);
        part->insert_community(comm_point);
    }
    part->set_begin(begin);
}

void initialise_weighted_graph(weightedGraph* w_G, tempGraph& G) {
    for (int id : G.get_nodes()) {
        w_G->add_node(id);
    }
}

int inter_community_connexion(community& c1, community& c2, weightedGraph& G) {
    int res = 0;
    for (int id : c1) {
        res += relative_weight_node(id, c2, G);
    }
    return res;
}

int merge_inc(community& c1, community& c2, weightedGraph& G) {
    int W1 = weight_community(c1, G);
    int W2 = weight_community(c2, G);
    int L12 = inter_community_connexion(c1, c2, G);
    int m = G.get_weight();
    return 2 * m * L12 - W1 * W2;
}

bool try_edge_is_coherent(tempEdge e, weightedGraph& G, temporal_partition* part) {
    int id_u = e.get_start();
    int id_v = e.get_end();
    community* comm_of_u = part->get_community(id_u);
    community* comm_of_v = part->get_community(id_v);
    if (comm_of_u == comm_of_v) {
        return true;
    }

    int inc_u_to_v = louvain_inc(id_u, comm_of_v, part, G);
    int inc_v_to_u = louvain_inc(id_v, comm_of_u, part, G);
    int inc_merge = merge_inc(*comm_of_u, *comm_of_v, G);

    if (inc_u_to_v >= inc_v_to_u && inc_u_to_v >= inc_merge) {
        if (inc_u_to_v > 0) {
            part->change_community(id_u, comm_of_v);
            return true;
        }
        if (inc_u_to_v == 0) {
            return true;
        }
    }
    else if (inc_v_to_u >= inc_merge) {
        if (inc_v_to_u > 0) {
            part->change_community(id_v, comm_of_u);
            return true;
        }
        else if (inc_u_to_v == 0) {
            return true;
        }
    }
    else if (inc_merge > 0) {
        part->merge_communities(comm_of_u, comm_of_v);
        return true;
    }
    else if (inc_merge >=  0) {
        return true;
    }
    return false;
}

void temporal_louvain(history* H, tempGraph& G) {
    weightedGraph w_G;
    temporal_partition* present_part = new temporal_partition;
    cout << "Begin initialisation\n";
    initialise_weighted_graph(&w_G, G);
    initialise_temp_partition(G, present_part, 0);
    cout << "End initialisation\n";

    for (auto e : G.get_edges()) {
        weightEdge w_e(e.get_start(), e.get_end(), 1);
        w_G.increase_weight(w_e);
        if (try_edge_is_coherent(e, w_G, present_part)) {
            //cout << "Edge coherent!\n";
        }
        else {
            //cout << "Edge not coherent!\n";
            present_part->set_end(e.get_time());
            H->insert_partition(present_part);
            present_part = new temporal_partition;
            initialise_temp_partition(G, present_part, e.get_time());
            w_G.clear_edges();
        }
    }
    H->insert_partition(present_part);
}