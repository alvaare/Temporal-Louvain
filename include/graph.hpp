#ifndef __GRAPH_HPP_INCLUDED__
#define __GRAPH_HPP_INCLUDED__

#include <unordered_set>
#include <list>
#include <unordered_map>
using namespace std;

class node {
    protected:
        int id;
        string community; 

    public:
        node(int, string);
        int get_id();
        string get_community();
        void print();
};

class edge {
    protected:
        int id_u;
        int id_v;
    
    public:
        edge(int, int);
        edge();
        int get_start();
        int get_end();
        void print();
};

class tempEdge : public edge {
    protected:
        int time;
    
    public:
        tempEdge(int, int, int);
        int get_time();
        void print();
};

class weightEdge : public edge {
    protected:
        int weight;
    
    public:
        weightEdge(int, int, int);
        int get_weight();
        void print();
};

class graph {
    protected:
        unordered_set<int> nodes;
    
    public:
        unordered_set<int>& get_nodes();
        void add_node(int);
        bool node_is_present(int);
        void print();
        int size();
};

class tempGraph : public graph {
    protected: 
        list<tempEdge> edges;
        unordered_map<int, string> groundtruth;
    
    public:
        list<tempEdge>& get_edges();
        void add_node(node);
        void change_community(int, string);        
        string get_community(int);
        void add_edge(tempEdge);
        void print();
        unordered_map<int, string>& get_groundtruth();
};

class weightedGraph : public graph {
    protected:
        int total_weight = 0;
        unordered_map<int, unordered_map<int, int>> edges;
        unordered_map<int, int> weight_of_node; 
    
    public:
        unordered_map<int, unordered_map<int, int>>& get_edges();
        int get_weight();
        void print();
        void clear();
        void clear_edges();
        int edge_weight(edge);
        bool edge_is_present(edge);
        void add_edge(weightEdge);
        void increase_weight(weightEdge);
        void decrease_weight(weightEdge);
        int weight_node(int);
        void add_node(int);
        unordered_map<int, int>& get_neighbors(int);
};

tempGraph readTempGraph(string, char);

weightedGraph from_temp_to_weight(tempGraph&);

#endif
