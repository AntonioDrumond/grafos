#include "Graph.h"
#include <unordered_map>
#include <vector>

struct Edge {
	int u;			// If G is directed, then: u -> v
	int v;
	double w = 0.0;

	Edge() {
		this->u = -1;
		this->v = -1;
		this->w = 0.0;
	}

	Edge(int u, int v) {
		this->u = u;
		this->v = v;
	}

	Edge(int u, int v, double w) {
		this->u = u;
		this->v = v;
		this->w = w;
	}

	~Edge() = default;

	void print(void) {
		std::cout << "u: " << this->u << " v: " << this->v << " w: " << this->w << std::endl;
	}

};

void quicksort(std::vector<Edge>* arr, int L, int R) {
	double pivot = (*arr)[(int)((L+R)/2.0)].w;
	int l = L;
	int r = R;

	while (l <= r) {
		while ((*arr)[l].w < pivot) {
			l++;
		}
		while ((*arr)[r].w > pivot) {
			r--;
		}
		if (l <= r) {
			Edge tmp = (*arr)[l];
			(*arr)[l] = (*arr)[r];
			(*arr)[r] = tmp;
			l++;
			r--;
		}
	}
	
	if (l < R) {
		quicksort(arr, l, R);
	}
	if (L < r) {
		quicksort(arr, L, r);
	}
}


// Get a MST from kruskal's algorithm
// ! Should not be called when g is directed !
WeightedGraph* kruskal (WeightedGraph* G) {
	
	std::vector<Edge> edges = std::vector<Edge>();
	int vert_n = G->vert_count();
	int union_find [2][vert_n];

	// Find edges
	for (int i = 0; i < vert_n; i++) {

		// Reuse the same for to create the union_find vector
		union_find[0][i] = i; // ancestor
		union_find[1][i] = 0; // rank

		std::unordered_map<int, std::vector<double>> N = G->vert_neighbors(i);

		for (int j = i; j < vert_n; j++) {
			int edge_n = N[j].size();
			for (int k = 0; k < edge_n; k++) {
				edges.push_back(Edge(i, j, N[j][k]));
			}
		}
	}

	int edge_n = edges.size();
	quicksort(&edges, 0, edge_n-1);

	// Create the MST's Graph
	WeightedGraph* T = new WeightedGraph(vert_n);
	T->all_verts();

	// Union find
	for (int i = 0; i < edge_n; i++) {

		Edge e = edges[i];
		int u = e.u;
		int v = e.v;
		int ancestor_u = u;
		int ancestor_v = v;

		// Find oldest ancestor for u
		while (ancestor_u != union_find[0][ancestor_u]) {
			ancestor_u = union_find[0][ancestor_u];
		}

		// Find oldest ancestor for u
		while (ancestor_v != union_find[0][ancestor_v]) {
			ancestor_v = union_find[0][ancestor_v];
		}

		// If they dont share the same ancestor | there is no cycle
		if (ancestor_u != ancestor_v) {

			T->add_edge(u, v, e.w);

			// If u's ancestor has a higher rank
			if (union_find[1][ancestor_u] > union_find[1][ancestor_v]) { 
				union_find[0][ancestor_v] = ancestor_u; // Set parent 
				union_find[1][ancestor_u]++; 			// Increase rank

			// If v's ancestor has a higher rank
			} else if (union_find[1][ancestor_u] < union_find[1][ancestor_v]) {
				union_find[0][ancestor_u] = ancestor_v; // Set parent 
				union_find[1][ancestor_v]++; 			// Increase rank
			}
		}
	}
	return (T);
}

int main (void) {

	WeightedGraph g = WeightedGraph(11);
	g.all_verts();

	g.add_edge(0, 1, 7);
	g.add_edge(0, 2, 8);
	g.add_edge(1, 3, 5);
	g.add_edge(1, 4, 9);
	g.add_edge(2, 3, 2);
	g.add_edge(3, 4, 13);
	g.add_edge(3, 6, 11);
	g.add_edge(3, 10, 4);
	g.add_edge(4, 5, 6);
	g.add_edge(4, 8, 2);
	g.add_edge(6, 7, 6);
	g.add_edge(7, 8, 6);
	g.add_edge(8, 9, 3);
	g.add_edge(9, 10, 5);

	g.print_raw();
	std::cout << "\n\n\n";

	WeightedGraph* T = kruskal(&g);
	T->print_raw();

	return (0);
}
