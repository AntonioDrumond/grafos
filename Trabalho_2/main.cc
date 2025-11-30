#include "Ppm.h"
#include "mst.h"
#include "gabow.h"
#include "tarjan.h"
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

    auto original_image = image;
    auto color_graph_input = original_image;
    color_graph_input = blurImg(color_graph_input, width, height, 3);

    WeightedGraph G = WeightedGraph::from_ppm_matrix(original_image, width, height, 0.0);

	clock_t grayscale = clock();
    grayscaleImg(image, width, height);
    savePPM_matrix("grayscale.ppm", image, width, height);

	clock_t blur = clock();
    image = blurImg(image, width, height, 5);
    savePPM_matrix("blurred.ppm", image, width, height);

	clock_t sobel_start = clock();
	std::vector<std::vector<std::vector<int>>> sobel = sobelOperator(image, width, height);
    savePPM_matrix("sobel.ppm", sobel, width, height);

    clock_t graphFromMatrix = clock();
    WeightedGraph S = WeightedGraph::from_color_and_gradient(
        color_graph_input,
        sobel,
        width,
        height,
        1.1,
        0.45
    );

    clock_t kruskal = clock();
    WeightedGraph* T = kruskal_segmentation(G, &S, width, 1550);

//	    T->avg_colors_components();
    auto t = T->to_ppm_matrix(width, height);

    clock_t matrixFromGraph = clock();
    savePPM_matrix("result.ppm", t, width, height);

    clock_t gabow_start = clock();
    DirectedGraph directed = DirectedGraph::from_weighted_graph(S);
    GabowAlgorithm gabow_algo;
    ArborescenceResult gabow_result = gabow_algo.segment_image(directed, 300.0, 20);

    // Recolor by component average for better visualization
    std::unordered_map<int, std::vector<int>> comps;
    int nverts = directed.vertex_count();
    for (int i = 0; i < nverts; ++i) {
        int root = gabow_result.parent_of[i];
        comps[root].push_back(i);
    }

    auto pixColors = G.getPixColor();
    std::unordered_map<int, RGB> avgColor;
    for (auto &kv : comps) {
        long long sr = 0, sg = 0, sb = 0;
        for (int idx : kv.second) {
            sr += pixColors[idx].r;
            sg += pixColors[idx].g;
            sb += pixColors[idx].b;
        }
        int m = (int)kv.second.size();
        RGB c;
        c.r = static_cast<int>(sr / m);
        c.g = static_cast<int>(sg / m);
        c.b = static_cast<int>(sb / m);
        avgColor[kv.first] = c;
    }

    std::vector<std::vector<std::vector<int>>> gabow_avg(height, std::vector<std::vector<int>>(width, std::vector<int>(3)));
    for (int i = 0; i < nverts; ++i) {
        int root = gabow_result.parent_of[i];
        RGB c = avgColor[root];
        int x = i % width;
        int y = i / width;
        gabow_avg[y][x][0] = c.r;
        gabow_avg[y][x][1] = c.g;
        gabow_avg[y][x][2] = c.b;
    }
    savePPM_matrix("result_2.ppm", gabow_avg, width, height);

    clock_t tarjan_start = clock();
    TarjanAlgorithm tarjan_algo;
    ArborescenceResult tarjan_result = tarjan_algo.segment_image(directed, 15);

    // Recolor por componente média para melhor visualização
    std::unordered_map<int, std::vector<int>> tarjan_comps;
    for (int i = 0; i < nverts; ++i) {
        int comp = tarjan_result.parent_of[i];
        tarjan_comps[comp].push_back(i);
    }

    std::unordered_map<int, RGB> tarjan_avgColor;
    for (auto &kv : tarjan_comps) {
        long long sr = 0, sg = 0, sb = 0;
        for (int idx : kv.second) {
            sr += pixColors[idx].r;
            sg += pixColors[idx].g;
            sb += pixColors[idx].b;
        }
        int m = (int)kv.second.size();
        RGB c;
        c.r = static_cast<int>(sr / m);
        c.g = static_cast<int>(sg / m);
        c.b = static_cast<int>(sb / m);
        tarjan_avgColor[kv.first] = c;
    }

    std::vector<std::vector<std::vector<int>>> tarjan_avg(height, std::vector<std::vector<int>>(width, std::vector<int>(3)));
    for (int i = 0; i < nverts; ++i) {
        int comp = tarjan_result.parent_of[i];
        RGB c = tarjan_avgColor[comp];
        int x = i % width;
        int y = i / width;
        tarjan_avg[y][x][0] = c.r;
        tarjan_avg[y][x][1] = c.g;
        tarjan_avg[y][x][2] = c.b;
    }
    savePPM_matrix("result_3.ppm", tarjan_avg, width, height);

    clock_t end = clock();

    printf("Execution time:\n\n");
	printf("Matrix From image: %lf\n", getRuntime(start, mkImage));
	printf("Graph from matrix: %lf\n", getRuntime(mkImage, graphFromMatrix));
	printf("-> Grayscale: %lf\n", getRuntime(mkImage, blur));
	printf("-> Gaussian: %lf\n", getRuntime(blur, sobel_start));
	printf("-> Sobel: %lf\n", getRuntime(sobel_start, graphFromMatrix));
	printf("Kruskal: %lf\n", getRuntime(kruskal, matrixFromGraph));
	printf("Matrix from graph: %lf\n", getRuntime(matrixFromGraph, gabow_start));
    printf("Gabow: %lf\n", getRuntime(gabow_start, tarjan_start));
	printf("Tarjan: %lf\n", getRuntime(tarjan_start, end));
	printf("\nTotal runtime Kruskal: %lf\n", getRuntime(start, gabow_start));
	printf("Total runtime Gabow: %lf\n", getRuntime(start, mkImage) + getRuntime(mkImage, graphFromMatrix) + getRuntime(gabow_start, tarjan_start));
	printf("Total runtime Tarjan: %lf\n", getRuntime(start, mkImage) + getRuntime(mkImage, graphFromMatrix) + getRuntime(tarjan_start, end));
    return 0;
}