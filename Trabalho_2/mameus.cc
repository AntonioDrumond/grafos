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
    clock_t mkImage = clock();
    // std::cout<< "valor da largura: "<<width<<std::endl;
    // std::cout<< "valor da altura: "<<height<<std::endl;

//	    print_ppm(image, width, height);

    image = blurImg(image, width, height, 7);
    savePPM_matrix("blurred.ppm", image, width, height);

    lightenImg(image, width, height, 1.4);
    savePPM_matrix("light.ppm", image, width, height);

    WeightedGraph g = WeightedGraph::from_ppm_matrix(image, width, height, true);

    clock_t graphFromMatrix = clock();
    WeightedGraph* T = kruskal_segmentation(&g, 6, width);
    clock_t kruskal = clock();

    T->avg_colors_components();
    clock_t painting = clock();
    auto t = T->to_ppm_matrix(width, height);
    clock_t matrixFromGraph = clock();
    savePPM_matrix("result.ppm", t, width, height);
    clock_t saving = clock();

    std::cout << "\n\n";
//	    T->print_raw();
    std::cout << "\n\n";
    // g.print_csacademy();

    clock_t end = clock();
    double time = (double)(end-start);
    printf("Execution time:\n\nMatrix From image: %lf\nGraph from matrix: %lf\nKruskal: %lf\nAvg colors: %lf\nMatrix from graph: %lf\nSave matrix: %lf\n", (((double)(mkImage-start))/CLOCKS_PER_SEC), (((double)(graphFromMatrix-mkImage))/CLOCKS_PER_SEC), (((double)(kruskal-graphFromMatrix))/CLOCKS_PER_SEC), (((double)(painting-kruskal))/CLOCKS_PER_SEC), (((double)(matrixFromGraph-painting))/CLOCKS_PER_SEC), (((double)(saving-matrixFromGraph))/CLOCKS_PER_SEC));
    printf("runtime: %lf\n", (time/CLOCKS_PER_SEC));

    return 0;
}
