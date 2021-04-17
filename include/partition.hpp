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
        unordered_set<community*>& get_communities();
        void print();
        void insert_community(community*);
        void erase_community(community*);
        void erase_communities(unordered_set<community*>&);
        community* get_community(int);
        void change_community(int, community* );
        void clear();
};

#endif 