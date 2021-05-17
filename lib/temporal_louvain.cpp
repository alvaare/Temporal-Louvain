#include <iostream>
#include <vector>
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

void initialize_louvain_window(partition& part, weightedGraph& G, const tempGraph& t_G) {
    for (auto id : t_G.get_nodes()) {
        G.add_node(id);
    }
    int n = t_G.size();
    int wind_size = n * WIND_CONST;
    int i = 0;
    for (auto e : t_G.get_edges()) {
        if (i >= wind_size) {
            break;
        }
        weightEdge w_e(e.get_start(), e.get_end(), 1);
        G.add_edge(w_e);
        i++;
    }
    louvain(G, part);
}

void window_update_old(weightEdge old_e, weightedGraph& G, partition& part) {
    int id_u = old_e.get_start();
    int id_v = old_e.get_end();
    community* u_comm = part.get_community(id_u);
    community* v_comm = part.get_community(id_v);

    if (u_comm != v_comm) {
        return;
    }

    scan_node_louvain(part, G, id_u);
    scan_node_louvain(part, G, id_v);
}
/*
void window_iteration(weightedGraph& G, partition& part, weightEdge old_e, weightEdge new_e) {
    tempEdge new_t_e(new_e.get_start(), new_e.get_end(), 0);
    try_edge_is_coherent_simple(new_t_e, G, part);
    window_update_old(old_e, G, part);
} */
/*
void temporal_louvain_window(history& H, const tempGraph& G) {
    weightedGraph w_G;
    temporal_partition* present_part = new temporal_partition;
    H.insert_partition(present_part);
    cout << "Begin initialisation\n";
    initialize_louvain_window(*present_part, w_G, G);
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
        w_G.add_edge(new_w_e);
        weightEdge old_w_e(old_e->get_start(), old_e->get_end(), 1);
        w_G.decrease_weight(old_w_e);
        window_iteration(w_G, *present_part, old_w_e, new_w_e);
        //store_performance(*present_part, w_G, G);
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
*/
//=============================================================================

const double CRITICAL_RATIO = 0.9;

void initialise_window(mod_tracker& status, const tempGraph& G, int i_start = 0) {
    for (auto id : G.get_nodes()) {
        status.add_node(id);
    }
    int i = i_start;
    for (auto e : G.get_edges()) {
        weightEdge w_e(e.get_start(), e.get_end(), 1);
        status.add_edge(w_e);
        i++;
        if (i >= i_start+status.wind_size()) {
            break;
        }
    }
    louvain(status, status);
    status.fill();
}

void window_iteration(mod_tracker& status, int i_edge, const tempGraph& G) {
    tempEdge new_e = G.get_temp_edge(i_edge);
    tempEdge old_e = G.get_temp_edge(i_edge-status.wind_size());
    new_e.print();
    old_e.print();
    status.insert_edge(new_e);
    status.erase_edge(old_e);
    status.try_edge_is_coherent(new_e);
}

void temporal_louvain_window(history& H, const tempGraph& G) {
    mod_tracker* status = new mod_tracker;
    initialise_window(*status, G);
    double max_mod = status->get_modularity();
    int i_edge = status->wind_size();
    int total = G.get_edges().size();
    vector<string> separators;
    vector<string> modularity_history;
    while (i_edge < G.nb_edges()) {
        window_iteration(*status, i_edge, G);
        double mod = status->get_modularity();
        modularity_history.push_back(to_string(mod));
        if (mod > max_mod) {
            max_mod = mod;
        }
        if (mod < CRITICAL_RATIO * max_mod) {
            int time = G.get_temp_edge(i_edge).get_time();
            cout << time << " " << i_edge << "\n";
            separators.push_back(to_string(time));
            delete status;
            
            status = new mod_tracker;
            initialise_window(*status, G, i_edge);
            i_edge += status->wind_size();
            max_mod = status->get_modularity();
        }
        cout << double(i_edge)/total*100 << " " << " " << "\n";
        i_edge++;
    }
    write_log(separators);
    write_log("\n");
    write_log(modularity_history); 
    delete status;
}

//=============================================================================

int mod_tracker::get_links_sum() const {
    int r = 0;
    for (auto c : get_communities()) {
        for (auto id_u : *c) {
            for (auto id_v : *c) {
                r += edge_weight(edge(id_u, id_v));
            }
        }
    }
    return r;
}

double mod_tracker::get_modularity() const {
    int m = weightedGraph::get_weight();
    //cout << get_links_sum() << " " << links_sum << "\n";
    /*if (get_links_sum()!=links_sum) {
        weightedGraph::print();
        partition::print();
        exit(0);
    }*/
    return 1.0/(2*m)*(links_sum-1.0/(2*m)*weights_squared_sum);
}

int mod_tracker::get_weight(community* c) const {
    return community_weight.at(c);
}

int mod_tracker::get_links(community* c) const {
    return community_links.at(c);
}

void mod_tracker::fill() {
    for (auto c : get_communities()) {
        int weight = 0;
        int links = 0;
        for (auto id_u : *c) {
            weight += weight_node(id_u);
            for (auto id_v : *c) {
                links += edge_weight(edge(id_u, id_v));
            }
        }
        community_links.insert({c, links});
        community_weight.insert({c, weight});
        links_sum += links;
        weights_squared_sum += weight*weight;
    }
}

int mod_tracker::wind_size() const {
    return size() * WIND_CONST;
}

void mod_tracker::insert_edge(tempEdge e) {
    add_edge(e);
    int id_u = e.get_start();
    int id_v = e.get_end();
    community* u_comm = get_community(id_u);
    community* v_comm = get_community(id_v);
    increase_weight(u_comm);
    increase_weight(v_comm);

    if (u_comm == v_comm) {
        increase_links(u_comm, 2);
    }
}

void mod_tracker::increase_weight(community* c, int k) { 
    weights_squared_sum += 2*k*get_weight(c) + k*k;
    community_weight[c] += k;
}

void mod_tracker::increase_links(community* c, int k) {
    links_sum += k;
    community_links[c] += k;
}
/*
double mod_tracker::mod_inc(community* c_0) {
    int m = weightedGraph::get_weight();
    int w_0 = get_weight(c_0);
    return 1.0/(m+1) - (w_0+1.0)/((m+1)*(m+1)) - links_sum/(2*m*(m+1.0)) + (2+1.0/m)*weights_squared_sum/(4*m*(m+1)*(m+1));
}

double mod_tracker::mod_inc(community* c_0, community* c_1) {
    int m = weightedGraph::get_weight();
    int w_0 = get_weight(c_0);
    int w_1 = get_weight(c_1);
    return -(w_0+w_1+1.0)/(2*(m+1)*(m+1)) + (2+1.0/m)*weights_squared_sum/(4*m*(m+1)*(m+1)) - links_sum/(2*m*(m+1.0));
}
*/
double mod_tracker::mod_inc(int id_u, community* c) {
    int m = weightedGraph::get_weight();
    community* comm_of_u = get_community(id_u);
    int dcuu = relative_weight_node(id_u, comm_of_u);
    int du = weight_node(id_u);
    int dcu = get_weight(comm_of_u);
    int dcvu = relative_weight_node(id_u, c);
    int dcv = get_weight(c);
    int self_loop_weight = edge_weight(edge(id_u, id_u));
    return double(dcvu-dcuu+self_loop_weight)/(2*m) + double(du)*(dcu-dcv-du)/(4*m*m);
}

int mod_tracker::relative_weight_node(int id_u, community* c) {
    int weight = 0;
    for (auto id_v : *c) {
        weight += edge_weight(edge(id_u, id_v));
    }
    return weight;
}

void mod_tracker::erase_edge(tempEdge e) {
    decrease_weight(e);
    int id_u = e.get_start();
    int id_v = e.get_end();
    community* u_comm = get_community(id_u);
    community* v_comm = get_community(id_v);
    increase_weight(u_comm, -1);
    increase_weight(v_comm, -1);

    if (u_comm == v_comm) {
        increase_links(u_comm, -2);
    }
}

void mod_tracker::try_edge_is_coherent(tempEdge e) {
    int id_u = e.get_start();
    int id_v = e.get_end();
    community* comm_of_u = get_community(id_u);
    community* comm_of_v = get_community(id_v);
    if (comm_of_u == comm_of_v) {
        return;
    }

    double inc_u_to_v = mod_inc(id_u, comm_of_v);
    double inc_v_to_u = mod_inc(id_v, comm_of_u);

    if (inc_u_to_v >= inc_v_to_u && inc_u_to_v > 0)  {
        change_community(id_u, comm_of_v);
    }
    if (inc_v_to_u > inc_u_to_v && inc_v_to_u > 0) {
        change_community(id_v, comm_of_u);
    }
}

void mod_tracker::change_community(int id_u, community* c) {
    //cout << "CHANGE " << id_u << "\n";
    community* u_comm = get_community(id_u);
    int d_u_in_u_comm = relative_weight_node(id_u, u_comm);
    int d_u_in_c = relative_weight_node(id_u, c);
    int du = weight_node(id_u);
    increase_links(c, 2 * d_u_in_c);
    increase_links(u_comm, -2 * d_u_in_u_comm);
    increase_weight(c, du);
    increase_weight(u_comm, -du);
    partition::change_community(id_u, c);
}