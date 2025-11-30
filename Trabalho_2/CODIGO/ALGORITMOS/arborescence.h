#ifndef ARBORESCENCE_H
#define ARBORESCENCE_H

#include "../GRAFO/edge.h"
#include "../GRAFO/Graph.h"
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>
#include <algorithm>
#include <limits>
#include <cmath>
#include <climits>

struct DirectedEdge {
    int source;         // Vértice de origem 
    int target;         // Vértice de destino 
    double cost;        // Custo/peso da aresta
    
    DirectedEdge();
    DirectedEdge(int from, int to, double weight);
    
    void show() const;
    bool is_valid() const;
};


class DirectedGraph {
private:
    int max_vertices;                                           // Capacidade máxima 
    int current_vertices;                                       // |V| atual
    std::vector<std::unordered_map<int, double>> outgoing;     // [u][v] = peso da aresta u→v
    std::vector<std::unordered_map<int, double>> incoming;     // [v][u] = peso da aresta u→v

public:
    DirectedGraph(int capacity);
    ~DirectedGraph() = default;
    
    // Vértices...
    bool add_vertex();                  
    void add_all_vertices();           // Adiciona todos os vértices possíveis até a capacidade
    int vertex_count() const;          
    int capacity() const;              // Retorna capacidade máxima
    
    // Arestas...
    bool connect(int from, int to, double cost);           // Conecta vértice da origem ao destino com custo
    bool disconnect(int from, int to);                     // Remove conexão 
    bool has_connection(int from, int to) const;           // Verifica se existe conexão
    double connection_cost(int from, int to) const;       // Obtém custo da conexão 
    int total_connections() const;                         // |E|
    
    // Navegação dentro do grafo por arestas
    std::unordered_map<int, double> get_destinations_from(int vertex) const;   // Para onde o vértice aponta
    std::unordered_map<int, double> get_sources_to(int vertex) const;          // Quem aponta para o vértice
    std::vector<DirectedEdge> get_all_connections() const;                     // Todas as arestas do grafo
    std::vector<DirectedEdge> get_minimum_undirected_edges() const;             // Pares u-v com menor custo
    
    void display() const;                                                 
    bool is_reachable(int from, int to) const;                               // Verifica se o destino é alcancável a partir da origem
    
    static DirectedGraph from_weighted_graph(const WeightedGraph& weighted_graph);  // Converte WeightedGraph direcionado em DirectedGraph
};


struct ArborescenceResult {
    std::vector<int> parent_of;         // Pai do vértice v na árvore 
    std::vector<double> edge_costs;     // Custo da aresta que conecta parent_of[v] → v
    double total_tree_cost;             // Custo total de toda a arborescência
    int root_vertex;                    // Raiz?
    bool is_complete;                   
    
    ArborescenceResult(int num_vertices, int root);
    
    void display_tree() const;                        
    std::vector<int> get_children_of(int vertex) const;    // Retorna filhos diretos de um vértice
    int get_tree_depth_of(int vertex) const;              // Calcula profundidade 
    bool is_ancestor(int possible_ancestor, int descendant) const;  // Relação ancestral
    
    std::vector<std::vector<int>> get_subtrees() const;
    std::vector<int> get_path_to_root(int vertex) const;
    
    // Converte arborescência para matriz de imagem PPM
    std::vector<std::vector<std::vector<int>>> to_ppm_matrix(
        int width, int height, const std::vector<RGB>& original_colors) const;
};

// ============================================================================
// IMPLEMENTAÇÕES INLINE
// ============================================================================

// DirectedEdge
inline DirectedEdge::DirectedEdge() : source(-1), target(-1), cost(0.0) {}

inline DirectedEdge::DirectedEdge(int from, int to, double weight) 
    : source(from), target(to), cost(weight) {}

inline void DirectedEdge::show() const {}

inline bool DirectedEdge::is_valid() const {
    return source >= 0 && target >= 0 && cost >= 0.0;
}

// DirectedGraph
inline DirectedGraph::DirectedGraph(int capacity) 
    : max_vertices(capacity), current_vertices(0), outgoing(capacity), incoming(capacity) {}

inline bool DirectedGraph::add_vertex() {
    if (current_vertices < max_vertices) {
        current_vertices++;
        return true;
    }
    return false;
}

inline void DirectedGraph::add_all_vertices() {
    current_vertices = max_vertices;
}

inline int DirectedGraph::vertex_count() const {
    return current_vertices;
}

inline int DirectedGraph::capacity() const {
    return max_vertices;
}

inline bool DirectedGraph::connect(int from, int to, double cost) {
    if (from >= current_vertices || to >= current_vertices || from < 0 || to < 0) {
        return false;
    }
    outgoing[from][to] = cost;
    incoming[to][from] = cost;
    return true;
}

inline bool DirectedGraph::disconnect(int from, int to) {
    if (from >= current_vertices || to >= current_vertices || from < 0 || to < 0) {
        return false;
    }
    outgoing[from].erase(to);
    incoming[to].erase(from);
    return true;
}

inline bool DirectedGraph::has_connection(int from, int to) const {
    if (from >= current_vertices || to >= current_vertices || from < 0 || to < 0) {
        return false;
    }
    return outgoing[from].find(to) != outgoing[from].end();
}

inline double DirectedGraph::connection_cost(int from, int to) const {
    if (!has_connection(from, to)) {
        return std::numeric_limits<double>::infinity();
    }
    return outgoing[from].at(to);
}

inline int DirectedGraph::total_connections() const {
    int count = 0;
    for (int u = 0; u < current_vertices; u++) {
        count += outgoing[u].size();
    }
    return count;
}

inline std::unordered_map<int, double> DirectedGraph::get_destinations_from(int vertex) const {
    if (vertex >= current_vertices || vertex < 0) {
        return {};
    }
    return outgoing[vertex];
}

inline std::unordered_map<int, double> DirectedGraph::get_sources_to(int vertex) const {
    if (vertex >= current_vertices || vertex < 0) {
        return {};
    }
    return incoming[vertex];
}

inline std::vector<DirectedEdge> DirectedGraph::get_all_connections() const {
    std::vector<DirectedEdge> edges;
    for (int u = 0; u < current_vertices; u++) {
        for (const auto& [v, cost] : outgoing[u]) {
            edges.emplace_back(u, v, cost);
        }
    }
    return edges;
}

inline std::vector<DirectedEdge> DirectedGraph::get_minimum_undirected_edges() const {
    std::vector<DirectedEdge> undirected_edges;

    std::unordered_map<long long, double> min_cost;
    min_cost.reserve(total_connections());

    auto encode_pair = [](int a, int b) -> long long {
        if (a > b) {
            std::swap(a, b);
        }
        return (static_cast<long long>(a) << 32) | static_cast<unsigned int>(b);
    };

    for (int u = 0; u < current_vertices; u++) {
        for (const auto& [v, cost] : outgoing[u]) {
            if (u == v) {
                continue;
            }
            long long key = encode_pair(u, v);
            auto it = min_cost.find(key);
            if (it == min_cost.end() || cost < it->second) {
                min_cost[key] = cost;
            }
        }
    }

    undirected_edges.reserve(min_cost.size());
    for (const auto& entry : min_cost) {
        int u = static_cast<int>(entry.first >> 32);
        int v = static_cast<int>(entry.first & 0xffffffffLL);
        undirected_edges.emplace_back(u, v, entry.second);
    }

    return undirected_edges;
}

inline void DirectedGraph::display() const {}

inline bool DirectedGraph::is_reachable(int from, int to) const {
    if (from == to) return true;
    if (from >= current_vertices || to >= current_vertices || from < 0 || to < 0) {
        return false;
    }
    
    std::vector<bool> visited(current_vertices, false);
    std::queue<int> queue;
    queue.push(from);
    visited[from] = true;
    
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        
        for (const auto& [neighbor, cost] : outgoing[current]) {
            if (neighbor == to) return true;
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                queue.push(neighbor);
            }
        }
    }
    return false;
}

inline DirectedGraph DirectedGraph::from_weighted_graph(const WeightedGraph& weighted_graph) {
    WeightedGraph& graph_ref = const_cast<WeightedGraph&>(weighted_graph);
    int n = graph_ref.vert_count();
    DirectedGraph directed(n);
    directed.add_all_vertices();
    
    for (int u = 0; u < n; u++) {
        auto neighbors = graph_ref.vert_neighbors(u);
        for (const auto& [v, weights] : neighbors) {
            if (!weights.empty()) {
                double cost = *std::min_element(weights.begin(), weights.end());
                directed.connect(u, v, cost);
            }
        }
    }
    return directed;
}

// ArborescenceResult
inline ArborescenceResult::ArborescenceResult(int num_vertices, int root) 
    : parent_of(num_vertices, -1), edge_costs(num_vertices, 0.0), 
      total_tree_cost(0.0), root_vertex(root), is_complete(false) {}

inline void ArborescenceResult::display_tree() const {}

inline std::vector<int> ArborescenceResult::get_children_of(int vertex) const {
    std::vector<int> children;
    for (int v = 0; v < parent_of.size(); v++) {
        if (parent_of[v] == vertex) {
            children.push_back(v);
        }
    }
    return children;
}

inline int ArborescenceResult::get_tree_depth_of(int vertex) const {
    if (vertex == root_vertex) return 0;
    if (vertex < 0 || vertex >= parent_of.size()) return -1;
    if (parent_of[vertex] == -1) return -1;
    
    int depth = 0;
    int current = vertex;
    
    while (current != root_vertex && parent_of[current] != -1) {
        current = parent_of[current];
        depth++;
        if (depth > parent_of.size()) return -1;
    }
    return (current == root_vertex) ? depth : -1;
}

inline bool ArborescenceResult::is_ancestor(int possible_ancestor, int descendant) const {
    if (possible_ancestor == descendant) return true;
    if (descendant < 0 || descendant >= parent_of.size()) return false;
    
    int current = descendant;
    while (current != -1 && current != root_vertex) {
        if (parent_of[current] == possible_ancestor) return true;
        current = parent_of[current];
    }
    return false;
}

inline std::vector<std::vector<int>> ArborescenceResult::get_subtrees() const {
    std::vector<std::vector<int>> subtrees;
    std::vector<bool> visited(parent_of.size(), false);
    
    for (int v = 0; v < parent_of.size(); v++) {
        if (!visited[v]) {
            std::vector<int> subtree;
            std::stack<int> stack;
            stack.push(v);
            
            while (!stack.empty()) {
                int current = stack.top();
                stack.pop();
                
                if (!visited[current]) {
                    visited[current] = true;
                    subtree.push_back(current);
                    
                    auto children = get_children_of(current);
                    for (int child : children) {
                        if (!visited[child]) {
                            stack.push(child);
                        }
                    }
                }
            }
            
            if (!subtree.empty()) {
                subtrees.push_back(subtree);
            }
        }
    }
    return subtrees;
}

inline std::vector<int> ArborescenceResult::get_path_to_root(int vertex) const {
    std::vector<int> path;
    if (vertex < 0 || vertex >= parent_of.size()) return path;
    
    int current = vertex;
    while (current != -1) {
        path.push_back(current);
        if (current == root_vertex) break;
        current = parent_of[current];
        if (path.size() > parent_of.size()) break;
    }
    return path;
}

inline std::vector<std::vector<std::vector<int>>> ArborescenceResult::to_ppm_matrix(
    int width, int height, const std::vector<RGB>& original_colors) const {

    std::vector<std::vector<std::vector<int>>> result(
        height,
        std::vector<std::vector<int>>(width, std::vector<int>(3))
    );

    int nVerts = static_cast<int>(parent_of.size());
    int maxPixels = width * height;
    if (nVerts > maxPixels) {
        nVerts = maxPixels;
    }

    std::unordered_map<int, RGB> comp_colors;
    auto color_for_component = [&](int comp_id) {
        auto it = comp_colors.find(comp_id);
        if (it != comp_colors.end()) {
            return it->second;
        }
        RGB color;
        int shift = 0;
        do {
            color.r = (comp_id * 79 + 43 + shift) % 256;
            color.g = (comp_id * 47 + 67 + shift) % 200;
            color.b = (comp_id * 113 + 89 + shift) % 256;
            shift += 31;
        } while (color.g > 150 && color.r < 100 && color.b < 100);
        comp_colors[comp_id] = color;
        return color;
    };

    for (int i = 0; i < nVerts; i++) {
        int comp_id = parent_of[i];
        if (comp_id < 0) {
            comp_id = 0;
        }

        int x = i % width;
        int y = i / width;
        RGB color = color_for_component(comp_id);
        result[y][x][0] = color.r;
        result[y][x][1] = color.g;
        result[y][x][2] = color.b;
    }

    return result;
}

#endif