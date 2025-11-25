#include <cstdint>
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <stdio.h>
#include <stdlib.h>
#include "Util.h"

class Graph{

private:

    int n; // maximum capacity
    int last_vert; //current size
    bool directed;
    std::vector<std::unordered_set<int>> arr; //adjacency list
    std::vector<std::string> label;

public:

    //Constructor
    Graph(int n, bool directed = false)
    : n(n), last_vert(0), directed(directed), arr(n), label(n) {}

    //Destructor
    ~Graph() = default;

    bool add_vert(){
        if(last_vert < n){
            last_vert++;
            return true;
        }
        else return false;
    }

    bool add_vert(std::string label){
        if(last_vert < n){
            this->label[last_vert] = label;
            last_vert++;
            return true;
        }
        else return false;
    }

    void setLabel(int vert, std::string label)
    {
        if (vert <= last_vert) {
            this->label[vert] = label;
        } 
        else {
            throw std::invalid_argument("Vertex does not exist");
        }
    }

    std::string getLabel(int vert)
    {
        if (vert <= last_vert) {
            return (this->label[vert]);
        }
        else {
            throw std::invalid_argument("Vertex does not exist");
        }
    }

    void all_verts(){
        while(add_vert()); // Adicionar todos os vertices possiveis
    }

    std::unordered_set<int> vert_neighbors(int vert) {
        if(vert <= last_vert){
            return arr[vert];
        }
        else {
            throw std::invalid_argument("Vertex does not exist");
        }
    }

    bool add_edge(int vert1, int vert2){
        if(vert1 <= last_vert && vert2 <= last_vert){
            if(arr[vert1].count(vert2) == 0){ // Verifica se a aresta ja existe
                arr[vert1].insert(vert2);
                if (!directed) {
                    arr[vert2].insert(vert1);
                }
                return true; // Aresta adicionada
            }
            else return false; // Vertices validos, mas ja ha aresta
        }
        else return false; // Vertices invalidos
    }

    bool check_edge(int vert1, int vert2){
        if(vert1 <= last_vert && vert2 <= last_vert){
            if (!directed) {
                return (arr[vert1].count(vert2) > 0) && (arr[vert2].count(vert1) > 0);
            }
            else {
                return (arr[vert1].count(vert2) > 0);
            }
        }
        else return false; // Vertices invalidos
    }

    bool remove_edge(int vert1, int vert2){
        if(check_edge(vert1, vert2)){
            arr[vert1].erase(vert2);
            if (!directed) {
                arr[vert2].erase(vert1);
            }
            return true;
        }
        else return false;
    }

    void test(){
        std::cout << "n = " << this->n << "\n";
        std::cout << "last_vert = " << this->last_vert << "\n";
    }

    // Prints the graph in a format recognized by https://csacademy.com/app/graph_editor/
    void print_csacademy(){
        for(int i=0; i<last_vert; i++){
            std::cout << i << "\n";
        }
        for(int i=0; i<last_vert; i++){
            for(int neighbor : arr[i]){
                std::cout << i << " " << neighbor << "\n";
            }
        }
    }

    void print_raw(){
        for(int i=0; i<last_vert; i++) {
            if (label[last_vert] != "") {
                std::cout << i << " ) ";
            }
            else {
                std::cout << i << " : \"" << label[i] << "\" ) ";
            }
            printVec(arr[i]);
        }
    }

private:

    void printVec(std::unordered_set<int> v){
        std::cout << "| ";
        for(int neighbor : v){
            std::cout << neighbor << " | ";
        }
        std::cout << "\n";
    }
};

class RGB {
public:
    int r;
    int g;
    int b;

    RGB()
    : r(0), g(0), b(0) {}

    RGB(int r, int g, int b)
    : r(r), g(g), b(b) {}

    ~RGB() = default;
};


class WeightedGraph{

private:

    int n; // maximum capacity
    int last_vert; //current size
    bool directed; 
    std::vector<std::unordered_map<int, std::vector<double>>> arr; //adjacency list
    std::vector<std::string> label;
    std::vector<RGB> pix_color;

public:

    //Constructor
    WeightedGraph(int n, bool directed = false)
    : n(n), last_vert(0), directed(directed), arr(n), label(n), pix_color(n) {}

    //Destructor
    ~WeightedGraph() = default;

    static WeightedGraph from_ppm_matrix(
        std::vector<std::vector<std::vector<int>>> &img,
        int width, int height
    ) {
        int nVerts = width * height;
        WeightedGraph res (nVerts);

        // printf("width = %d\nheight = %d\nnVerts = %d\nres size = %d\n", width, height, nVerts, res.n);

        res.all_verts();

        for(int i=0; i<nVerts; i++){
            int x = i % width,
                y = i / width;

            bool leftEdge = x < 1,
                rightEdge = x == width-1, 
                underEdge = y == height-1;

            res.pix_color[i].r = img[y][x][0];
            res.pix_color[i].g = img[y][x][1];
            res.pix_color[i].b = img[y][x][2];

            // printf("i = %d\nx = %d\ny = %d\n", i, x, y);
            // printf("leftEdge = %d, rightEdge = %d, underEdge = %d\n", leftEdge, rightEdge, underEdge);

            if (!leftEdge && !underEdge) { // Diagonal esq-baixo
                int other = (x-1) + ( (y+1) * width );
                // std::cout << "(diag esq baixo) i = " << i << " | other = " << other << std::endl;
                double diff = rgb_diff(img[y][x], img[y+1][x-1]);
                res.add_edge(i, other, diff);
            }

            if (!underEdge) { // Para baixo
                int other = (x) + ( (y+1) * width );
                // std::cout << "(baixo) i = " << i << " | other = " << other << std::endl;
                double diff = rgb_diff(img[y][x], img[y+1][x]);
                res.add_edge(i, other, diff);
            }

            if (!underEdge && !rightEdge) { // Diagonal baixo-direita
                int other = (x+1) + ( (y+1) * width );
                // std::cout << "(diag baix esq) i = " << i << " | other = " << other << std::endl;
                double diff = rgb_diff(img[y][x], img[y+1][x+1]);
                res.add_edge(i, other, diff);
            }

            if (!rightEdge) { // Para direita
                int other = (x+1) + ( (y) * width );
                // std::cout << "(direita) i = " << i << " | other = " << other << std::endl;
                double diff = rgb_diff(img[y][x], img[y][x+1]);
                res.add_edge(i, other, diff);
            }

            // printf("\n\n");

        }

        return res;
    }

    std::vector<std::vector<std::vector<int>>> to_ppm_matrix(int width, int height) {
        std::vector<std::vector<std::vector<int>>> res;
        res.resize(height, std::vector<std::vector<int>>(width, std::vector<int>(3)));

        int nVerts = this->last_vert <= (width*height) ? this->last_vert : (width*height);
        for(int i=0; i<nVerts; i++){
            int x = i % width,
                y = i / width;

            bool leftEdge = x < 1,
                rightEdge = x == width-1, 
                underEdge = y == height-1;

            res[y][x][0] = this->pix_color[i].r;
            res[y][x][1] = this->pix_color[i].g;
            res[y][x][2] = this->pix_color[i].b;
        }

        return res;
    }

    bool add_vert() {
        if (last_vert < n) {
            last_vert++;
            return true;
        }
        return false;
    }

    bool add_vert(std::string label) {
        if (last_vert < n) {
            this->label[last_vert++] = label;
            return true;
        }
        return false;
    }

    void setLabel(int vert, std::string label)
    {
        if (vert <= last_vert) {
            this->label[vert] = label;
        }
        else {
            throw std::invalid_argument("Vertex does not exist");
        }
    }

    std::string getLabel(int vert)
    {
        if (vert <= last_vert) {
            return (this->label[vert]);
        }
        else {
            throw std::invalid_argument("Vertex does not exist");
        }
    }

    void all_verts(){
        while(add_vert()); // Adicionar todos os vertices possiveis
    }

    bool add_edge(int vert1, int vert2, double weight){
        if(vert1 <= last_vert && vert2 <= last_vert){

            // Verifica se a aresta ja existe ou se esse peso ja existe
            if ( (arr[vert1].count(vert2) == 0) || (std::count(arr[vert1][vert2].begin(), arr[vert1][vert2].end (), weight) == 0) )
            { 
                arr[vert1][vert2].push_back(weight);
                if (!directed) {
                    arr[vert2][vert1].push_back(weight);
                }
                return true; // Aresta adicionada
            }
            else return false; // Vertices validos, mas ja ha aresta ou peso
        }
        else return false; // Vertices invalidos
    }

    bool check_edge(int vert1, int vert2){
        if(vert1 <= last_vert && vert2 <= last_vert){
            if (!directed) {
                return (arr[vert1].count(vert2) > 0) && (arr[vert2].count(vert1) > 0);
            }
            else {
                return (arr[vert1].count(vert2) > 0);
            }
        }
        else return false; // Vertices invalidos
    }

    bool remove_edge(int vert1, int vert2)
    {
        if (check_edge(vert1, vert2)) {
            arr[vert1].erase(vert2);
            if (!directed) {
                arr[vert2].erase(vert1);
            }
            return true;
        }
        else return false;
    }

    bool remove_edge(int vert1, int vert2, double weight) 
    {
        if(check_edge(vert1, vert2)) 
        {
            if (arr[vert1][vert2].size() > 1) {
                std::vector<double>* weight_list = &arr[vert1][vert2];
                auto it = std::find(weight_list->begin(), weight_list->end(), weight);

                if (it != weight_list->end()) {
                    *it = weight_list->back();
                    weight_list->pop_back();
                }
                else return false; // There is no edge with the given weight

                if (!directed) {
                    std::vector<double>* weight_list = &arr[vert2][vert1];
                    auto it = std::find(weight_list->begin(), weight_list->end(), weight);

                    *it = weight_list->back();	 	// Assuming that the weight was found based on the previous check
                    weight_list->pop_back();
                }
                return true;
            }
            else if (arr[vert1][vert2][0] == weight)
            {
                arr[vert1].erase(vert2);
                if (!directed) {
                    arr[vert2].erase(vert1);
                }
                return true;
            }
        }
        return false;
    }

    std::unordered_map<int, std::vector<double>> vert_neighbors(int vert) {
        if(vert <= last_vert){
            return arr[vert];
        }
        else {
            throw std::invalid_argument("Vertex does not exist");
        }
    }

    void print_csacademy() const {
        // char *color = new char[10];
        for (int i = 0; i < last_vert; i++) {
            // snprintf(color, 9, "0x%02x%02x%02x", this->pix_color[i].r, this->pix_color[i].g, this->pix_color[i].b);
            // std::cout << i << " " << color << "\n";
            std::cout << i << "\n";
        }
        // delete[] color;
        for (int i = 0; i < last_vert; i++) {
            for (auto [neighbor, weight] : arr[i]) {
                std::cout << i << " " << neighbor << " " << weight[0] << "\n";
            }
        }
    }

    void print_raw() const {
        for (int i = 0; i < last_vert; i++) {
            if (label[i] != "") {
                std::cout << i << " : \"" << label[i] << "\" ) | ";
            }
            else {
                std::cout << i << " ) | ";
            }
            for (auto [neighbor, weight_list] : arr[i]) {
                std::cout << neighbor << "{";
                int n = weight_list.size();
                std::cout << weight_list[0];
                for (int i = 1; i < n; i++) {
                    std::cout << ", " << weight_list[i];
                }
                std::cout << "} | ";
            }
            std::cout << "\n";
        }
    }

};
