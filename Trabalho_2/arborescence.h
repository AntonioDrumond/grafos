#ifndef ARBORESCENCE_H
#define ARBORESCENCE_H

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

#ifndef EDGE_H
#include "edge.h"
#endif
#include "Graph.h"


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
    
    void display() const;                                                 
    bool is_reachable(int from, int to) const;                               // Verifica se o destino é alcançável a partir da origem
    
    static DirectedGraph build_from_mst(const WeightedGraph& mst, int root_pixel);  // Converte MST em grafo direcionado
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
    
    std::vector<std::vector<int>> get_subtrees() const;           // Todas as subárvores
    std::vector<int> get_path_to_root(int vertex) const;          // Caminho do vértice até a raiz
};


class MinimumArborescenceAlgorithm {
private:

    static constexpr double INFINITE_COST = std::numeric_limits<double>::infinity(); 

    struct CycleComponent {
        std::vector<int> vertices_in_cycle;     // Vértices que fazem parte deste ciclo
        int cycle_representative;               // Vértice que representa este ciclo após contração
        double cheapest_entry_cost;             // Menor custo para entrar neste ciclo
        int cheapest_entry_source;              // De onde vem a aresta mais barata para entrar
        
        CycleComponent();
    };
    
    std::vector<DirectedEdge> find_cheapest_incoming_edges(const DirectedGraph& graph, int root);
    
    std::vector<std::vector<int>> detect_cycles_in_solution(const std::vector<DirectedEdge>& cheapest_edges, 
                                                           int num_vertices, int root);

    DirectedGraph contract_cycles(const DirectedGraph& original, 
                                 const std::vector<std::vector<int>>& cycles,
                                 std::vector<int>& vertex_mapping);

public:
    
    ArborescenceResult find_minimum_cost_arborescence(DirectedGraph& graph, int root_vertex);
     
    ArborescenceResult build_arborescence_prim_style(DirectedGraph& graph, int root_vertex);

    ArborescenceResult build_segmentation_hierarchy(const WeightedGraph& mst, 
                                                   int image_width, int image_height,
                                                   int root_pixel);
};

#endif 