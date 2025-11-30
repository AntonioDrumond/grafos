#ifndef EDMONDS_H
#define EDMONDS_H

#include "arborescence.h"
#include <algorithm>
#include <limits>
#include <numeric>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class EdmondsAlgorithm {
private:

    static constexpr double INFINITE_COST = std::numeric_limits<double>::infinity();

    struct InternalResult {
        bool success;
        std::vector<int> parent;
        std::vector<double> edge_costs;

        InternalResult(int n = 0)
            : success(true), parent(n, -1), edge_costs(n, 0.0) {}
    };

    struct ContractedEdgeInfo {
        int from_component;
        int to_component;
        double adjusted_cost;
        int original_source;
        int original_target;
        double original_cost;
    };

    struct CycleDetectionResult {
        std::vector<int> cycle_id_of_vertex;
        std::vector<std::vector<int>> cycles;
    };
    
    struct UnionFind {
        std::vector<int> parent;
        std::vector<int> size;
        std::vector<double> internal_cost; // Custo interno máximo da componente

        UnionFind(int n) : parent(n), size(n, 1), internal_cost(n, 0.0) {
            for (int i = 0; i < n; i++) {
                parent[i] = i;
            }
        }

        int find(int x) {
            if (parent[x] != x) {
                parent[x] = find(parent[x]);
            }
            return parent[x];
        }

        bool join(int u, int v, double edge_weight, double k) {
            int root_u = find(u);
            int root_v = find(v);

            if (root_u == root_v) {
                return false;
            }

            // Limiar Felzenszwalb: MInt(C1,C2) = min(Int(C1)+k/|C1|, Int(C2)+k/|C2|)
            double threshold_u = internal_cost[root_u] + k / size[root_u];
            double threshold_v = internal_cost[root_v] + k / size[root_v];
            double threshold = std::min(threshold_u, threshold_v);

            // Funde apenas se a diferença for pequena o suficiente
            if (edge_weight <= threshold) {
                // Une os conjuntos
                if (size[root_u] < size[root_v]) {
                    parent[root_u] = root_v;
                    size[root_v] += size[root_u];
                    // Distância interna é o peso da aresta de fusão
                    internal_cost[root_v] = edge_weight;
                } else {
                    parent[root_v] = root_u;
                    size[root_u] += size[root_v];
                    internal_cost[root_u] = edge_weight;
                }
                return true;
            }
            return false;
        }

        void force_merge(int u, int v, double weight_hint) {
            int root_u = find(u);
            int root_v = find(v);
            if (root_u == root_v) {
                return;
            }
            if (size[root_u] < size[root_v]) {
                std::swap(root_u, root_v);
            }
            parent[root_v] = root_u;
            size[root_u] += size[root_v];
            // Mantém o maior custo interno das duas componentes
            internal_cost[root_u] = std::max(internal_cost[root_u], internal_cost[root_v]);
        }
    };

    struct CycleComponent {
        std::vector<int> vertices_in_cycle;
        int cycle_representative;
        double cheapest_entry_cost;
        int cheapest_entry_source;
        
        CycleComponent() 
            : cycle_representative(-1), cheapest_entry_cost(INFINITE_COST), cheapest_entry_source(-1) {}
    };
    
    std::vector<DirectedEdge> find_cheapest_incoming_edges(const DirectedGraph& graph, int root) {
        int n = graph.vertex_count();
        std::vector<DirectedEdge> cheapest_edges(n);
        std::vector<double> min_costs(n, INFINITE_COST);
        
        for (int v = 0; v < n; v++) {
            if (v == root) continue;
            
            auto sources = graph.get_sources_to(v);
            for (const auto& [u, cost] : sources) {
                if (cost < min_costs[v]) {
                    min_costs[v] = cost;
                    cheapest_edges[v] = DirectedEdge(u, v, cost);
                }
            }
        }
        
        return cheapest_edges;
    }
    
    CycleDetectionResult detect_cycles(const std::vector<DirectedEdge>& cheapest_edges,
                                       int num_vertices,
                                       int root) {
        CycleDetectionResult result;
        result.cycle_id_of_vertex.assign(num_vertices, -1);
        std::vector<int> visit_tag(num_vertices, -1);
        int cycle_index = 0;

        for (int start = 0; start < num_vertices; ++start) {
            if (start == root || result.cycle_id_of_vertex[start] != -1) {
                continue;
            }

            int current = start;
            while (current != root && current != -1 && result.cycle_id_of_vertex[current] == -1 && visit_tag[current] != start) {
                visit_tag[current] = start;
                current = cheapest_edges[current].source;
            }

            if (current != root && current != -1 && result.cycle_id_of_vertex[current] == -1) {
                std::vector<int> cycle;
                int node = current;
                do {
                    cycle.push_back(node);
                    result.cycle_id_of_vertex[node] = cycle_index;
                    node = cheapest_edges[node].source;
                } while (node != current && node != -1);
                result.cycles.push_back(cycle);
                cycle_index++;
            }
        }

        return result;
    }

    static long long encode_edge_key(int from, int to) {
        return (static_cast<long long>(from) << 32) ^ (static_cast<unsigned long long>(to) & 0xffffffffULL);
    }

    InternalResult run_chu_liu(const DirectedGraph& graph, int root_vertex) {
        int n = graph.vertex_count();
        InternalResult result(n);

        if (n == 0 || root_vertex < 0 || root_vertex >= n) {
            result.success = false;
            return result;
        }

        auto cheapest_edges = find_cheapest_incoming_edges(graph, root_vertex);

        for (int v = 0; v < n; ++v) {
            if (v == root_vertex) {
                continue;
            }
            if (cheapest_edges[v].source == -1) {
                result.success = false;
                return result;
            }
        }

        auto cycle_detection = detect_cycles(cheapest_edges, n, root_vertex);

        if (cycle_detection.cycles.empty()) {
            for (int v = 0; v < n; ++v) {
                if (v == root_vertex) {
                    continue;
                }
                result.parent[v] = cheapest_edges[v].source;
                result.edge_costs[v] = cheapest_edges[v].cost;
            }
            return result;
        }

        int cycle_count = static_cast<int>(cycle_detection.cycles.size());
        std::vector<int> component_id(n, -1);
        for (int v = 0; v < n; ++v) {
            if (cycle_detection.cycle_id_of_vertex[v] != -1) {
                component_id[v] = cycle_detection.cycle_id_of_vertex[v];
            }
        }

        int next_id = cycle_count;
        for (int v = 0; v < n; ++v) {
            if (component_id[v] == -1) {
                component_id[v] = next_id++;
            }
        }

        int contracted_vertices = next_id;
        int contracted_root = component_id[root_vertex];

        DirectedGraph contracted(contracted_vertices);
        contracted.add_all_vertices();

        std::unordered_map<long long, ContractedEdgeInfo> edge_mapping;
        edge_mapping.reserve(graph.total_connections());

        auto all_edges = graph.get_all_connections();
        for (const auto& edge : all_edges) {
            int from_comp = component_id[edge.source];
            int to_comp = component_id[edge.target];

            if (from_comp == to_comp) {
                continue;
            }

            double adjusted_cost = edge.cost;
            if (cycle_detection.cycle_id_of_vertex[edge.target] != -1) {
                adjusted_cost -= cheapest_edges[edge.target].cost;
            }

            long long key = encode_edge_key(from_comp, to_comp);
            bool has_connection = contracted.has_connection(from_comp, to_comp);
            double current_cost = has_connection ? contracted.connection_cost(from_comp, to_comp) : INFINITE_COST;

            if (!has_connection || adjusted_cost < current_cost) {
                contracted.connect(from_comp, to_comp, adjusted_cost);
                edge_mapping[key] = ContractedEdgeInfo{from_comp, to_comp, adjusted_cost,
                                                       edge.source, edge.target, edge.cost};
            }
        }

        auto contracted_result = run_chu_liu(contracted, contracted_root);
        if (!contracted_result.success) {
            result.success = false;
            return result;
        }

        for (int v = 0; v < n; ++v) {
            if (v == root_vertex) {
                continue;
            }
            result.parent[v] = cheapest_edges[v].source;
            result.edge_costs[v] = cheapest_edges[v].cost;
        }

        for (int comp = 0; comp < contracted_vertices; ++comp) {
            if (comp == contracted_root) {
                continue;
            }
            int parent_comp = contracted_result.parent[comp];
            if (parent_comp == -1) {
                continue;
            }

            long long key = encode_edge_key(parent_comp, comp);
            auto mapped_edge = edge_mapping.find(key);
            if (mapped_edge == edge_mapping.end()) {
                result.success = false;
                return result;
            }

            int target_vertex = mapped_edge->second.original_target;
            result.parent[target_vertex] = mapped_edge->second.original_source;
            result.edge_costs[target_vertex] = mapped_edge->second.original_cost;
        }

        return result;
    }

public:
    
    ArborescenceResult find_minimum_cost_arborescence(DirectedGraph& graph, int root_vertex) {
        int n = graph.vertex_count();
        ArborescenceResult result(n, root_vertex);
        
        if (root_vertex < 0 || root_vertex >= n) {
            return result;
        }
        auto internal_result = run_chu_liu(graph, root_vertex);
        if (!internal_result.success) {
            return result;
        }

        result.total_tree_cost = 0.0;
        for (int v = 0; v < n; ++v) {
            if (v == root_vertex) {
                result.parent_of[v] = -1;
                result.edge_costs[v] = 0.0;
            } else {
                result.parent_of[v] = internal_result.parent[v];
                result.edge_costs[v] = internal_result.edge_costs[v];
                if (result.parent_of[v] != -1) {
                    result.total_tree_cost += result.edge_costs[v];
                } else {
                    result.is_complete = false;
                    return result;
                }
            }
        }
        result.is_complete = true;
        return result;
    }

    ArborescenceResult build_segmentation_hierarchy(DirectedGraph& directed_graph, int root_pixel) {
        ArborescenceResult result = find_minimum_cost_arborescence(directed_graph, root_pixel);
        return result;
    }
    
    // Método de limpeza de ruído (Felzenszwalb)
    ArborescenceResult segment_image(const DirectedGraph& graph, double k, int min_size = 0) {
        int n = graph.vertex_count();
        ArborescenceResult result(n, -1); // Sem raiz única
        
        // Arestas direcionadas em pares não direcionados com menor custo
        std::vector<DirectedEdge> edges = graph.get_minimum_undirected_edges();

        // Ordena arestas por peso crescente
        std::sort(edges.begin(), edges.end(),
                  [](const DirectedEdge& a, const DirectedEdge& b) {
                      return a.cost < b.cost;
                  });
        
        // Union-Find para segmentação
        UnionFind uf(n);
        
        // Segmentação principal
        for (const auto& edge : edges) {
            uf.join(edge.source, edge.target, edge.cost, k);
        }
        
        // Compressão final de caminhos para consistência
        for (int i = 0; i < n; i++) {
            uf.find(i);
        }

        for (int i = 0; i < n; i++) {
            result.parent_of[i] = uf.find(i);
        }
        
        result.is_complete = true;
        return result;
    }
};

#endif