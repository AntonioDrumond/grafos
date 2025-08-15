#include "Graph.h"

int main (int argc, char *argv[]) {
    Graph* g = new Graph(10);
    g->test();
    delete g;
    return 0;
}
