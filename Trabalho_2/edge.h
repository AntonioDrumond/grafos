#include <iostream>

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

