#include <cmath>
#include <vector>
#include <stdio.h>
#include <iostream>

inline double rgb_diff(std::vector<int> v1, std::vector<int> v2) {
    int rdiff = std::abs(v1[0] - v2[0]),
        gdiff = std::abs(v2[1] - v2[1]),
        bdiff = std::abs(v1[2] - v2[2]);
    return std::sqrt( rdiff * rdiff + gdiff * gdiff + bdiff * bdiff );
}

inline int rgb_max(std::vector<int> v1, std::vector<int> v2) {
	int rmax = std::max(v1[0], v2[0]);
	int gmax = std::max(v1[1], v2[1]);
	int bmax = std::max(v1[2], v2[2]);
	return std::max(rmax, std::max(gmax, bmax));
}

inline void print_ppm(std::vector<std::vector<std::vector<int>>> &image, int width, int height) {
    int nVerts = width * height;
    std::cout << "vec1 len = " << image.size() << "\nvec2 len = " << image[0].size() << std::endl;
    for(int i=0; i<nVerts; i++){
        int x = i % width,
            y = i / width;
        printf("[0x%02x%02x%02x]", image[y][x][0], image[y][x][1], image[y][x][2]);
        // std::cout << "[0x0000" << image[x][y][2] << "]" << std::endl;
        bool rightEdge = x == width-1;
        if (rightEdge) printf("\n");
    }
}

