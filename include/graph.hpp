#ifndef __GRAPH_HPP_INCLUDED__
#define __GRAPH_HPP_INCLUDED__

#include <unordered_set>
#include <list>
#include <unordered_map>
#include <vector>
using namespace std;

class node {
    protected:
        int id;
        string community; 

    public:
        node(int, string);
        int get_id() const;
        string get_community() const;
        void print() const;
};

class edge {
    protected:
        int id_u;
        int id_v;
    
    public:
        edge(int, int);
        edge();
        int get_start() const;
        int get_end() const;
        void print() const;
};

class tempEdge : public edge {
    protected:
        int time;
    
    public:
        tempEdge(int, int, int);
        int get_time() const;
        void print() const;
};

class weightEdge : public edge {
    protected:
        int weight;
    
    public:
        weightEdge(int, int, int);
        weightEdge(tempEdge);
        int get_weight() const;
        void print() const;
};

class graph {
    protected:
        unordered_set<int> nodes;
    
    public:
        const unordered_set<int>& get_nodes() const;
        void add_node(int);
        bool node_is_present(int) const;
        void print() const;
        int size() const;
        void set_nodes(const unordered_set<int>&);
};

typedef unordered_map<int, string> groundtruth_type;

class tempGraph : public graph {
    protected: 
        vector<tempEdge> edges;
        groundtruth_type groundtruth;
    
    public:
        const vector<tempEdge>& get_edges() const;
        void add_node(node);
        void change_community(int, string);        
        string get_community(int) const;
        void add_edge(tempEdge);
        void print() const;
        const groundtruth_type& get_groundtruth() const;
        int nb_edges() const;
        tempEdge get_temp_edge(int) const;
};

class weightedGraph : public graph {
    protected:
        int total_weight = 0;
        unordered_map<int, unordered_map<int, int>> edges;
        unordered_map<int, int> weight_of_node; 
    
    public:
        const unordered_map<int, unordered_map<int, int>>& get_edges() const;
        int get_weight() const;
        void print() const;
        void clear();
        void clear_edges();
        int edge_weight(edge) const;
        bool edge_is_present(edge) const;
        void add_edge(weightEdge);
        void increase_weight(weightEdge);
        void decrease_weight(weightEdge);
        int weight_node(int) const;
        void add_node(int);
        void set_nodes(const unordered_set<int>&);
        const unordered_map<int, int>& get_neighbors(int) const;
        void add_edges(const weightedGraph&);
        void half();
};

tempGraph readTempGraph(string, char);

weightedGraph from_temp_to_weight(const tempGraph&);

weightedGraph graph_union(const weightedGraph&, const weightedGraph&);

#endif
