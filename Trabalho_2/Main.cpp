#include "Graph.h"
#include "Ppm.h"

int main() {

    int width, height;

    std::vector<std::vector<std::vector<int>>> image; // estrutura para armazenar uma imagem representada por uma matriz tridimensional dinâmica
    if (!loadPPM("./input.ppm", image, width, height)) {
        std::cout<< "nao foi possivel abrir a imagem"<<std::endl;

    }
    std::cout<< "valor da largura: "<<width<<std::endl;
    std::cout<< "valor da altura: "<<height<<std::endl;

    // Parametros para a segmentação da imagem

    float k = 300.0f;//parâmetro que influencia a formação das regiões.quanto mais baixo mais "fina"

    // Segmentação da imagem

    std::vector<int> labels = segmentImage(image, width, height, k);

    // Salvar a imagem segmentada
    savePPM("./segment_image.ppm", image, labels, width, height);
    
    std::cout<< "concluido "<<height<<std::endl;
    return 0;
}
