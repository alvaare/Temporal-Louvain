#ifndef __PARTITION_HPP_INCLUDED__
#define __PARTITION_HPP_INCLUDED__

#include <unordered_set>
#include <unordered_map>
using namespace std;

typedef unordered_set<int> community;

class partition {
    protected:
        unordered_set<community*> communities;
        unordered_map<int, community*> community_of_node;
    
    public:
        ~partition();
        const unordered_set<community*>& get_communities() const;
        void print() const;
        void insert_community(community*);
        void erase_community(community*);
        void erase_communities(unordered_set<community*>&);
        community* get_community(int) const;
        void insert_pair(int, community*);
        void change_community(int, community*);
        void clear();
        void merge_communities(community*, community*);
};

void print_community(community&);

#endif 