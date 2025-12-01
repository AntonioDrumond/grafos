#include "util/Ppm.h"
#include "lib/felzenszwalb.h"
#include "lib/edmonds.h"
#include <ctime>
#include <filesystem>

double getRuntime (clock_t start, clock_t end) {
	return (((double)(end-start)/CLOCKS_PER_SEC));
}

int main (int argc, char *argv[]) {
    clock_t start = clock();
    int width, height;

	if (!std::filesystem::exists("input.ppm")) {
		std::cout << "No ppm input file detected!\nTo convert an image, run the code at [src/util/png_to_ppm.py] with a png file as a parameter.\n";
		return 0;
	}

    std::vector<std::vector<std::vector<int>>> image; // estrutura para armazenar uma imagem representada por uma matriz tridimensional dinâmica
    if (!loadPPM("./input.ppm", image, width, height)) {
        std::cout<< "nao foi possivel abrir a imagem"<<std::endl;
    }
    auto original_image = image;
    auto color_graph_input = original_image;

    clock_t after_image_load = clock();

    WeightedGraph G = WeightedGraph::from_ppm_matrix(original_image, width, height, 0.0);

	clock_t after_initial_graph_created = clock();

	grayscaleImg(image, width, height);
    savePPM_matrix("grayscale.ppm", image, width, height);

	clock_t after_grayscale = clock();

    image = blurImg(image, width, height, 5);
    color_graph_input = blurImg(color_graph_input, width, height, 3);
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
    auto t = T->to_ppm_matrix(width, height);
    clock_t after_kruskal = clock();

    savePPM_matrix("Felzenszwalb.ppm", t, width, height);
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
        int m = static_cast<int>(kv.second.size());
        if (m == 0) {
            continue;
        }
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
    savePPM_matrix("Edmonds.ppm", edmonds_avg, width, height);

    clock_t finish = clock();
    printf("Execution time --\n\n");
	printf("Matrix from image: %lf\n", getRuntime(start, after_image_load));
	printf("Original graph from matrix: %lf\n", getRuntime(after_image_load, after_initial_graph_created));
	printf("Preprocessing:\n-> Grayscale: %lf\n", getRuntime(after_initial_graph_created, after_grayscale));
	printf("-> Gaussian: %lf\n", getRuntime(after_grayscale, after_blur));
	printf("-> Sobel: %lf\n", getRuntime(after_blur, after_sobel));
	printf("Preprocessed graph from matrix: %lf\n", getRuntime(after_sobel, after_graph_from_matrix));
	printf("Felzenszwalb: %lf\n", getRuntime(after_graph_from_matrix, after_kruskal));
	printf("Matrix from graph: %lf\n", getRuntime(after_kruskal, after_matrix_from_graph));
    printf("Edmonds: %lf\n", getRuntime(after_matrix_from_graph, finish));
    printf("Total runtime: %lf\n", getRuntime(start, finish));

    return 0;
}
