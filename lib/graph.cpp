#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "graph.hpp"

node::node(int id, string community) {
    this->id = id;
    this->community = community;
}

int node::get_id() const {
    return id; 
}

string node::get_community() const {
    return community;
}

void node::print() const {
    cout << id << " " << community << "\n";
}

edge::edge(int id_u, int id_v) {
    this->id_u = id_u;
    this->id_v = id_v;
}

edge::edge() = default;

int edge::get_start() const {
    return id_u;
}

int edge::get_end() const {
    return id_v;
}

void edge::print() const {
    cout << id_u << " " << id_v << "\n";
}

tempEdge::tempEdge(int id_u, int id_v, int time) {
    this->id_u = id_u;
    this->id_v = id_v;
    this->time = time;
} 

int tempEdge::get_time() const {
    return time;
}

void tempEdge::print() const {
    cout << time << " " << id_u << " " << id_v << "\n";
}

weightEdge::weightEdge(int id_u, int id_v, int weight) {
    this->id_u = id_u;
    this->id_v = id_v;
    this->weight = weight;
}

int weightEdge::get_weight() const {
    return weight;
}

void weightEdge::print() const {
    cout << id_u << " " << id_v << " " << weight << "\n";
}

const unordered_set<int>& graph::get_nodes() const {
    return nodes;
}

void graph::add_node(int id) {
    nodes.insert(id);
}

bool graph::node_is_present(int id) const {
    return nodes.find(id) != nodes.end();
}

int graph::size() const {
    return nodes.size();
}

void graph::print() const {
    for (auto i : nodes) {
        cout << i << " ";
    }
    cout << "\n";
}

const list<tempEdge>& tempGraph::get_edges() const {
    return edges;
}

void tempGraph::add_node(node u) {
    graph::add_node(u.get_id());
    groundtruth[u.get_id()] = u.get_community();
}

void tempGraph::change_community(int id, string c) {
    groundtruth[id] = c;
}

void tempGraph::add_edge(tempEdge e) {
    edges.push_back(e);
}

string tempGraph::get_community(int id) const {
    return groundtruth.at(id);
}

void tempGraph::print() const {
    cout << "Edges: \n";
    for (auto e : edges) {
        e.print();
    }
    cout << "Groundtruth: \n";
    for (auto i : groundtruth) {
        cout << i.first << "\t" << i.second << "\n";
    }
}

const groundtruth_type& tempGraph::get_groundtruth() const {
    return groundtruth;
}

const unordered_map<int, unordered_map<int, int>>& weightedGraph::get_edges() const {
    return edges;
}

int weightedGraph::get_weight() const {
   return total_weight;
}

void weightedGraph::print() const {
    cout << "Total weight: " << total_weight << "\n";
    for (auto e : edges) {
        unordered_map<int, int> neighbors = e.second;
        for (auto neighbor : neighbors) {
            cout << e.first << " " << neighbor.first << " " << neighbor.second << "\n";
        }
    }        
}

void weightedGraph::clear() {
    total_weight = 0;
    nodes.clear();
    edges.clear();
    weight_of_node.clear();
}

void weightedGraph::clear_edges() {
    total_weight = 0;
    edges.clear();
    weight_of_node.clear();
}
                    
int weightedGraph::edge_weight(edge e) const {
    int id_u = e.get_start();
    int id_v = e.get_end();
    if (edges.at(id_u).find(id_v)!=edges.at(id_u).end()) {
        return edges.at(id_v).at(id_u);
    }
    return 0;
}

bool weightedGraph::edge_is_present(edge e) const {
    return edges.at(e.get_start()).find(e.get_end())!=edges.at(e.get_end()).end();
}

void weightedGraph::add_edge(weightEdge e) {
    int id_u = e.get_start();
    int id_v = e.get_end();
    total_weight += e.get_weight();
    add_node(id_u);
    add_node(id_v);
    edges[id_u][id_v] += e.get_weight();
    edges[id_v][id_u] += e.get_weight();
    weight_of_node[id_u] += e.get_weight();
    weight_of_node[id_v] += e.get_weight();
}

void weightedGraph::increase_weight(weightEdge e) {
    int id_u = e.get_start();
    int id_v = e.get_end();
    if (!edge_is_present(edge(id_u, id_v))) {
        add_edge(e);
    }
    else {
        total_weight += e.get_weight();
        edges[id_u][id_v] += e.get_weight();
        edges[id_v][id_u] += e.get_weight();
        weight_of_node[id_u] += e.get_weight();
        weight_of_node[id_v] += e.get_weight();
    }
}

void weightedGraph::decrease_weight(weightEdge e) {
    int id_u = e.get_start();
    int id_v = e.get_end();
    if (!edge_is_present(edge(id_u, id_v))) {
        cout << "Trying to decreasing absent edge!\n";
    }
    else {
        int w = e.get_weight();
        total_weight -= w;
        edges[id_u][id_v] -= w;
        edges[id_v][id_u] -= w;
        weight_of_node[id_u] -= w;
        weight_of_node[id_v] -= w;
    }
}

int weightedGraph::weight_node(int id) const {
   return weight_of_node.at(id);
}

void weightedGraph::add_node(int id) {
    graph::add_node(id);
    weight_of_node.insert({id, 0});
    edges.insert({id, {}});
}

const unordered_map<int, int>& weightedGraph::get_neighbors(int id) const {
    return edges.at(id);
}

//=============================================================================

vector<string> split(const string& s, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

void addLine(string line, tempGraph* G, char delimiter) {
    vector<string> scattered_line = split(line, delimiter);
    int time = stoi(scattered_line[0]);
    int id_u = stoi(scattered_line[1]);
    int id_v = stoi(scattered_line[2]);
    string community_u = scattered_line[3];
    string community_v = scattered_line[4];
        
    G->add_edge(tempEdge(id_u, id_v, time));
    G->add_node(node(id_u, community_u));
    G->add_node(node(id_v, community_v));
}

tempGraph readTempGraph(string filename, char delimiter) {
    tempGraph G;
    string line;
    ifstream myfile (filename);
    while (getline(myfile, line)) {
        addLine(line, &G, delimiter);
    }
    return G;
}

//=============================================================================

weightedGraph from_temp_to_weight(const tempGraph& temp_G) {
    weightedGraph w_G;
    for (auto edge : temp_G.get_edges()) {
        weightEdge w_e(edge.get_start(), edge.get_end(), 1);
        w_G.add_edge(w_e);
    }
    return w_G;
}
