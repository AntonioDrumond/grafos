#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <map>
#include <fstream>
#include <sstream>
#include <cstdlib>

#ifndef EDGE_H
#define EDGE_H
#include "edge.h"
#endif


struct Component {
    int root;  // Raiz do componente
    int size;  // Tamanho do componente
    float max_weight; // Peso máximo do componente
    int color;  // Cor ou identificador único do componente
};

// Função para encontrar a raiz de um componente
int findRoot(std::vector<Component>& components, int u) {
    if (components[u].root != u) {
        components[u].root = findRoot(components, components[u].root);
    }
    return components[u].root;
}

// Função para mesclar dois componentes
void mergeComponents(std::vector<Component>& components, int u_root, int v_root, float weight, int new_color) {
    // Mesclar o componente menor no maior
    if (components[u_root].size < components[v_root].size) {
        std::swap(u_root, v_root);
    }

    // Atualizar a raiz e o tamanho do componente
    components[v_root].root = u_root;
    components[u_root].size += components[v_root].size;
    components[u_root].max_weight = std::max(components[u_root].max_weight, weight);

    // Atribuir uma cor única ao componente resultante
    components[u_root].color = new_color;  // Novo identificador de cor
}

// Função de comparação para ordenar as arestas
bool compareEdges(const Edge& e1, const Edge& e2) {
    return e1.w < e2.w;
}

// Função para segmentação de imagem
std::vector<int> segmentImage(const std::vector<std::vector<std::vector<int>>>& image, int width, int height, float k) {

    int num_pixels = width * height;
    std::vector<Edge> edges;//Grafo representado por uma lista de arestas

    // Construir as arestas com base na diferença de cor entre pixels adjacentes
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int u = y * width + x;// constroi o id do pixel 

            // Conectar à direita
            if (x < width - 1) {
                int v = u + 1;
                float dr = image[y][x][0] - image[y][x + 1][0]; // Diferença no canal R
                float dg = image[y][x][1] - image[y][x + 1][1]; // Diferença no canal G
                float db = image[y][x][2] - image[y][x + 1][2]; // Diferença no canal B
                float weight = std::sqrt(dr * dr + dg * dg + db * db);
                if (weight < 3.0999f) {
                    weight = 0.0f; // Marcar como iguais
                }
                edges.push_back({u, v, weight});
            }

            // Conectar abaixo
            if (y < height - 1) {
                int v = u + width;
                float dr = image[y][x][0] - image[y + 1][x][0]; 
                float dg = image[y][x][1] - image[y + 1][x][1];
                float db = image[y][x][2] - image[y + 1][x][2];
                float weight = std::sqrt(dr * dr + dg * dg + db * db);
                if (weight < 3.0999f) {
                    weight = 0.0f;
                }
                edges.push_back({u, v, weight});
            }

            // Conectar na diagonal inferior direita
            if (x < width - 1 && y < height - 1) {
                int v = u + width + 1;
                float dr = image[y][x][0] - image[y + 1][x + 1][0]; 
                float dg = image[y][x][1] - image[y + 1][x + 1][1];
                float db = image[y][x][2] - image[y + 1][x + 1][2];
                float weight = std::sqrt(dr * dr + dg * dg + db * db);
                if (weight < 3.0999f) {
                    weight = 0.0f;
                }
                edges.push_back({u, v, weight});
            }

            // Conectar na diagonal inferior esquerda
            if (x > 0 && y < height - 1) {
                int v = u + width - 1;
                float dr = image[y][x][0] - image[y + 1][x - 1][0]; 
                float dg = image[y][x][1] - image[y + 1][x - 1][1];
                float db = image[y][x][2] - image[y + 1][x - 1][2];
                float weight = std::sqrt(dr * dr + dg * dg + db * db);
                if (weight < 3.0999f) {
                    weight = 0.0f;
                }
                edges.push_back({u, v, weight});
            }
        }
    }

    // Ordenar as arestas por peso
    std::sort(edges.begin(), edges.end(), compareEdges);

    // Inicializar os componentes com cores únicas
    std::vector<Component> components(num_pixels);
    for (int i = 0; i < num_pixels; ++i) {
        components[i] = {i, 1, 0.0f, i}; // Cada pixel (verticie) é um componente inicialmente
    }

    // Algoritmo de Kruskal para segmentação com critérios de fusão

    int color_counter = num_pixels; //incialmente todo pixel e um componente

    for (const auto& edge : edges) {
        int u_root = findRoot(components, edge.u);
        int v_root = findRoot(components, edge.v);

        if (u_root != v_root) {//verifica se estao em componentes disjuntos

            // Calcular a diferença interna dos componentes
            float internal_diff_u = components[u_root].max_weight + k / components[u_root].size;
            float internal_diff_v = components[v_root].max_weight + k / components[v_root].size;

        
            // Se o peso da aresta for pequeno em comparação com a diferença interna, realizar a fusão
            if (edge.w <= std::min(internal_diff_u, internal_diff_v)) {
                // Fusão dos componentes u e v
                mergeComponents(components, u_root, v_root, edge.w, color_counter++);
            }
        }
    }


    // Atribuir o rótulo final com base na cor dos componentes
    std::vector<int> labels(num_pixels);
    for (int i = 0; i < num_pixels; ++i) {
        labels[i] = components[findRoot(components, i)].color;
    }

    return labels;
}

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
            int nrx = 0,
                ngx = 0,
                nbx = 0,
                nry = 0,
                ngy = 0,
                nby = 0;
            if (!top) {
                // nrx += 0;
                // ngx += 0;
                // nbx += 0;
                nry += -2 * img[y+1][x][0];
                ngy += -2 * img[y+1][x][1];
                nby += -2 * img[y+1][x][2];
            } else {
                nry += -2 * img[y][x][0];
                ngy += -2 * img[y][x][1];
                nby += -2 * img[y][x][2];
            }
            if (!bot) {
                // nrx += 0;
                // ngx += 0;
                // nbx += 0;
                nry += 2 * img[y-1][x][0];
                ngy += 2 * img[y-1][x][1]; 
                nby += 2 * img[y-1][x][2];
            } else {
                nry += 2 * img[y][x][0];
                ngy += 2 * img[y][x][1];
                nby += 2 * img[y][x][2];
            }
            if (!right) {
                nrx += 2 * img[y][x+1][0];
                ngx += 2 * img[y][x+1][1];
                nbx += 2 * img[y][x+1][2];
                // nry += 0;
                // ngy += 0;
                // nby += 0;
            } else {
                nrx += 2 * img[y][x][0];
                ngx += 2 * img[y][x][1];
                nbx += 2 * img[y][x][2];
            }
            if (!left) {
                nrx += -2 * img[y][x-1][0];
                ngx += -2 * img[y][x-1][1];
                nbx += -2 * img[y][x-1][2];
                // nry += 0;
                // ngy += 0;
                // nby += 0;
            } else {
                nrx += -2 * img[y][x][0];
                ngx += -2 * img[y][x][1];
                nbx += -2 * img[y][x][2];
            }
            if (!left && !bot){
                nrx += -1 * img[y-1][x-1][0];
                ngx += -1 * img[y-1][x-1][1];
                nbx += -1 * img[y-1][x-1][2];
                nry += img[y-1][x-1][0];
                ngy += img[y-1][x-1][1];
                nby += img[y-1][x-1][2];
            } else {
                nrx += -1 * img[y][x][0];
                ngx += -1 * img[y][x][1];
                nbx += -1 * img[y][x][2];
                nry += img[y][x][0];
                ngy += img[y][x][1];
                nby += img[y][x][2];
            }
            if (!left && !top){
                nrx += -1 * img[y+1][x-1][0];
                ngx += -1 * img[y+1][x-1][1];
                nbx += -1 * img[y+1][x-1][2];
                nry += -1 * img[y+1][x-1][0];
                ngy += -1 * img[y+1][x-1][1];
                nby += -1 * img[y+1][x-1][2];
            } else {
                nrx += -1 * img[y][x][0];
                ngx += -1 * img[y][x][1];
                nbx += -1 * img[y][x][2];
                nry += -1 * img[y][x][0];
                ngy += -1 * img[y][x][1];
                nby += -1 * img[y][x][2];
            }
            if (!right&& !bot){
                nrx += img[y-1][x+1][0];
                ngx += img[y-1][x+1][1];
                nbx += img[y-1][x+1][2];
                nry += img[y-1][x+1][0];
                ngy += img[y-1][x+1][1];
                nby += img[y-1][x+1][2];
            } else {
                nrx += img[y][x][0];
                ngx += img[y][x][1];
                nbx += img[y][x][2];
                nry += img[y][x][0];
                ngy += img[y][x][1];
                nby += img[y][x][2];
            }
            if (!right && !top){
                nrx += img[y+1][x+1][0];
                ngx += img[y+1][x+1][1];
                nbx += img[y+1][x+1][2];
                nry += -1 * img[y+1][x+1][0];
                ngy += -1 * img[y+1][x+1][1];
                nby += -1 * img[y+1][x+1][2];
            } else {
                nrx += img[y][x][0];
                ngx += img[y][x][1];
                nbx += img[y][x][2];
                nry += -1 * img[y][x][0];
                ngy += -1 * img[y][x][1];
                nby += -1 * img[y][x][2];
            }
            int nr = std::sqrt(nrx * nrx + nry * nry);
            int ng = std::sqrt(ngx * ngx + ngy * ngy);
            int nb = std::sqrt(nbx * nbx + nby * nby);
            res[y][x] = {nr, ng, nb};
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

std::vector<std::vector<std::vector<int>>> blurImg (std::vector<std::vector<std::vector<int>>> &img, int width, int height, int passes){
    auto blurred = gaussianBlur(img, width, height);
    for(int i=1; i<passes; i++){
        blurred = gaussianBlur(blurred, width, height);
    }
    return blurred;
}

// Função para salvar imagem segmentada em PPM
void savePPM(
    const std::string &filename, 
    const std::vector<std::vector<std::vector<int>>> &image,
    const std::vector<int> &labels, 
    int width, int height
) {
    std::ofstream file(filename);
    
    file << "P6\n";
    file << "# Segmented Image\n";
    file << width << " " << height << "\n";
    file << "255\n";
    
    std::map<int, std::vector<int>> color_map;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int label = labels[y * width + x];

            if (color_map.find(label) == color_map.end()) {
                int r = rand() % 256, g = rand() % 256, b = rand() % 256;
                color_map[label] = {r, g, b};
            }
            unsigned char pixel[3] = {static_cast<unsigned char>(color_map[label][0]),
                                    static_cast<unsigned char>(color_map[label][1]),
                                    static_cast<unsigned char>(color_map[label][2])};
            file.write(reinterpret_cast<const char*>(pixel), 3);
        }
    }
}
