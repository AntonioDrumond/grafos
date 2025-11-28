#include "Graph.h"
#include <queue>
#include <unordered_map>
#include <vector>

#ifndef EDGE_H
#define EDGE_H
#include "edge.h"
#endif

struct minHeap {
	bool operator()(const Edge& a, const Edge& b) const {
		bool res = false;
		if (a.w != b.w) {
			res = (a.w > b.w);
		} else { 
			res = (a.u < b.u || a.v < b.v);
		}
		return (res);
	}
};

// Get a MST from kruskal's algorithm
// ! Should not be called when g is directed !
WeightedGraph* kruskal_segmentation (WeightedGraph* G, double max_weight, int width) {
	
	std::priority_queue<Edge,  std::vector<Edge>, minHeap> pq;
	int vert_n = G->vert_count();

	// Find edges
	for (int i = 0; i < vert_n; i++) {

		int i1 = i+1;
		int iw = i+width;
		int iw1 = iw+1;

		if ((i1 % width) != 0) {
			std::vector<double> weights = G->get_weight(i, i1);
			for (double w : weights) {
				pq.push(Edge(i, i1, w));
			}
		}
		if ((iw % width) != 0) {
			std::vector<double> weights = G->get_weight(i, iw);
			for (double w : weights) {
				pq.push(Edge(i, iw, w));
			}
		}
		if ((iw1 % width) != 0) {
			std::vector<double> weights = G->get_weight(i, iw1);
			for (double w : weights) {
				pq.push(Edge(i, iw1, w));
			}
		}
	}

	// Create the MST's Graph
	WeightedGraph* T = new WeightedGraph(vert_n);
	T->all_verts();
	T->setPixColor(G->getPixColor());

	int union_find [2][vert_n];
	for (int i = 0; i < vert_n; i++) {
		union_find[0][i] = i; // ancestor
		union_find[1][i] = 0; // rank
	}

	int edge_n = pq.size();

	// Union find
	for (int i = 0; i < edge_n; i++) {

		Edge e = pq.top();
		pq.pop();

		if (e.w >= max_weight) {
			i = edge_n;
		} else {
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

				// If u's ancestor has a higher or equal rank
				if (union_find[1][ancestor_u] >= union_find[1][ancestor_v]) { 
					union_find[0][ancestor_v] = ancestor_u; // Set parent 
					union_find[1][ancestor_u]++; 			// Increase rank

				// If v's ancestor has a higher rank
				} else {
					union_find[0][ancestor_u] = ancestor_v; // Set parent 
					union_find[1][ancestor_v]++; 			// Increase rank
				}
			}
		}
	}

	// Paint components

	std::vector<int> stack = std::vector<int>(vert_n);
	std::vector<RGB> colors = T->getPixColor();

	for (int i = 0; i < vert_n; i++) {
		int ancestor_u = i;
		while (ancestor_u != union_find[0][ancestor_u]) {
			stack.push_back(ancestor_u);
			ancestor_u = union_find[0][ancestor_u];
		}

		if (ancestor_u != i) {
			while (!stack.empty()) {
				int u = stack.back();
				stack.pop_back();
				colors[u] = colors[ancestor_u];
				union_find[0][u] = u;
			}
		}
	}
	T->setPixColor(colors);

	return (T);
}
