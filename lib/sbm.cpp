#include "sbm.hpp"
using namespace std;

double rd0() {
    return (double) rand() / RAND_MAX;
}

void tsbm(tempGraph& G, int k, int n, int m, double p, double q) {
    for (int idk = 0; idk < k; idk++) {
        for (int idn = idk*n; idn < n * (idk+1); idn++) {
            G.add_node(node(idn, to_string(idk)));
        }
    }

    int id_m = 0;
    while (id_m < m) {
        int id_u = rand() % (k*n);
        int id_v = rand() % (k*n);

        if (id_u != id_v) {
            string u_comm = G.get_community(id_u);
            string v_comm = G.get_community(id_v);
            double rd_obj;

            if (u_comm == v_comm) {
                rd_obj = p;
            }
            else {
                rd_obj = q;
            }
            if (rd0() < rd_obj) {
                G.add_edge(tempEdge(id_u, id_v, id_m));
                id_m++;
            }
        }        
    }

    for (int idk = 0; idk < k; idk++) {
        for (int idn = idk; idn < n * k; idn += k) {
            G.add_node(node(idn, to_string(idk)));
        }
    }

    id_m = 0;
    while (id_m < m*4) {
        int id_u = rand() % (k*n);
        int id_v = rand() % (k*n);

        if (id_u != id_v) {
            string u_comm = G.get_community(id_u);
            string v_comm = G.get_community(id_v);
            double rd_obj;

            if (u_comm == v_comm) {
                rd_obj = p;
            }
            else {
                rd_obj = q;
            }
            if (rd0() < rd_obj) {
                G.add_edge(tempEdge(id_u, id_v, m + id_m));
                id_m++;
            }
        }        
    }
}

void tsbm(tempGraph* G, partition& part, int m) {

}