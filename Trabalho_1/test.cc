#include "Graph.h"

int main (int argc, char *argv[]) {
    Graph* g = new Graph(10);
    g->add_vert();
    g->add_vert();
    g->add_vert();
    g->add_vert();
    
    g->add_edge(0 , 1);
    g->add_edge(1 , 2);
    g->add_edge(0 , 2);
    g->add_edge(3 , 2);
    //if(g->check_edge(4, 2)) std::cout << "yes\n";
    //else std::cout << "no\n";
    g->print_csacademy();
    g->print_raw();
    delete g;
    return 0;
}
