#include "CODIGO/GRAFO/Graph.h"
#include "CODIGO/OUTROS/Ppm.h"
#include "CODIGO/ALGORITMOS/tarjan.h"
#include "CODIGO/ALGORITMOS/gabow.h"
#include <chrono>
#include <cmath>
#include <cstring>
#include <iostream>
#include <random>

namespace {
void print_graph_info(const DirectedGraph &graph, int root) {
    std::cout << "--- Informações do Grafo ---\n";
    std::cout << "Vértices: " << graph.vertex_count() << "\n";
    std::cout << "Arestas: " << graph.total_connections() << "\n";
    
    // Verificar arestas de entrada
    int missing_edges = 0;
    for (int v = 0; v < graph.vertex_count(); ++v) {
        if (v == root) continue;
        auto sources = graph.get_sources_to(v);
        if (sources.empty()) {
            missing_edges++;
        }
    }
    std::cout << "Vértices sem aresta de entrada: " << missing_edges << "\n";
}

DirectedGraph make_synthetic_graph(int vertices, int out_degree, uint32_t seed) {
    DirectedGraph graph(vertices);
    graph.add_all_vertices();
    if (vertices == 0) {
        return graph;
    }

    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> weight_dist(0.1, 10.0); // Custos positivos

    // Garantir que cada vértice (exceto raiz) tenha pelo menos uma aresta de entrada
    for (int v = 1; v < vertices; ++v) {
        std::uniform_int_distribution<int> parent_dist(0, v - 1);
        int parent = parent_dist(rng);
        double weight = weight_dist(rng);
        graph.connect(parent, v, weight);
    }

    // Adicionar arestas extras
    for (int u = 0; u < vertices; ++u) {
        int current_out = static_cast<int>(graph.get_destinations_from(u).size());
        int desired = std::min(out_degree, vertices - 1);
        
        if (current_out >= desired) continue;

        for (int v = 0; v < vertices; ++v) {
            if (u == v) continue;
            if (current_out >= desired) break;
            
            if (!graph.has_connection(u, v)) {
                double weight = weight_dist(rng);
                graph.connect(u, v, weight);
                current_out++;
            }
        }
    }

    return graph;
}

DirectedGraph create_simple_test_graph() {
    DirectedGraph graph(5);
    graph.add_all_vertices();
    
    // Grafo simples sem ciclos para teste
    graph.connect(0, 1, 2.0);
    graph.connect(0, 2, 1.0);
    graph.connect(1, 3, 3.0);
    graph.connect(2, 4, 4.0);
    graph.connect(3, 4, 1.0);
    
    return graph;
}
}

int main(int argc, char *argv[]) {
    std::string input_path = "./input.ppm";
    int vertex_limit = 1000; // Reduzido para testes mais rápidos
    bool use_synthetic = false;
    bool use_image = false;
    int synthetic_vertices = 10;
    int synthetic_out_degree = 3;
    uint32_t synthetic_seed = 1337u;
    bool use_simple_test = true;

    for (int i = 1; i < argc; ++i) {
        if ((std::strcmp(argv[i], "--limit") == 0 || std::strcmp(argv[i], "-l") == 0) && i + 1 < argc) {
            vertex_limit = std::max(1, std::atoi(argv[++i]));
        } else if (std::strcmp(argv[i], "--synthetic") == 0 || std::strcmp(argv[i], "-s") == 0) {
            use_synthetic = true;
            use_simple_test = false;
            use_image = false;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                synthetic_vertices = std::max(2, std::atoi(argv[++i]));
            }
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                synthetic_out_degree = std::max(1, std::atoi(argv[++i]));
            }
        } else if (std::strcmp(argv[i], "--seed") == 0 && i + 1 < argc) {
            synthetic_seed = static_cast<uint32_t>(std::strtoul(argv[++i], nullptr, 10));
        } else if (std::strcmp(argv[i], "--image") == 0) {
            use_synthetic = false;
            use_simple_test = false;
            use_image = true;
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                input_path = argv[++i];
            }
        }
    }

    DirectedGraph graph(0);
    
    if (use_simple_test) {
        std::cout << "Usando grafo de teste simples...\n";
        graph = create_simple_test_graph();
    } else if (use_synthetic) {
        std::cout << "Gerando grafo sintético com " << synthetic_vertices
                  << " vértices e " << synthetic_out_degree << " arestas por vértice\n";
        graph = make_synthetic_graph(synthetic_vertices, synthetic_out_degree, synthetic_seed);
    } else if (use_image) {
        int width = 0, height = 0;
        std::vector<std::vector<std::vector<int>>> image;

        if (!loadPPM(input_path, image, width, height)) {
            std::cerr << "Não foi possível abrir " << input_path << "\n";
            return 1;
        }

        auto smoothed = blurImg(image, width, height, 2);
        WeightedGraph weighted = WeightedGraph::from_ppm_matrix(smoothed, width, height, 0.0);
        graph = DirectedGraph::from_weighted_graph(weighted);
        
        if (vertex_limit < graph.vertex_count()) {
            DirectedGraph limited(vertex_limit);
            limited.add_all_vertices();
            for (int u = 0; u < vertex_limit; ++u) {
                auto destinations = graph.get_destinations_from(u);
                for (const auto &entry : destinations) {
                    if (entry.first < vertex_limit) {
                        limited.connect(u, entry.first, entry.second);
                    }
                }
            }
            graph = limited;
            std::cout << "Grafo limitado para " << vertex_limit << " vértices\n";
        }
    } else {
        std::cout << "Nenhum modo escolhido explicitamente; use --image ou --synthetic para outros testes.\n";
        graph = create_simple_test_graph();
    }

    int root_vertex = 0;
    print_graph_info(graph, root_vertex);

    TarjanArborescence tarjan;
    GabowArborescence gabow;

    // Testar Tarjan
    auto start_tarjan = std::chrono::high_resolution_clock::now();
    auto tarjan_result = tarjan.find_min_arborescence(graph, root_vertex);
    auto end_tarjan = std::chrono::high_resolution_clock::now();
    auto tarjan_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_tarjan - start_tarjan).count();

    // Testar Gabow
    auto start_gabow = std::chrono::high_resolution_clock::now();
    auto gabow_result = gabow.find_min_arborescence(graph, root_vertex);
    auto end_gabow = std::chrono::high_resolution_clock::now();
    auto gabow_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_gabow - start_gabow).count();

    std::cout << "\n=== RESULTADOS ===\n";
    std::cout << "Tarjan: " << (tarjan_result.is_complete ? "SUCESSO" : "FALHA")
              << " | Custo: " << tarjan_result.total_tree_cost
              << " | Tempo: " << tarjan_time << "ms\n";

    std::cout << "Gabow:  " << (gabow_result.is_complete ? "SUCESSO" : "FALHA")
              << " | Custo: " << gabow_result.total_tree_cost
              << " | Tempo: " << gabow_time << "ms\n";

    if (tarjan_result.is_complete && gabow_result.is_complete) {
        double diff = std::abs(tarjan_result.total_tree_cost - gabow_result.total_tree_cost);
        std::cout << "Diferença de custo: " << diff << "\n";
        if (tarjan_time > 0) {
            std::cout << "Razão de tempo (Gabow/Tarjan): " << (gabow_time * 1.0 / tarjan_time) << "\n";
        } else if (gabow_time == 0) {
            std::cout << "Razão de tempo (Gabow/Tarjan): N/A (ambos 0ms)\n";
        } else {
            std::cout << "Razão de tempo (Gabow/Tarjan): infinita (Tarjan 0ms)\n";
        }
    }

    return 0;
}