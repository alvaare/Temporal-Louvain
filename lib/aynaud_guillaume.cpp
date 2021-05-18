#include <iostream>
#include <vector>
#include "aynaud_guillaume.hpp"
#include "graph.hpp"
#include "louvain.hpp"
#include "performance.hpp"
#include "log.hpp"
using namespace std;

void snapshot::insert_interval(interval i) {
    time.push_back(i);
}

const vector<interval>& snapshot::get_time() const {
    return this->time;
}

//=============================================================================

const int SNAPSHOT_DURATION = 1200;

void construct_graphs(const tempGraph& G, vector<snapshot*>& snapshots) {
    snapshot* w_G = new snapshot;
    w_G->set_nodes(G.get_nodes());
    int begin_date = G.get_edges().begin()->get_time();
    int end_date = begin_date;
    for (auto e : G.get_edges()) {
        if (e.get_time() >= begin_date + SNAPSHOT_DURATION) {
            w_G->insert_interval({begin_date, end_date});
            begin_date = e.get_time();
            snapshots.push_back(w_G);
            w_G = new snapshot;
            w_G->set_nodes(G.get_nodes());
        }
        w_G->add_edge(e);
        end_date = e.get_time();
    }
    //snapshots.push_back(w_G);
}

void apply_louvain(const vector<snapshot*>& snapshots) {
    for (auto snap : snapshots) {
        louvain(*snap, *snap);
    }
}

pair<int, int> get_best_pair_aynaud_guillaume(const vector<snapshot*>& snapshots) {
    int n = snapshots.size();
    double max_sim = -1;
    pair<int, int> best_pair = {-1,-1};
    for (int i = 0; i < n; i++) {
        for (int j = i+1; j < n; j++) {
            double sim = similarity(*snapshots[i], *snapshots[j]);
            if (sim > max_sim) {
                max_sim = sim;
                best_pair = {i,j};
            } 
        }
    }
    return best_pair;
}

void insert_intervals(snapshot& s_obj, const snapshot& s_source) {
    for (auto interv : s_source.get_time()) {
        s_obj.insert_interval(interv);
    }
}

void construct_snapshot_union(snapshot& snap_union, const snapshot& s1, const snapshot& s2) {
    snap_union.set_nodes(s1.get_nodes());
    insert_intervals(snap_union, s1);
    insert_intervals(snap_union, s2);   
    snap_union.add_edges(s1);
    snap_union.add_edges(s2);
    snap_union.half();
    louvain(snap_union, snap_union); 
}

void store_snapshot(const snapshot& s1) {
    for (auto interval : s1.get_time()) {
        string s = find_time(interval.first);
        s += '-';
        s += find_time(interval.second);
        s += " ";
        write_log(s);
    }
    write_log("\n");
}

void aynaud_guillaume(const tempGraph& G) {
    vector<snapshot*> snapshots; 
    construct_graphs(G, snapshots);
    cout << "End construction of snapshots\n";
    apply_louvain(snapshots);
    cout << "End Louvain application on snapshots\n";
    
    while (snapshots.size() > 1) {
        pair<int, int> best_pair = get_best_pair_aynaud_guillaume(snapshots);
        cout << best_pair.first << " " << best_pair.second   << "\n";
        snapshot* snap_union = new snapshot;
        snapshot* s1 = snapshots[best_pair.first];
        snapshot* s2 = snapshots[best_pair.second];
        construct_snapshot_union(*snap_union, *s1, *s2);
        snapshots.erase(snapshots.begin()+best_pair.first);
        snapshots.erase(snapshots.begin()+best_pair.second-1);
        delete s1;
        delete s2;
        snapshots.push_back(snap_union);
        for (auto snap : snapshots) {
            store_snapshot(*snap);
            write_log("\n");
        }
        write_log("END OF SNAPSHOTS\n");
    }
    delete snapshots[0];
}