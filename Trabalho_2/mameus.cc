#include "Ppm.h"
#include "mst.h"
#include <ctime>

double getRuntime (clock_t start, clock_t end) {
	return (((double)(end-start)/CLOCKS_PER_SEC));
}

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

    WeightedGraph G = WeightedGraph::from_ppm_matrix(image, width, height, 0.0);

	clock_t grayscale = clock();
    grayscaleImg(image, width, height);
    savePPM_matrix("grayscale.ppm", image, width, height);

	clock_t blur = clock();
    image = blurImg(image, width, height, 8);
    savePPM_matrix("blurred.ppm", image, width, height);

	clock_t sobel_start = clock();
	std::vector<std::vector<std::vector<int>>> sobel = sobelOperator(image, width, height);
    savePPM_matrix("sobel.ppm", sobel, width, height);

    clock_t graphFromMatrix = clock();
    WeightedGraph S = WeightedGraph::from_ppm_matrix(sobel, width, height, 15.0);

    clock_t kruskal = clock();
    WeightedGraph* T = kruskal_segmentation(G, &S, width, 1000);

//	    T->avg_colors_components();
    auto t = T->to_ppm_matrix(width, height);

    clock_t matrixFromGraph = clock();
    savePPM_matrix("result.ppm", t, width, height);

    clock_t end = clock();

    printf("Execution time:\n\n");
	printf("Matrix From image: %lf\n", getRuntime(start, mkImage));
	printf("Graph from matrix: %lf\n", getRuntime(mkImage, graphFromMatrix));
	printf("-> Grayscale: %lf\n", getRuntime(grayscale, blur));
	printf("-> Gaussian: %lf\n", getRuntime(blur, sobel_start));
	printf("-> Sobel: %lf\n", getRuntime(sobel_start, graphFromMatrix));
	printf("Graph from matrix: %lf\n", getRuntime(graphFromMatrix, kruskal));
	printf("Kruskal: %lf\n", getRuntime(kruskal, matrixFromGraph));
	printf("Matrix from graph: %lf\n", getRuntime(matrixFromGraph, end));
	printf("runtime: %lf\n", getRuntime(start, end));
    return 0;
}
