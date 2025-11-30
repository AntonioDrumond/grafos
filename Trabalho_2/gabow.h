#include "arborescence.h"

class GabowAlgorithm {
private:

    static constexpr double INFINITE_COST = std::numeric_limits<double>::infinity(); 

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
    
    std::vector<std::vector<int>> detect_cycles_in_solution(
        const std::vector<DirectedEdge>& cheapest_edges, int num_vertices, int root) {
        
        std::vector<std::vector<int>> cycles;
        std::vector<bool> visited(num_vertices, false);
        std::vector<bool> in_current_path(num_vertices, false);
        
        for (int start = 0; start < num_vertices; start++) {
            if (start == root || visited[start]) continue;
            
            std::vector<int> path;
            std::fill(in_current_path.begin(), in_current_path.end(), false);
            
            int current = start;
            
            while (current != -1 && current != root && !visited[current]) {
                
                if (in_current_path[current]) {
                    std::vector<int> cycle;
                    bool in_cycle = false;
                    
                    for (int vertex : path) {
                        if (vertex == current) in_cycle = true;
                        if (in_cycle) cycle.push_back(vertex);
                    }
                    
                    if (!cycle.empty()) {
                        cycles.push_back(cycle);
                    }
                    break;
                }
                
                path.push_back(current);
                in_current_path[current] = true;
                
                if (cheapest_edges[current].source != -1) {
                    current = cheapest_edges[current].source;
                } else {
                    break;
                }
            }
            
            for (int vertex : path) {
                visited[vertex] = true;
            }
        }
        
        return cycles;
    }

    DirectedGraph contract_cycles(const DirectedGraph& original, 
                                  const std::vector<std::vector<int>>& cycles,
                                  std::vector<int>& vertex_mapping) {
        
        int n = original.vertex_count();
        vertex_mapping.resize(n);
        
        for (int i = 0; i < n; i++) {
            vertex_mapping[i] = i;
        }
        
        int next_id = n;
        
        for (const auto& cycle : cycles) {
            for (int vertex : cycle) {
                vertex_mapping[vertex] = next_id;
            }
            next_id++;
        }
        
        std::unordered_set<int> unique_ids(vertex_mapping.begin(), vertex_mapping.end());
        int contracted_size = unique_ids.size();
        
        DirectedGraph contracted(contracted_size);
        contracted.add_all_vertices();
        
        std::unordered_map<int, int> id_to_index;
        int index = 0;
        for (int id : unique_ids) {
            id_to_index[id] = index++;
        }
        
        auto all_edges = original.get_all_connections();
        for (const auto& edge : all_edges) {
            int from_contracted = id_to_index[vertex_mapping[edge.source]];
            int to_contracted = id_to_index[vertex_mapping[edge.target]];
            
            if (from_contracted != to_contracted) {
                if (!contracted.has_connection(from_contracted, to_contracted) ||
                    edge.cost < contracted.connection_cost(from_contracted, to_contracted)) {
                    contracted.connect(from_contracted, to_contracted, edge.cost);
                }
            }
        }
        
        return contracted;
    }

public:
    
    ArborescenceResult find_minimum_cost_arborescence(DirectedGraph& graph, int root_vertex) {
        int n = graph.vertex_count();
        ArborescenceResult result(n, root_vertex);
        
        if (root_vertex < 0 || root_vertex >= n) {
            return result;
        }
        
        auto cheapest_edges = find_cheapest_incoming_edges(graph, root_vertex);
        
        for (int v = 0; v < n; v++) {
            if (v == root_vertex) continue;
            if (cheapest_edges[v].source == -1) {
                return result;
            }
        }
        
        auto cycles = detect_cycles_in_solution(cheapest_edges, n, root_vertex);
        
        if (cycles.empty()) {
            result.total_tree_cost = 0.0;
            for (int v = 0; v < n; v++) {
                if (v == root_vertex) {
                    result.parent_of[v] = -1;
                    result.edge_costs[v] = 0.0;
                } else {
                    result.parent_of[v] = cheapest_edges[v].source;
                    result.edge_costs[v] = cheapest_edges[v].cost;
                    result.total_tree_cost += cheapest_edges[v].cost;
                }
            }
            result.is_complete = true;
            
        } else {
            DirectedGraph modified_graph = graph;
            
            for (const auto& cycle : cycles) {
                double max_cost = -1.0;
                int worst_from = -1, worst_to = -1;
                
                for (int v : cycle) {
                    if (cheapest_edges[v].cost > max_cost) {
                        max_cost = cheapest_edges[v].cost;
                        worst_from = cheapest_edges[v].source;
                        worst_to = cheapest_edges[v].target;
                    }
                }
                
                if (worst_from != -1 && worst_to != -1) {
                    modified_graph.disconnect(worst_from, worst_to);
                }
            }
            
            return find_minimum_cost_arborescence(modified_graph, root_vertex);
        }
        
        return result;
    }

    ArborescenceResult build_segmentation_hierarchy(DirectedGraph& directed_graph, int root_pixel) {
        ArborescenceResult result = find_minimum_cost_arborescence(directed_graph, root_pixel);
        return result;
    }
};
