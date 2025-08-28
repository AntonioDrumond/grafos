#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

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
		if (vert <= last_vert)
		{
			this->label[vert] = label;
		}
		else
		{
			throw std::invalid_argument("Vertex does not exist");
		}
	}

	std::string getLabel(int vert)
	{
		if (vert <= last_vert)
		{
			return (this->label[vert]);
		}
		else
		{
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
        for(int i=0; i<last_vert; i++) 
		{
			if (label[last_vert] != "")
			{
				std::cout << i << " ) ";
			}
			else
			{
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


class WeightedGraph{

    private:

	int n; // maximum capacity
	int last_vert; //current size
	bool directed; 
	std::vector<std::unordered_map<int, std::vector<double>>> arr; //adjacency list
	std::vector<std::string> label;

	public:

	//Constructor
	WeightedGraph(int n, bool directed = false)
		: n(n), last_vert(0), directed(directed), arr(n), label(n) {}

	//Destructor
	~WeightedGraph() = default;

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
		if (vert <= last_vert)
		{
			this->label[vert] = label;
		}
		else
		{
			throw std::invalid_argument("Vertex does not exist");
		}
	}

	std::string getLabel(int vert)
	{
		if (vert <= last_vert)
		{
			return (this->label[vert]);
		}
		else
		{
			throw std::invalid_argument("Vertex does not exist");
		}
	}

	void all_verts(){
		while(add_vert()); // Adicionar todos os vertices possiveis
	}

	bool add_edge(int vert1, int vert2, double weight){
		if(vert1 <= last_vert && vert2 <= last_vert){

			// Verifica se a aresta ja existe
			if ( (arr[vert1].count(vert2) == 0) || (std::count(arr[vert1][vert2].begin(), arr[vert1][vert2].end (), weight) == 0 ))
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

	int count_weight(int vert1, int vert2, double weight)
	{
		int count = 0;
		if (check_edge(vert1, vert2))
		{
			std::vector<double> weight_list = arr[vert1][vert2];  
			count = std::count(weight_list.begin(), weight_list.end(), weight);
		}
		return (count);
	}

    bool remove_edge(int vert1, int vert2)
	{
		if (check_edge(vert1, vert2))
		{
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
			if (arr[vert1][vert2].size() > 1)
			{
				std::vector<double>* weight_list = &arr[vert1][vert2];
				auto it = std::find(weight_list->begin(), weight_list->end(), weight);

				if (it != weight_list->end())
				{
					*it = weight_list->back();
					weight_list->pop_back();
				}
				else return false; // There is no edge with the given weight

				if (!directed)
				{
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
        for (int i = 0; i < last_vert; i++) {
            std::cout << i << "\n";
        }
        for (int i = 0; i < last_vert; i++) {
            for (auto [neighbor, weight] : arr[i]) {
                std::cout << i << " " << neighbor << "\n";
            }
        }
    }

    void print_raw() const {
        for (int i = 0; i < last_vert; i++) {
			if (label[i] != "")
			{
				std::cout << i << " : \"" << label[i] << "\" ) | ";
			}
			else
			{
				std::cout << i << " ) | ";
			}
            for (auto [neighbor, weight_list] : arr[i]) {
                std::cout << neighbor << "{";
				int n = weight_list.size();
				std::cout << weight_list[0];
				for (int i = 1; i < n; i++)
				{
					std::cout << ", " << weight_list[i];
				}
				std::cout << "} | ";
            }
            std::cout << "\n";
        }
    }

};
