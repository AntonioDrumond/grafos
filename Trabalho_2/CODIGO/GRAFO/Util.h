#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
#include <stdio.h>
#include <iostream>

inline double rgb_diff(std::vector<int> v1, std::vector<int> v2) {
    int rdiff = std::abs(v1[0] - v2[0]),
        gdiff = std::abs(v1[1] - v2[1]),
        bdiff = std::abs(v1[2] - v2[2]);
    return std::sqrt( rdiff * rdiff + gdiff * gdiff + bdiff * bdiff );
}

inline double srgb_channel_to_linear(double value) {
    double normalized = value / 255.0;
    if (normalized <= 0.04045) {
        return normalized / 12.92;
    }
    return std::pow((normalized + 0.055) / 1.055, 2.4);
}

inline std::array<double, 3> rgb_to_xyz(const std::vector<int>& rgb) {
    double r = srgb_channel_to_linear(static_cast<double>(rgb[0]));
    double g = srgb_channel_to_linear(static_cast<double>(rgb[1]));
    double b = srgb_channel_to_linear(static_cast<double>(rgb[2]));

    double x = r * 0.4124564 + g * 0.3575761 + b * 0.1804375;
    double y = r * 0.2126729 + g * 0.7151522 + b * 0.0721750;
    double z = r * 0.0193339 + g * 0.1191920 + b * 0.9503041;

    return {x * 100.0, y * 100.0, z * 100.0};
}

inline double pivot_xyz(double value) {
    constexpr double epsilon = 0.008856;
    constexpr double kappa = 903.3;
    if (value > epsilon) {
        return std::cbrt(value);
    }
    return (kappa * value + 16.0) / 116.0;
}

inline std::array<double, 3> rgb_to_lab_color(const std::vector<int>& rgb) {
    const std::array<double, 3> xyz = rgb_to_xyz(rgb);
    constexpr double refX = 95.047;
    constexpr double refY = 100.000;
    constexpr double refZ = 108.883;

    double x = pivot_xyz(xyz[0] / refX);
    double y = pivot_xyz(xyz[1] / refY);
    double z = pivot_xyz(xyz[2] / refZ);

    double L = std::max(0.0, 116.0 * y - 16.0);
    double a = 500.0 * (x - y);
    double b = 200.0 * (y - z);

    return {L, a, b};
}

inline double lab_distance(const std::array<double, 3>& lab1, const std::array<double, 3>& lab2) {
    double dL = lab1[0] - lab2[0];
    double da = lab1[1] - lab2[1];
    double db = lab1[2] - lab2[2];
    return std::sqrt(dL * dL + da * da + db * db);
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

#endif
