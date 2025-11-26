#include "Ppm.h"
#include "Graph.h"

int main (int argc, char *argv[]) {
    int width, height;

    std::vector<std::vector<std::vector<int>>> image; // estrutura para armazenar uma imagem representada por uma matriz tridimensional dinâmica
    std::cout << "\n";
    if (!loadPPM("./input.ppm", image, width, height)) {
        std::cout<< "nao foi possivel abrir a imagem"<<std::endl;

    }
    // std::cout<< "valor da largura: "<<width<<std::endl;
    // std::cout<< "valor da altura: "<<height<<std::endl;

    print_ppm(image, width, height);

    WeightedGraph g = WeightedGraph::from_ppm_matrix(image, width, height, true);

    g.avg_colors_components();
    auto t = g.to_ppm_matrix(width, height);
    savePPM_matrix("result.ppm", t, width, height);

    std::cout << "\n\n";
    g.print_raw();
    std::cout << "\n\n";
    // g.print_csacademy();

    return 0;
}
