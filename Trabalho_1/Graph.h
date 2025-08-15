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

    bool add_vert(){
        if(last_vert < n){
            last_vert++;
            return true;
        }
        else return false;
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

    void test(){
        std::cout << "renato\n"; 
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
