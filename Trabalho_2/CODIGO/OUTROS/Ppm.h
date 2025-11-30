#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdint.h>
#include "../GRAFO/edge.h"

bool loadPPM(
    const std::string &filename, 
    std::vector<std::vector<std::vector<int>>> &image, 
    int &width, int &height
) {
    
    std::ifstream file(filename, std::ios::binary); 
    if (!file.is_open()) {
        std::cerr << "ERRO ao abrir o arquivo PPM." << std::endl;
        return false;
    }

    std::string header;
    file >> header;
    if (header != "P6") {
        std::cerr << "ERRO: Formato PPM esperado: P6." << std::endl;
        return false;
    }

    char nextChar;
    file.get(nextChar);
    while (nextChar == '#') {
        std::string comment;
        std::getline(file, comment);
        file.get(nextChar);
    }
    file.unget(); 

    int max_value;
    file >> width >> height >> max_value;
    file.get(); 

    if (width <= 0 || height <= 0 || max_value != 255) {
        std::cerr << "Dimensões ou valor máximo inválido no arquivo PPM." << std::endl;
        return false;
    }

    // [height][widht][rgb[3]]
    image.resize(height, std::vector<std::vector<int>>(width, std::vector<int>(3)));

    // Para cada pixel armazena os dados sobre o rgb dele
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char rgb[3];
            if (!file.read(reinterpret_cast<char *>(rgb), 3)) {
                std::cerr << "Erro ao ler dados RGB do arquivo PPM." << std::endl;
                return false;
            }
            image[y][x][0] = rgb[0];
            image[y][x][1] = rgb[1];
            image[y][x][2] = rgb[2];
        }
    }

    return true;
}

void savePPM_matrix(
    const std::string &filename, 
    const std::vector<std::vector<std::vector<int>>> &image,
    int width, int height
) {
    std::ofstream file(filename);
    
    file << "P6\n";
    // file << "# Segmented Image\n";
    file << width << " " << height << "\n";
    file << "255\n";
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            unsigned char pixel[3] = {static_cast<unsigned char>(image[y][x][0]),
                                    static_cast<unsigned char>(image[y][x][1]),
                                    static_cast<unsigned char>(image[y][x][2])};
            file.write(reinterpret_cast<const char*>(pixel), 3);
        }
    }
}

std::vector<std::vector<std::vector<int>>> sobelOperator(std::vector<std::vector<std::vector<int>>> &img, int width, int height){
    std::vector<std::vector<std::vector<int>>> res;
    res.resize(height, std::vector<std::vector<int>>(width, std::vector<int>(3)));
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            bool left = x < 1,
                 right = x > width-2,
                 bot = y < 1,
                 top = y > height-2;

            int nrx = 0, nry = 0;

            if (!top) {
                // nrx += 0;
                // ngx += 0;
                // nbx += 0;
                nry += -2 * img[y+1][x][0];
            }
            if (!bot) {
                // nrx += 0;
                // ngx += 0;
                // nbx += 0;
                nry += 2 * img[y-1][x][0];
            }
            if (!right) {
                nrx += 2 * img[y][x+1][0];
                // nry += 0;
                // ngy += 0;
                // nby += 0;
            } else {
                nrx += 2 * img[y][x][0];
            }
            if (!left) {
                nrx += -2 * img[y][x-1][0];
                // nry += 0;
                // ngy += 0;
                // nby += 0;
            } else {
                nrx += -2 * img[y][x][0];
            }
            if (!left && !bot){
                nrx += -1 * img[y-1][x-1][0];
                nry += img[y-1][x-1][0];
            }
            if (!left && !top){
                nrx += -1 * img[y+1][x-1][0];
                nry += -1 * img[y+1][x-1][0];
            }
            if (!right&& !bot){
                nrx += img[y-1][x+1][0];
                nry += img[y-1][x+1][0];
            }
            if (!right && !top){
                nrx += img[y+1][x+1][0];
                nry += -1 * img[y+1][x+1][0];
            }
			// Use a single color since all colors should have the same value
			// because of the grayscaling
			int G = std::sqrt(nrx * nrx + nry * nry);
            res[y][x] = {G, G, G};
        }
    }
    return res;
}

std::vector<std::vector<std::vector<int>>> gaussianBlur (std::vector<std::vector<std::vector<int>>> &img, int width, int height){
    std::vector<std::vector<std::vector<int>>> res;
    res.resize(height, std::vector<std::vector<int>>(width, std::vector<int>(3)));
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            bool left = x < 1,
                 right = x > width-2,
                 bot = y < 1,
                 top = y > height-2;
            int nr = img[y][x][0] * 4,
                ng = img[y][x][1] * 4,
                nb = img[y][x][2] * 4;
            if (!top) {
                nr += 2 * img[y+1][x][0];
                ng += 2 * img[y+1][x][1];
                nb += 2 * img[y+1][x][2];
            } else {
                nr += 2 * img[y][x][0];
                ng += 2 * img[y][x][1];
                nb += 2 * img[y][x][2];
            }
            if (!bot) {
                nr += 2 * img[y-1][x][0];
                ng += 2 * img[y-1][x][1]; 
                nb += 2 * img[y-1][x][2];
            } else {
                nr += 2 * img[y][x][0];
                ng += 2 * img[y][x][1];
                nb += 2 * img[y][x][2];
            }
            if (!right) {
                nr += 2 * img[y][x+1][0];
                ng += 2 * img[y][x+1][1];
                nb += 2 * img[y][x+1][2];
            } else {
                nr += 2 * img[y][x][0];
                ng += 2 * img[y][x][1];
                nb += 2 * img[y][x][2];
            }
            if (!left) {
                nr += 2 * img[y][x-1][0];
                ng += 2 * img[y][x-1][1];
                nb += 2 * img[y][x-1][2];
            } else {
                nr += 2 * img[y][x][0];
                ng += 2 * img[y][x][1];
                nb += 2 * img[y][x][2];
            }
            if (!left && !bot){
                nr += img[y-1][x-1][0];
                ng += img[y-1][x-1][1];
                nb += img[y-1][x-1][2];
            } else {
                nr += img[y][x][0];
                ng += img[y][x][1];
                nb += img[y][x][2];
            }
            if (!left && !top){
                nr += img[y+1][x-1][0];
                ng += img[y+1][x-1][1];
                nb += img[y+1][x-1][2];
            } else {
                nr += img[y][x][0];
                ng += img[y][x][1];
                nb += img[y][x][2];
            }
            if (!right&& !bot){
                nr += img[y-1][x+1][0];
                ng += img[y-1][x+1][1];
                nb += img[y-1][x+1][2];
            } else {
                nr += img[y][x][0];
                ng += img[y][x][1];
                nb += img[y][x][2];
            }
            if (!right && !top){
                nr += img[y+1][x+1][0];
                ng += img[y+1][x+1][1];
                nb += img[y+1][x+1][2];
            } else {
                nr += img[y][x][0];
                ng += img[y][x][1];
                nb += img[y][x][2];
            }
            nr /= 16;
            ng /= 16;
            nb /= 16;
            res[y][x] = {nr, ng, nb};
        }
    }
    return res;
}

void lightenImg(std::vector<std::vector<std::vector<int>>> &img, int width, int height, double factor) {
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            img[y][x][0] *= factor;
            img[y][x][1] *= factor;
            img[y][x][2] *= factor;
            if(img[y][x][0] > 255) img[y][x][0] = 255;
            if(img[y][x][1] > 255) img[y][x][1] = 255;
            if(img[y][x][2] > 255) img[y][x][2] = 255;
        }
    }
}

void grayscaleImg (std::vector<std::vector<std::vector<int>>> &img, int width, int height) {
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
			uint8_t r = img[y][x][0];
			uint8_t g = img[y][x][1];
			uint8_t b = img[y][x][2];
			uint8_t color = ((double)(r+g+b)/3.0);
			img[y][x][0] = color;
			img[y][x][1] = color;
			img[y][x][2] = color;
        }
    }
}

std::vector<std::vector<std::vector<int>>> blurImg (std::vector<std::vector<std::vector<int>>> &img, int width, int height, int passes){
    auto blurred = gaussianBlur(img, width, height);
    for(int i=1; i<passes; i++){
        blurred = gaussianBlur(blurred, width, height);
    }
    return blurred;
}
