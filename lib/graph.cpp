#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "graph.hpp"

node::node(int id, string community) {
    this->id = id;
    this->community = community;
}

int node::get_id() {
    return id; 
}

string node::get_community() {
    return community;
}

void node::print() {
    cout << id << " " << community << "\n";
}

edge::edge(int id_u, int id_v) {
    this->id_u = id_u;
    this->id_v = id_v;
}

edge::edge() = default;

int edge::get_start() {
    return id_u;
}

int edge::get_end() {
    return id_v;
}

void edge::print() {
    cout << id_u << " " << id_v << "\n";
}

tempEdge::tempEdge(int id_u, int id_v, int time) {
    this->id_u = id_u;
    this->id_v = id_v;
    this->time = time;
} 

int tempEdge::get_time() {
    return time;
}

void tempEdge::print() {
    cout << time << " " << id_u << " " << id_v << "\n";
}

weightEdge::weightEdge(int id_u, int id_v, int weight) {
    this->id_u = id_u;
    this->id_v = id_v;
    this->weight = weight;
}

int weightEdge::get_weight() {
    return weight;
}

void weightEdge::print() {
    cout << id_u << " " << id_v << " " << weight << "\n";
}

unordered_set<int>& graph::get_nodes() {
    return nodes;
}

void graph::add_node(int id) {
    nodes.insert(id);
}

bool graph::node_is_present(int id) {
    return nodes.find(id) != nodes.end();
}

int graph::size() {
    return nodes.size();
}

void graph::print() {
    for (auto i : nodes) {
        cout << i << " ";
    }
    cout << "\n";
}

list<tempEdge>& tempGraph::get_edges() {
    return edges;
}

void tempGraph::add_node(node u) {
    graph::add_node(u.get_id());
    groundtruth[u.get_id()] = u.get_community();
}

void tempGraph::add_edge(tempEdge e) {
    edges.push_back(e);
}

string tempGraph::get_community(int id) {
    return groundtruth[id];
}

void tempGraph::print() {
    cout << "Edges: \n";
    for (auto e : edges) {
        e.print();
    }
    cout << "Groundtruth: \n";
    for (auto i : groundtruth) {
        cout << i.first << "\t" << i.second << "\n";
    }
}

unordered_map<int, string>& tempGraph::get_groundtruth() {
    return groundtruth;
}

unordered_map<int, unordered_map<int, int>>& weightedGraph::get_edges() {
    return edges;
}

int weightedGraph::get_weight() {
   return total_weight;
}

void weightedGraph::print() {
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
                    
int weightedGraph::edge_weight(edge e) {
    int id_u = e.get_start();
    int id_v = e.get_end();
    if (edges[id_u].find(id_v)!=edges[id_u].end()) {
        return edges[id_v][id_u];
    }
    return 0;
}

bool weightedGraph::edge_is_present(edge e) {
    return edges[e.get_start()].find(e.get_end())!=edges[e.get_end()].end();
}

void weightedGraph::add_edge(weightEdge e) {
    int id_u = e.get_start();
    int id_v = e.get_end();
    total_weight += e.get_weight();
    add_node(id_u);
    add_node(id_v);
    edges[id_u].insert({id_v, e.get_weight()});
    edges[id_v].insert({id_u, e.get_weight()});
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

int weightedGraph::weight_node(int id) {
   return weight_of_node[id];
}

void weightedGraph::add_node(int id) {
   graph::add_node(id);
   weight_of_node.insert({id, 0});
}

unordered_map<int, int>& weightedGraph::get_neighbors(int id) {
   return edges[id];
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

weightedGraph from_temp_to_weight(tempGraph& temp_G) {
    weightedGraph w_G;
    for (auto edge : temp_G.get_edges()) {
        weightEdge w_e(edge.get_start(), edge.get_end(), 1);
        w_G.increase_weight(w_e);
    }
    return w_G;
}
