#include "Ppm.h"
#include "mst.h"
#include <ctime>

int main (int argc, char *argv[]) {
	clock_t start = clock();
    int width, height;

    std::vector<std::vector<std::vector<int>>> image; // estrutura para armazenar uma imagem representada por uma matriz tridimensional dinâmica
    if (!loadPPM("./input.ppm", image, width, height)) {
        std::cout<< "nao foi possivel abrir a imagem"<<std::endl;

    }
    // std::cout<< "valor da largura: "<<width<<std::endl;
    // std::cout<< "valor da altura: "<<height<<std::endl;

//	    print_ppm(image, width, height);

    WeightedGraph g = WeightedGraph::from_ppm_matrix(image, width, height, true);
	WeightedGraph* T = kruskal_segmentation(&g, 6, width);

    T->avg_colors_components();
    auto t = T->to_ppm_matrix(width, height);
    savePPM_matrix("result.ppm", t, width, height);

    std::cout << "\n\n";
//	    T->print_raw();
    std::cout << "\n\n";
    // g.print_csacademy();

	clock_t end = clock();
	double time = (double)(end-start);
	printf("runtime: %lf\n", (time/CLOCKS_PER_SEC));

    return 0;
}
