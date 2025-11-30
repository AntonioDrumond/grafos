#ifndef TARJAN_H
#define TARJAN_H

#include "arborescence.h"
#include <stack>
#include <vector>
#include <algorithm>

class TarjanAlgorithm {
private:
    static constexpr int UNDEFINED = -1;

    struct TarjanState {
        std::vector<int> disc;           // Tempo de descoberta de cada vértice
        std::vector<int> low;            // Menor tempo alcançável
        std::vector<bool> on_stack;      // v tá na pilha?
        std::stack<int> stack;           // Pilha para construir SCCs
        std::vector<int> component_id;   // ID da componente de cada vértice
        int time;                        
        int num_components;              

        TarjanState(int n) 
            : disc(n, UNDEFINED), low(n, UNDEFINED), on_stack(n, false),
              component_id(n, UNDEFINED), time(0), num_components(0) {}
    };

    // DFS 
    void tarjan_dfs(const DirectedGraph& graph, int v, TarjanState& state) {
        // Inicializa tempo de descoberta e low-link
        state.disc[v] = state.low[v] = state.time++;
        state.stack.push(v);
        state.on_stack[v] = true;

        // Explora todos os vizinhos
        auto neighbors = graph.get_destinations_from(v);
        for (const auto& [w, cost] : neighbors) {
            if (state.disc[w] == UNDEFINED) {
                // Vizinho não visitado=fazer DFS recursivo
                tarjan_dfs(graph, w, state);
                state.low[v] = std::min(state.low[v], state.low[w]);
            } 
            else if (state.on_stack[w]) {
                // Possível ciclo 
                state.low[v] = std::min(state.low[v], state.disc[w]);
            }
        }

        // Se v é raiz de uma SCC, desempilha toda a componente
        if (state.low[v] == state.disc[v]) {
            int w;
            do {
                w = state.stack.top();
                state.stack.pop();
                state.on_stack[w] = false;
                state.component_id[w] = state.num_components;
            } while (w != v);
            state.num_components++;
        }
    }

public:
    ArborescenceResult find_strongly_connected_components(const DirectedGraph& graph) {
        int n = graph.vertex_count();
        ArborescenceResult result(n, -1); // Sem raiz única
        
        TarjanState state(n);

        // Executa DFS em todos os vértices não visitados
        for (int v = 0; v < n; v++) {
            if (state.disc[v] == UNDEFINED) {
                tarjan_dfs(graph, v, state);
            }
        }

        for (int i = 0; i < n; i++) {
            result.parent_of[i] = state.component_id[i];
        }

        result.is_complete = true;
        return result;
    }

    // Segmentação de imagem usando SCCs com pós-processamento
    ArborescenceResult segment_image(const DirectedGraph& graph, int min_component_size = 10) {
        int n = graph.vertex_count();
        
        // Encontrando componentes fortemente conexas
        ArborescenceResult scc_result = find_strongly_connected_components(graph);
        
        // Conta o tamanho de cada componente
        std::vector<int> component_sizes;
        int max_component = -1;
        for (int i = 0; i < n; i++) {
            int comp_id = scc_result.parent_of[i];
            if (comp_id > max_component) {
                max_component = comp_id;
            }
        }
        
        component_sizes.resize(max_component + 1, 0);
        for (int i = 0; i < n; i++) {
            component_sizes[scc_result.parent_of[i]]++;
        }

        // Mescla componentes pequenas com vizinhas maiores
        std::vector<int> final_component(n);
        for (int i = 0; i < n; i++) {
            int comp_id = scc_result.parent_of[i];
            
            if (component_sizes[comp_id] < min_component_size) {
                // Se a componente é muito pequena, ele tenta mesclar com um maior vizinho
                int best_neighbor = comp_id;
                int best_size = component_sizes[comp_id];
                
                auto neighbors = graph.get_destinations_from(i);
                for (const auto& [neighbor_idx, cost] : neighbors) {
                    int neighbor_comp = scc_result.parent_of[neighbor_idx];
                    if (component_sizes[neighbor_comp] > best_size) {
                        best_neighbor = neighbor_comp;
                        best_size = component_sizes[neighbor_comp];
                    }
                }
                final_component[i] = best_neighbor;
            } else {
                final_component[i] = comp_id;
            }
        }

        std::vector<int> id_mapping(max_component + 1, -1);
        int next_id = 0;
        for (int i = 0; i < n; i++) {
            int comp = final_component[i];
            if (id_mapping[comp] == -1) {
                id_mapping[comp] = next_id++;
            }
            final_component[i] = id_mapping[comp];
        }

        ArborescenceResult result(n, -1);
        result.parent_of = final_component;
        result.is_complete = true;
        return result;
    }
};

#endif
