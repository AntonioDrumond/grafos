#include "Graph.h"
#include <iostream>
#include <cassert>

void test_add_vertex() {
    Graph g1(2);
    assert(g1.add_vert() == true);
    assert(g1.add_vert() == true);
    assert(g1.add_vert() == false); // bigger than capacity
    std::cout << "add_vert not weighted : OK\n";

    WeightedGraph g(2);
    assert(g.add_vert() == true);
    assert(g.add_vert() == true);
    assert(g.add_vert() == false); // bigger than capacity
    std::cout << "add_vert weighted : OK\n";
    //destructor is called automatically at the end of the scope
}

void test_add_edge() {
    WeightedGraph g1(5); // undirected 
    g1.all_verts();
    assert(g1.add_edge(0, 1, 1.5) == true);
    assert(g1.add_edge(0, 1, 1.5) == false); // weight already exists
    assert(g1.add_edge(0, 1, 2.0) == true); // new weight 
    assert(g1.check_edge(0, 1) == true);
    assert(g1.check_edge(1, 0) == true);
    assert(g1.check_edge(0, 2) == false);
    std::cout << "add_edge & check_edge for undirected weighted : OK\n";

    WeightedGraph g2(5, true); // directed 
    g2.all_verts();
    assert(g2.add_edge(0, 1, 1.5) == true);
    assert(g2.add_edge(0, 1, 1.5) == false); // weight already exists
    assert(g2.add_edge(0, 1, 2.0) == true); // new weight
    assert(g2.check_edge(0, 1) == true);
    assert(g2.check_edge(1, 0) == false);
    assert(g2.check_edge(0, 2) == false);
    std::cout << "add_edge & check_edge for directed weighted  : OK\n";

    Graph g3(5); // undirected 
    g3.all_verts();
    assert(g3.add_edge(0, 1) == true);
    assert(g3.add_edge(0, 1) == false); // already exists
    assert(g3.check_edge(0, 1) == true);
    assert(g3.check_edge(1, 0) == true);
    assert(g3.check_edge(0, 2) == false);
    std::cout << "add_edge & check_edge for undirected not weighted : OK\n";

    Graph g4(5, true); // directed 
    g4.all_verts();
    assert(g4.add_edge(0, 1) == true);
    assert(g4.add_edge(0, 1) == false); // already exists
    assert(g4.check_edge(0, 1) == true);
    assert(g4.check_edge(1, 0) == false);
    assert(g4.check_edge(0, 2) == false);
    std::cout << "add_edge & check_edge for directed not weighted : OK\n";
}

void test_remove_edge() {
    Graph g1(5);
    g1.all_verts();
    g1.add_edge(0, 1);
    g1.add_edge(1, 2);
    assert(g1.remove_edge(0, 1) == true);
    assert(g1.check_edge(0, 1) == false);
    assert(g1.check_edge(1, 0) == false);
    assert(g1.remove_edge(0, 1) == false); // should not work as its already gone
    std::cout << "remove_edge not weighted : OK\n";

    WeightedGraph g(5);
    g.all_verts();
    g.add_edge(0, 1, 1.5);
    g.add_edge(1, 2, 2.5);
    g.add_edge(2, 1, 4.5);
    assert(g.remove_edge(0, 1) == true);
    assert(g.remove_edge(2, 1, 2.0) == false);
    assert(g.remove_edge(2, 1, 2.5) == true);
    assert(g.remove_edge(2, 1, 4.5) == true);
    assert(g.check_edge(0, 1) == false);
    assert(g.check_edge(1, 0) == false);
    assert(g.remove_edge(0, 1) == false); // should not work as its already gone
    std::cout << "remove_edge weighted : OK\n";
}

void test_neighbors() {
    Graph g1(5);
    g1.all_verts();
    g1.add_edge(0, 1);
    g1.add_edge(0, 2);

    auto neighbors0 = g1.vert_neighbors(0);
    assert(neighbors0.size() == 2);
    assert(neighbors0.count(1) == 1);
    assert(neighbors0.count(2) == 1);

    std::cout << "neighbors not weigthed: OK\n";

    WeightedGraph g2(5);
    g2.all_verts();
    g2.add_edge(0, 1, 1.5);
    g2.add_edge(0, 2, 2.5);

    auto neighbors = g2.vert_neighbors(0);
    assert(neighbors.size() == 2);
    assert(neighbors.at(1)[0] == 1.5);
    assert(neighbors.at(2)[0] == 2.5);

    std::cout << "neighbors undirected weighted : OK\n";
}

void test_empty_graph() {
    std::cout << "Testing empty graph...\n";
    Graph empty_g(0);
    assert(empty_g.add_vert() == false);
    assert(empty_g.check_edge(0, 1) == false);
    std::cout << "Empty graph edge cases: OK\n";
}

void test_single_node_graph() {
    std::cout << "Testing single node graph...\n";
    Graph single_g(1);
    assert(single_g.add_vert() == true);
    assert(single_g.add_edge(0, 0) == true); // self-loop if allowed
    assert(single_g.check_edge(0, 0) == true);
    std::cout << "Single node graph: OK\n";
}

void test_invalid_vertex_access() {
    std::cout << "Testing invalid vertex access...\n";
    Graph single_g(1);
    single_g.add_vert();
    if (single_g.add_edge(0, 2)) {
        std::cout << "Error: An invalid vertex access ocurred!\n";
    } else {
        std::cout << "Invalid vertex access handle: OK\n";
    }
}

void test_large_graph_performance() {
    std::cout << "Testing large graph performance...\n";
    const int N = 1000;
    Graph large_g(N);
    for (int i = 0; i < N; ++i) large_g.add_vert();
    for (int i = 0; i < N - 1; ++i) large_g.add_edge(i, i + 1);
    assert(large_g.check_edge(0, 1) == true);
    assert(large_g.check_edge(N - 2, N - 1) == true);
    std::cout << "Large graph test: OK\n";
}

int main (int argc, char *argv[]) {
    
	/*
    WeightedGraph* g = new WeightedGraph(10);
    g->add_vert();
    g->add_vert("aa");
    g->add_vert();
    g->add_vert();
	std::cout << g->getLabel(2) << "\n";
	try
	{
		std::cout << g->getLabel(5) << "\n";
	}
	catch (const std::invalid_argument& ex)
	{
	}
    g->add_edge(0 , 1, 0.5);
    g->add_edge(1 , 2, 0.5);
    g->add_edge(0 , 2, 0.5);
    g->add_edge(3 , 2, 0.5);
    g->add_edge(3 , 2, 0.6);
    //if(g->check_edge(4, 2)) std::cout << "yes\n";
    //else std::cout << "no\n";
//	    g->print_csacademy();
//	    g->print_raw();
    delete g;
    return 0;
	*/

    test_add_vertex();
    test_add_edge();
    test_remove_edge();
    test_neighbors();
    test_empty_graph();
    test_single_node_graph();
    test_invalid_vertex_access();
    test_large_graph_performance();
    std::cout << "All tests passed ✅\n";
    return 0;
}
