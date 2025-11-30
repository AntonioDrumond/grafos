#include "CODIGO/OUTROS/Ppm.h"
#include "CODIGO/ALGORITMOS/mst.h"
#include "CODIGO/ALGORITMOS/edmonds.h"
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
    clock_t after_image_load = clock();
    // std::cout<< "valor da largura: "<<width<<std::endl;
    // std::cout<< "valor da altura: "<<height<<std::endl;

//	    print_ppm(image, width, height);

    auto original_image = image;
    auto color_graph_input = original_image;
    color_graph_input = blurImg(color_graph_input, width, height, 3);

    WeightedGraph G = WeightedGraph::from_ppm_matrix(original_image, width, height, 0.0);

	grayscaleImg(image, width, height);
    savePPM_matrix("grayscale.ppm", image, width, height);
	clock_t after_grayscale = clock();

    image = blurImg(image, width, height, 5);
    savePPM_matrix("blurred.ppm", image, width, height);
	clock_t after_blur = clock();

	std::vector<std::vector<std::vector<int>>> sobel = sobelOperator(image, width, height);
    savePPM_matrix("sobel.ppm", sobel, width, height);
	clock_t after_sobel = clock();

    WeightedGraph S = WeightedGraph::from_color_and_gradient(
        color_graph_input,
        sobel,
        width,
        height,
        1.1,
        0.45
    );
    clock_t after_graph_from_matrix = clock();

    WeightedGraph* T = kruskal_segmentation(G, &S, width, 1550);
//	    T->avg_colors_components();
    auto t = T->to_ppm_matrix(width, height);
    clock_t after_kruskal = clock();

    savePPM_matrix("result.ppm", t, width, height);
    clock_t after_matrix_from_graph = clock();

    DirectedGraph directed = DirectedGraph::from_weighted_graph(S);
    EdmondsAlgorithm edmonds_algo;
    ArborescenceResult edmonds_result = edmonds_algo.segment_image(directed, 300.0, 20);

    // Recolor by component average for better visualization
    std::unordered_map<int, std::vector<int>> comps;
    int nverts = directed.vertex_count();
    for (int i = 0; i < nverts; ++i) {
        int root = edmonds_result.parent_of[i];
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

    std::vector<std::vector<std::vector<int>>> edmonds_avg(height, std::vector<std::vector<int>>(width, std::vector<int>(3)));
    for (int i = 0; i < nverts; ++i) {
        int root = edmonds_result.parent_of[i];
        RGB c = avgColor[root];
        int x = i % width;
        int y = i / width;
        edmonds_avg[y][x][0] = c.r;
        edmonds_avg[y][x][1] = c.g;
        edmonds_avg[y][x][2] = c.b;
    }
    savePPM_matrix("result_2.ppm", edmonds_avg, width, height);

    clock_t finish = clock();

    double time_matrix_from_image = getRuntime(start, after_image_load);
    double time_grayscale = getRuntime(after_image_load, after_grayscale);
    double time_gaussian = getRuntime(after_grayscale, after_blur);
    double time_sobel = getRuntime(after_blur, after_sobel);
    double time_graph_from_matrix = getRuntime(after_sobel, after_graph_from_matrix);
    double time_kruskal = getRuntime(after_graph_from_matrix, after_kruskal);
    double time_matrix_from_graph = getRuntime(after_kruskal, after_matrix_from_graph);
    double time_edmonds = getRuntime(after_matrix_from_graph, finish);

    printf("Execution time:\n\n");
	printf("Matrix from image: %lf\n", time_matrix_from_image);
	printf("-> Grayscale: %lf\n", time_grayscale);
	printf("-> Gaussian: %lf\n", time_gaussian);
	printf("-> Sobel: %lf\n", time_sobel);
	printf("Graph from matrix: %lf\n", time_graph_from_matrix);
	printf("Kruskal: %lf\n", time_kruskal);
	printf("Matrix from graph: %lf\n", time_matrix_from_graph);
    printf("Edmonds: %lf\n", time_edmonds);
    return 0;
}