#include <iostream>
#include "louvain.hpp"
#include "temporal_louvain.hpp"
#include "log.hpp"
#include "performance.hpp"

int temporal_partition::get_begin() const {
    return begin;
}

int temporal_partition::get_end() const {
    return end;
}

int temporal_partition::get_duration() const {
    return end-begin;
}

void temporal_partition::set_begin(int date) {
    begin = date;
}

void temporal_partition::set_end(int date) {
    end = date;
}

void temporal_partition::print() const {
    cout << "Begin: " << begin << " End: " << end << "\n";
    partition::print();
}

const vector<temporal_partition*>& history::get_content() const {
    return content;
}

void history::insert_partition(temporal_partition* part) {
    content.push_back(part);
}

void history::print() const {
    cout << "Number of stages: " << content.size() << "\n";
    for (auto part : content) {
        part->print();
    }
}

int history::get_size() const {
    return content.size();
}

history::~history() {
    for (auto part : content) {
        delete part;
    }
}

//=============================================================================

void initialise_temp_partition(const tempGraph& G, temporal_partition& part, int begin) {
    for (auto id_u : G.get_nodes()) {
        community* comm_point = new community;
        comm_point->insert(id_u);
        part.insert_community(comm_point);
    }
    part.set_begin(begin);
}

void initialise_weighted_graph(weightedGraph& w_G, const tempGraph& G) {
    for (int id : G.get_nodes()) {
        w_G.add_node(id);
    }
}

int inter_community_connexion(const community& c1, const community& c2, const weightedGraph& G) {
    int res = 0;
    for (int id : c1) {
        res += relative_weight_node(id, c2, G);
    }
    return res;
}

int merge_inc(community& c1, community& c2, const weightedGraph& G) {
    int W1 = weight_community(c1, G);
    int W2 = weight_community(c2, G);
    int L12 = inter_community_connexion(c1, c2, G);
    int m = G.get_weight();
    return 2 * m * L12 - 2 * W1 * W2;
}

bool try_edge_is_coherent_simple(tempEdge e, const weightedGraph& G, partition& part) {
    int id_u = e.get_start();
    int id_v = e.get_end();
    community* comm_of_u = part.get_community(id_u);
    community* comm_of_v = part.get_community(id_v);
    if (comm_of_u == comm_of_v) {
        return true;
    }

    int inc_u_to_v = louvain_inc(id_u, *comm_of_v, part, G);
    int inc_v_to_u = louvain_inc(id_v, *comm_of_u, part, G);

    if (inc_u_to_v >= inc_v_to_u) {
        if (inc_u_to_v > 0) {
            part.change_community(id_u, comm_of_v);
            return true;
        }
        if (inc_u_to_v == 0) {
            return true;
        }
    }
    else if (inc_v_to_u > 0) {
        part.change_community(id_v, comm_of_u);
        return true;
    }
    else if (inc_u_to_v == 0) {
        return true;
    }
    return false;
}
//=============================================================================

void store_performance(const partition& present_part, const weightedGraph& w_G, const tempGraph& G) {
    double mod = modularity(present_part, w_G);
    double mod_orig = groundtruth_performance(G.get_groundtruth(), w_G);
    double rand_score = rand_index(present_part, G.get_groundtruth());
    write_log(to_string(mod)+ "\t" + to_string(mod_orig) + "\t" + to_string(rand_score) + "\n");
}

//=============================================================================

double rd() {
    return (double) rand() / RAND_MAX;
}

bool try_edge_is_coherent_pan(tempEdge e, weightedGraph& G, partition& part) {
    int id_u = e.get_start();
    int id_v = e.get_end();
    community* comm_of_u = part.get_community(id_u);
    community* comm_of_v = part.get_community(id_v);
    if (comm_of_u == comm_of_v) {
        return true;
    }
    if (comm_of_u->size() == 1 && comm_of_v->size() > 1) {
        if (rd() > 1 / G.weight_node(id_v) ) {
            return false;
        }
    }
    if (comm_of_v->size() == 1 && comm_of_u->size() > 1) {
        if (rd() > 1 / G.weight_node(id_u) ) {
            return false;
        }
    }

    int inc_u_to_v = louvain_inc(id_u, *comm_of_v, part, G);
    int inc_v_to_u = louvain_inc(id_v, *comm_of_u, part, G);

    if (inc_u_to_v >= inc_v_to_u) {
        if (inc_u_to_v > 0) {
            part.change_community(id_u, comm_of_v);
            return true;
        }
        if (inc_u_to_v == 0) {
            return true;
        }
    }
    else if (inc_v_to_u > 0) {
        part.change_community(id_v, comm_of_u);
        return true;
    }
    else if (inc_u_to_v == 0) {
        return true;
    }
    return false;
}

void temporal_louvain(history& H, const tempGraph& G) {
    weightedGraph w_G;
    temporal_partition* present_part = new temporal_partition;
    cout << "Begin initialisation\n";
    initialise_weighted_graph(w_G, G);
    initialise_temp_partition(G, *present_part, 0);
    cout << "End initialisation\n";
    int i = 0;
    int total = G.get_edges().size();
    for (auto e : G.get_edges()) {
        weightEdge w_e(e.get_start(), e.get_end(), 1);
        w_G.add_edge(w_e);
        if (try_edge_is_coherent_simple(e, w_G, *present_part)) {
            //cout << "Edge coherent!\n";
        }
        /*else {
            //cout << "Edge not coherent!\n";
            present_part->set_end(e.get_time());
            H->insert_partition(present_part);
            present_part = new temporal_partition;
            initialise_temp_partition(G, present_part, e.get_time());
            w_G.clear_edges();
        }*/ 
        store_performance(*present_part, w_G, G);
        cout << double(i)*100/total << "\n";
        i++;
    }
    for (auto comm : present_part->get_communities()) {
        for (auto id : *comm) {
            cout << id << " "<< G.get_community(id) << " ";
        }
        cout << "\n";
    }
    cout << modularity(*present_part, w_G) << "\n";
    H.insert_partition(present_part);
}

//=============================================================================

const int WIND_CONST = 40;

void initialize_louvain_window(partition* part, weightedGraph* G, tempGraph& t_G) {
    int n = t_G.size();
    int wind_size = n * WIND_CONST;
    int i = 0;
    for (auto e : t_G.get_edges()) {
        if (i >= wind_size) {
            break;
        }
        weightEdge w_e(e.get_start(), e.get_end(), 1);
        G->increase_weight(w_e);
        i++;
    }
    louvain(*G, part);
}

void window_iteration(weightedGraph& G, partition* part, weightEdge old_e, weightEdge new_e) {
    
}

void temporal_louvain_window(tempGraph& G) {
    weightedGraph w_G;
    partition* present_part = new partition;
    cout << "Begin initialisation\n";
    initialize_louvain_window(present_part, &w_G, G);
    cout << "End initialisation\n";
    int n = G.size();
    int wind_size = n * WIND_CONST;
    int i = 0;
    int total = G.get_edges().size()-wind_size;
    auto new_e = G.get_edges().begin();
    advance(new_e, wind_size);
    auto old_e = G.get_edges().begin();
    while (new_e != G.get_edges().end()) {
        weightEdge new_w_e(new_e->get_start(), new_e->get_end(), 1);
        w_G.increase_weight(new_w_e);
        weightEdge old_w_e(old_e->get_start(), old_e->get_end(), 1);
        w_G.decrease_weight(old_w_e);
        window_iteration(w_G, present_part, old_w_e, new_w_e);
        double mod = modularity(*present_part, w_G);
        write_log(to_string(mod)+"\n");
        cout << double(i)*100/total << "\n";
        i++;
        new_e++;
        old_e++;
    }
    for (auto comm : present_part->get_communities()) {
        for (auto id : *comm) {
            cout << id << " "<< G.get_community(id) << " ";
        }
        cout << "\n";
    }
    cout << modularity(*present_part, w_G) << "\n";
}