#include <iostream>
#include <vector>

struct Graph{
    int n;
    int last_vert;
    std::vector<int>* arr;

    Graph(int n){
        this->arr = new std::vector<int>[n];
        this->n = n;
        this->last_vert = 0;
    }

    ~Graph(){
        if(this->arr) delete[] this->arr;
    }

    void all_verts(){
        while(add_vert()); // Adicionar todos os vertices possiveis
    }

    bool add_vert(){
        if(last_vert < n){
            last_vert++;
            return true;
        }
        else return false;
    }

    std::vector<int> vert_neighbors(int vert){
        if(vert <= last_vert){
            return arr[vert];
        }
        else return {};
    }

    bool add_edge(int vert1, int vert2){
        if(vert1 <= last_vert && vert2 <= last_vert){
            if(-1 != searchVec(arr[vert1], vert2) && -1 != searchVec(arr[vert2], vert1)){ // Verifica se a aresta ja existe
                arr[vert1].push_back(vert2);
                arr[vert2].push_back(vert1);
                return true; // Aresta adicionada
            }
            else return false; // Vertices validos, mas ja ha aresta
        }
        else return false; // Vertices invalidos
    }

    bool check_edge(int vert1, int vert2){
        if(vert1 <= last_vert && vert2 <= last_vert){
            if(-1 != searchVec(arr[vert1], vert2) && -1 != searchVec(arr[vert2], vert1)){ // Verifica se a aresta existe
                return true;
            }
            else return false; // Vertices validos, aresta inexistente
        }
        else return false; // Vertices invalidos
    }

    bool remove_edge(int vert1, int vert2){
        if(check_edge(vert1, vert2)){
            int reps = arr[vert1].size();
            for(int i=0; i<reps; i++){
                if(arr[vert1].at(i) == vert2){
                    arr[vert1].erase(arr[vert1].begin() + i);
                    i = reps;
                }
            }
            reps = arr[vert2].size();
            for(int i=0; i<reps; i++){
                if(arr[vert2].at(i) == vert1){
                    arr[vert2].erase(arr[vert2].begin() + i);
                    i = reps;
                }
            }
            return true;
        }
        else return false;
    }

    void test(){
        std::cout << "n = " << this->n << "\n";
        std::cout << "last_vert = " << this->last_vert << "\n";
    }

    private:

    int searchVec(std::vector<int> v, int e){
        int pos = -1,
            reps = v.size();
        for(int i=0; i<reps; i++){
            if(v[i] == e){
                pos = i;
                i = reps;
            }
        }
        return pos;
    }

};
