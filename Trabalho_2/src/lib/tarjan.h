#ifndef TARJAN_ARBORESCENCE_H
#define TARJAN_ARBORESCENCE_H

#include "arborescence.h"
#include <algorithm>
#include <limits>
#include <vector>
#include <stack>

class TarjanArborescence {
private:
    static constexpr double INF = std::numeric_limits<double>::infinity();

public:
    ArborescenceResult find_min_arborescence(const DirectedGraph &graph, int root_vertex) {
        int n = graph.vertex_count();
        ArborescenceResult result(n, root_vertex);

        if (root_vertex < 0 || root_vertex >= n) {
            return result;
        }

        // Encontrar arestas de entrada mínimas
        auto min_in_edges = find_min_incoming_edges(graph, root_vertex);
        
        // Verificar se todos os vértices (exceto raiz) têm aresta de entrada
        for (int v = 0; v < n; ++v) {
            if (v != root_vertex && !min_in_edges[v].is_valid()) {
                return result;
            }
        }

        // Detectar ciclos
        auto cycles = find_cycles(min_in_edges, root_vertex);
        
        if (cycles.empty()) {
            // Sem ciclos - construir resultado diretamente
            result.total_tree_cost = 0.0;
            for (int v = 0; v < n; ++v) {
                if (v == root_vertex) {
                    result.parent_of[v] = -1;
                    result.edge_costs[v] = 0.0;
                } else {
                    result.parent_of[v] = min_in_edges[v].source;
                    result.edge_costs[v] = min_in_edges[v].cost;
                    result.total_tree_cost += min_in_edges[v].cost;
                }
            }
            result.is_complete = true;
        } else {
            // Abordagem iterativa para lidar com ciclos
            result = solve_with_cycles(graph, root_vertex, min_in_edges, cycles);
        }

        return result;
    }

private:
    std::vector<DirectedEdge> find_min_incoming_edges(const DirectedGraph &graph, int root) {
        int n = graph.vertex_count();
        std::vector<DirectedEdge> min_edges(n);
        std::vector<double> min_costs(n, INF);

        for (int v = 0; v < n; ++v) {
            if (v == root) continue;
            
            auto sources = graph.get_sources_to(v);
            for (const auto &[u, cost] : sources) {
                if (cost < min_costs[v]) {
                    min_costs[v] = cost;
                    min_edges[v] = DirectedEdge(u, v, cost);
                }
            }
        }
        return min_edges;
    }

    std::vector<std::vector<int>> find_cycles(const std::vector<DirectedEdge> &min_edges, int root) {
        int n = min_edges.size();
        std::vector<std::vector<int>> cycles;
        std::vector<int> visited(n, -1); // -1: não visitado, 0: visitando, 1: visitado

        for (int start = 0; start < n; ++start) {
            if (start == root || visited[start] == 1) continue;

            std::vector<int> path;
            int current = start;
            
            while (current != -1 && current != root) {
                if (visited[current] == 0) {
                    // Ciclo detectado - reconstruir
                    auto it = std::find(path.begin(), path.end(), current);
                    if (it != path.end()) {
                        std::vector<int> cycle(it, path.end());
                        cycles.push_back(cycle);
                        
                        // Marcar todos os vértices do ciclo como visitados
                        for (int v : cycle) {
                            visited[v] = 1;
                        }
                    }
                    break;
                }

                if (visited[current] == 1) {
                    break;
                }

                visited[current] = 0;
                path.push_back(current);
                current = min_edges[current].source;
            }

            // Marcar vértices não cíclicos como visitados
            for (int v : path) {
                if (visited[v] != 1) {
                    visited[v] = 1;
                }
            }
        }
        return cycles;
    }

    ArborescenceResult solve_with_cycles(const DirectedGraph &graph, int root,
                                        const std::vector<DirectedEdge> &min_edges,
                                        const std::vector<std::vector<int>> &cycles) {
        int n = graph.vertex_count();
        ArborescenceResult result(n, root);
        
        // Inicializar com arestas mínimas
        std::vector<int> parent(n, -1);
        std::vector<double> cost(n, 0.0);
        double total_cost = 0.0;
        
        for (int v = 0; v < n; ++v) {
            if (v != root) {
                parent[v] = min_edges[v].source;
                cost[v] = min_edges[v].cost;
                total_cost += cost[v];
            }
        }

        // Quebrar ciclos substituindo a aresta mais cara
        for (const auto &cycle : cycles) {
            double max_cost = -1.0;
            int break_vertex = -1;
            
            for (int v : cycle) {
                if (cost[v] > max_cost) {
                    max_cost = cost[v];
                    break_vertex = v;
                }
            }

            if (break_vertex != -1) {
                // Encontrar alternativa para quebrar o ciclo
                DirectedEdge alternative = find_alternative_edge(graph, break_vertex, parent[break_vertex], root);
                if (alternative.is_valid()) {
                    total_cost -= cost[break_vertex];
                    parent[break_vertex] = alternative.source;
                    cost[break_vertex] = alternative.cost;
                    total_cost += cost[break_vertex];
                }
            }
        }

        // Verificar se temos uma arborescência válida
        if (is_valid_arborescence(parent, root)) {
            result.parent_of = parent;
            result.edge_costs = cost;
            result.total_tree_cost = total_cost;
            result.is_complete = true;
        }

        return result;
    }

    DirectedEdge find_alternative_edge(const DirectedGraph &graph, int target, int exclude_source, int root) {
        double min_cost = INF;
        DirectedEdge best_edge;
        
        auto sources = graph.get_sources_to(target);
        for (const auto &[u, cost] : sources) {
            if (u != exclude_source && u != target && u != root && cost < min_cost) {
                min_cost = cost;
                best_edge = DirectedEdge(u, target, cost);
            }
        }
        
        return best_edge;
    }

    bool is_valid_arborescence(const std::vector<int> &parent, int root) {
        int n = parent.size();
        std::vector<bool> visited(n, false);
        std::stack<int> stack;
        
        stack.push(root);
        visited[root] = true;
        
        while (!stack.empty()) {
            int u = stack.top();
            stack.pop();
            
            for (int v = 0; v < n; ++v) {
                if (parent[v] == u && !visited[v]) {
                    visited[v] = true;
                    stack.push(v);
                }
            }
        }
        
        // Verificar se todos os vértices são alcançáveis da raiz
        for (int i = 0; i < n; ++i) {
            if (i != root && !visited[i]) {
                return false;
            }
        }
        
        return true;
    }
};

#endif