#ifndef ARBORESCENCE_H
#define ARBORESCENCE_H

// Imports básicos para estruturas de dados e algoritmos
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

// Dependências do projeto
#ifndef EDGE_H
#include "edge.h"
#endif
#include "Graph.h"

// ================================================================================================
// ESTRUTURAS BÁSICAS PARA ARBORESCÊNCIAS
// ================================================================================================

/**
 * @brief Representa uma aresta direcionada (u → v) com peso
 * 
 * Uma aresta direcionada conecta dois vértices em uma única direção,
 * diferente das arestas bidirecionais usadas em MST
 */
struct DirectedEdge {
    int source;         // Vértice de origem (de onde sai a aresta)
    int target;         // Vértice de destino (para onde vai a aresta)  
    double cost;        // Custo/peso da aresta
    
    // Construtores
    DirectedEdge();
    DirectedEdge(int from, int to, double weight);
    
    // Utilitários
    void show() const;
    bool is_valid() const;
};

/**
 * @brief Grafo direcionado otimizado para algoritmos de arborescência
 * 
 * Armazena tanto arestas de saída quanto de entrada para cada vértice,
 * permitindo navegação eficiente em ambas as direções
 */
class DirectedGraph {
private:
    int max_vertices;                                           // Capacidade máxima do grafo
    int current_vertices;                                       // Número atual de vértices
    std::vector<std::unordered_map<int, double>> outgoing;     // [u][v] = peso da aresta u→v
    std::vector<std::unordered_map<int, double>> incoming;     // [v][u] = peso da aresta u→v

public:
    // Construção e destruição
    DirectedGraph(int capacity);
    ~DirectedGraph() = default;
    
    // === GERENCIAMENTO DE VÉRTICES ===
    bool add_vertex();                  // Adiciona um novo vértice
    void add_all_vertices();           // Adiciona todos os vértices possíveis até a capacidade
    int vertex_count() const;          // Retorna quantos vértices existem atualmente
    int capacity() const;              // Retorna capacidade máxima
    
    // === GERENCIAMENTO DE ARESTAS ===
    bool connect(int from, int to, double cost);           // Conecta vértice 'from' ao 'to' com custo
    bool disconnect(int from, int to);                     // Remove conexão from→to
    bool has_connection(int from, int to) const;           // Verifica se existe from→to
    double connection_cost(int from, int to) const;       // Obtém custo da conexão from→to
    int total_connections() const;                         // Conta total de arestas
    
    // === NAVEGAÇÃO NO GRAFO ===
    std::unordered_map<int, double> get_destinations_from(int vertex) const;   // Para onde o vértice aponta
    std::unordered_map<int, double> get_sources_to(int vertex) const;          // Quem aponta para o vértice
    std::vector<DirectedEdge> get_all_connections() const;                     // Todas as arestas do grafo
    
    // === UTILITÁRIOS ===
    void display() const;                                                      // Mostra estrutura do grafo
    bool is_reachable(int from, int to) const;                               // Verifica se 'to' é alcançável de 'from'
    
    // === CONVERSÃO ===
    static DirectedGraph build_from_mst(const WeightedGraph& mst, int root_pixel);  // Converte MST em grafo direcionado
};

// ================================================================================================
// RESULTADO DE ARBORESCÊNCIA
// ================================================================================================

/**
 * @brief Armazena o resultado de um algoritmo de arborescência
 * 
 * Uma arborescência é uma árvore direcionada com uma raiz única,
 * onde existe um caminho da raiz para todos os outros vértices
 */
struct ArborescenceResult {
    std::vector<int> parent_of;         // parent_of[v] = pai do vértice v na árvore (-1 se for raiz)
    std::vector<double> edge_costs;     // edge_costs[v] = custo da aresta que conecta parent_of[v] → v
    double total_tree_cost;             // Custo total de toda a arborescência
    int root_vertex;                    // Qual vértice é a raiz da árvore
    bool is_complete;                   // Se a arborescência foi construída com sucesso
    
    // Construtor
    ArborescenceResult(int num_vertices, int root);
    
    // === CONSULTAS DA ÁRVORE ===
    void display_tree() const;                         // Mostra a estrutura da árvore
    std::vector<int> get_children_of(int vertex) const;    // Retorna filhos diretos de um vértice
    int get_tree_depth_of(int vertex) const;              // Calcula profundidade do vértice na árvore
    bool is_ancestor(int possible_ancestor, int descendant) const;  // Verifica relação ancestral
    
    // === ANÁLISE DA SEGMENTAÇÃO ===
    std::vector<std::vector<int>> get_subtrees() const;           // Obtém todas as subárvores
    std::vector<int> get_path_to_root(int vertex) const;          // Caminho do vértice até a raiz
};

// ================================================================================================
// ALGORITMO DE ARBORESCÊNCIA DE CUSTO MÍNIMO
// ================================================================================================

/**
 * @brief Implementação do algoritmo de Gabow-Tarjan para arborescência de custo mínimo
 * 
 * Este algoritmo resolve o problema da "árvore direcionada de custo mínimo":
 * - Dado um grafo direcionado com pesos e uma raiz
 * - Encontra uma arborescência que conecta todos os vértices
 * - Minimiza o custo total das arestas escolhidas
 * 
 * Complexidade: O(E + V log V) com heaps de Fibonacci
 */
class MinimumArborescenceAlgorithm {
private:
    // === CONSTANTES ===
    static constexpr double INFINITE_COST = std::numeric_limits<double>::infinity();
    
    // === ESTRUTURAS AUXILIARES ===
    
    /**
     * @brief Representa um componente fortemente conexo durante contração de ciclos
     */
    struct CycleComponent {
        std::vector<int> vertices_in_cycle;     // Vértices que fazem parte deste ciclo
        int cycle_representative;               // Vértice que representa este ciclo após contração
        double cheapest_entry_cost;             // Menor custo para entrar neste ciclo
        int cheapest_entry_source;              // De onde vem a aresta mais barata para entrar
        
        CycleComponent();
    };
    
    // === MÉTODOS AUXILIARES PRIVADOS ===
    
    /**
     * @brief Para cada vértice, encontra a aresta de entrada mais barata
     */
    std::vector<DirectedEdge> find_cheapest_incoming_edges(const DirectedGraph& graph, int root);
    
    /**
     * @brief Detecta ciclos formados pelas arestas mais baratas
     */
    std::vector<std::vector<int>> detect_cycles_in_solution(const std::vector<DirectedEdge>& cheapest_edges, 
                                                           int num_vertices, int root);
    
    /**
     * @brief Contrai ciclos em super-vértices para resolver recursivamente
     */
    DirectedGraph contract_cycles(const DirectedGraph& original, 
                                 const std::vector<std::vector<int>>& cycles,
                                 std::vector<int>& vertex_mapping);

public:
    // === ALGORITMOS PRINCIPAIS ===
    
    /**
     * @brief Encontra arborescência de custo mínimo usando algoritmo de Gabow-Tarjan
     * 
     * @param graph Grafo direcionado onde buscar a arborescência
     * @param root_vertex Vértice que será a raiz da arborescência
     * @return Estrutura com a arborescência encontrada (ou inválida se impossível)
     */
    ArborescenceResult find_minimum_cost_arborescence(DirectedGraph& graph, int root_vertex);
    
    /**
     * @brief Versão simplificada similar ao algoritmo de Prim para grafos direcionados
     * 
     * Mais simples de implementar, mas pode não funcionar se houver ciclos.
     * Útil para casos onde o grafo já é quase uma árvore (como MSTs convertidas)
     * 
     * @param graph Grafo direcionado onde buscar a arborescência  
     * @param root_vertex Vértice que será a raiz da arborescência
     * @return Estrutura com a arborescência encontrada
     */
    ArborescenceResult build_arborescence_prim_style(DirectedGraph& graph, int root_vertex);
    
    // === UTILITÁRIOS PARA SEGMENTAÇÃO ===
    
    /**
     * @brief Constrói arborescência hierárquica para segmentação de imagens
     * 
     * Combina MST (conectividade) com arborescência (hierarquia) para criar
     * uma segmentação hierárquica onde diferentes níveis representam
     * diferentes granularidades de segmentação
     */
    ArborescenceResult build_segmentation_hierarchy(const WeightedGraph& mst, 
                                                   int image_width, int image_height,
                                                   int root_pixel);
};

#endif // ARBORESCENCE_H