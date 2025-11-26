#include "Graph.h"
#include <cstdio>
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

	// Necessary for the count in the kruskal's
	bool operator== (const Edge other) {
		bool res = false;
		if ((this->u == other.u) &&
			(this->v = other.v)) {
			res = true;
		}
		return (res);
	}

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


WeightedGraph* kruskal (WeightedGraph* g) {
	
	std::vector<Edge> edges = std::vector<Edge>();

	int vert_n = g->vert_count();
	for (int i = 0; i < vert_n; i++) {
		std::unordered_map<int, std::vector<double>> N = g->vert_neighbors(i);
		for (int j = i; j < vert_n; j++) {
			int edge_n = N[j].size();
			for (int k = 0; k < edge_n; k++) {
				edges.push_back(Edge(i, j, N[j][k]));
			}
		}
	}

	int edge_n = edges.size();
	quicksort(&edges, 0, edge_n-1);
	for (int i = 0; i < edge_n; i++) {
		edges[i].print();
	}
	return (g);
}

int main (void) {

	WeightedGraph g = WeightedGraph(5);
	g.all_verts();
	g.add_edge(0, 1, 0.5);
	g.add_edge(0, 1, 0.7);
	g.add_edge(0, 2, 0.3);
	g.print_raw();
	kruskal(&g);
	return (0);
}
