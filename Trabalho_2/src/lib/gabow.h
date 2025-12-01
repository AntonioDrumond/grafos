#ifndef GABOW_ARBORESCENCE_H
#define GABOW_ARBORESCENCE_H

#include "arborescence.h"
#include <algorithm>
#include <limits>
#include <stack>
#include <vector>

class GabowArborescence {
private:
    static constexpr double INF = std::numeric_limits<double>::infinity();

public:
    ArborescenceResult find_min_arborescence(const DirectedGraph &graph, int root_vertex) {
        int n = graph.vertex_count();
        ArborescenceResult result(n, root_vertex);

        if (root_vertex < 0 || root_vertex >= n) {
            return result;
        }

        // Abordagem simplificada do Gabow
        auto min_in_edges = find_min_incoming_edges(graph, root_vertex);

        // Verificar se existe arborescência
        for (int v = 0; v < n; ++v) {
            if (v != root_vertex && !min_in_edges[v].is_valid()) {
                return result;
            }
        }

        std::vector<int> parent(n, -1);
        std::vector<double> cost(n, 0.0);
        double total_cost = 0.0;

        // Fase 1: Selecionar arestas mínimas
        for (int v = 0; v < n; ++v) {
            if (v != root_vertex) {
                parent[v] = min_in_edges[v].source;
                cost[v] = min_in_edges[v].cost;
                total_cost += cost[v];
            }
        }

        // Fase 2: Lidar com ciclos iterativamente
        bool changed;
        do {
            changed = false;
            auto cycles = find_cycles(parent, root_vertex);
            
            for (const auto &cycle : cycles) {
                // Quebrar ciclo removendo a aresta mais cara
                double max_cost = -1.0;
                int break_vertex = -1;
                
                for (int v : cycle) {
                    if (cost[v] > max_cost) {
                        max_cost = cost[v];
                        break_vertex = v;
                    }
                }

                if (break_vertex != -1) {
                    DirectedEdge alternative = find_alternative_edge(graph, break_vertex, parent[break_vertex]);
                    if (alternative.is_valid()) {
                        total_cost -= cost[break_vertex];
                        parent[break_vertex] = alternative.source;
                        cost[break_vertex] = alternative.cost;
                        total_cost += cost[break_vertex];
                        changed = true;
                    }
                }
            }
        } while (changed);

        // Verificar resultado final
        if (is_valid_arborescence(parent, root_vertex)) {
            result.parent_of = parent;
            result.edge_costs = cost;
            result.total_tree_cost = total_cost;
            result.is_complete = true;
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

    std::vector<std::vector<int>> find_cycles(const std::vector<int> &parent, int root) {
        int n = parent.size();
        std::vector<std::vector<int>> cycles;
        std::vector<int> visited(n, -1);

        for (int start = 0; start < n; ++start) {
            if (start == root || visited[start] != -1) continue;

            std::vector<int> path;
            int current = start;
            bool found_cycle = false;

            while (current != -1 && current != root && !found_cycle) {
                if (visited[current] == 0) {
                    // Ciclo detectado
                    auto it = std::find(path.begin(), path.end(), current);
                    if (it != path.end()) {
                        std::vector<int> cycle(it, path.end());
                        cycles.push_back(cycle);
                        found_cycle = true;
                        
                        // Marcar vértices do ciclo
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
                current = parent[current];
            }

            // Marcar vértices não-cíclicos
            for (int v : path) {
                if (visited[v] != 1) {
                    visited[v] = 1;
                }
            }
        }
        return cycles;
    }

    DirectedEdge find_alternative_edge(const DirectedGraph &graph, int target, int exclude_source) {
        double min_cost = INF;
        DirectedEdge best_edge;

        auto sources = graph.get_sources_to(target);
        for (const auto &[u, cost] : sources) {
            if (u != exclude_source && u != target && cost < min_cost) {
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
        int visited_count = 1;

        while (!stack.empty()) {
            int u = stack.top();
            stack.pop();

            for (int v = 0; v < n; ++v) {
                if (parent[v] == u && !visited[v]) {
                    visited[v] = true;
                    visited_count++;
                    stack.push(v);
                }
            }
        }

        return visited_count == n;
    }
};

#endif