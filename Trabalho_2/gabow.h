#ifndef GABOW_H
#define GABOW_H

#include "arborescence.h"
#include <algorithm>
#include <stack>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <limits>
#include <cmath>

// ============================================================================
// ALGORITMO DE GABOW ET AL. (1986) - IMPLEMENTAÇÃO FIEL
// ============================================================================
class GabowAlgorithm {
private:
    static constexpr double INFINITE_COST = std::numeric_limits<double>::infinity();
    
    class FibonacciHeap {
    private:
        struct Node {
            int vertex;
            double key;
            Node* parent;
            Node* child;
            Node* left;
            Node* right;
            int degree;
            bool marked;
            
            Node(int v, double k) : vertex(v), key(k), parent(nullptr), child(nullptr),
                                  left(this), right(this), degree(0), marked(false) {}
        };
        
        Node* min_node;
        int node_count;
        std::unordered_map<int, Node*> vertex_to_node;
        
        void link(Node* y, Node* x) {
            y->left->right = y->right;
            y->right->left = y->left;
            
            y->parent = x;
            if (x->child == nullptr) {
                x->child = y;
                y->left = y;
                y->right = y;
            } else {
                y->left = x->child;
                y->right = x->child->right;
                x->child->right->left = y;
                x->child->right = y;
            }
            
            x->degree++;
            y->marked = false;
        }
        
        void consolidate() {
            if (min_node == nullptr) return;
            
            int max_degree = static_cast<int>(std::log2(std::max(1, node_count))) + 2;
            std::vector<Node*> degree_table(max_degree + 1, nullptr);
            
            std::vector<Node*> roots;
            Node* current = min_node;
            if (current != nullptr) {
                do {
                    roots.push_back(current);
                    current = current->right;
                } while (current != min_node);
            }
            
            for (Node* node : roots) {
                Node* x = node;
                int d = x->degree;
                while (degree_table[d] != nullptr) {
                    Node* y = degree_table[d];
                    if (x->key > y->key) {
                        std::swap(x, y);
                    }
                    link(y, x);
                    degree_table[d] = nullptr;
                    d++;
                }
                degree_table[d] = x;
            }
            
            min_node = nullptr;
            for (Node* node : degree_table) {
                if (node != nullptr) {
                    if (min_node == nullptr) {
                        min_node = node;
                        node->left = node;
                        node->right = node;
                    } else {
                        node->left = min_node;
                        node->right = min_node->right;
                        min_node->right->left = node;
                        min_node->right = node;
                        if (node->key < min_node->key) {
                            min_node = node;
                        }
                    }
                }
            }
        }
        
        void cut(Node* x, Node* y) {
            if (x->right == x) {
                y->child = nullptr;
            } else {
                x->left->right = x->right;
                x->right->left = x->left;
                if (y->child == x) {
                    y->child = x->right;
                }
            }
            y->degree--;
            
            x->parent = nullptr;
            x->marked = false;
            x->left = min_node;
            x->right = min_node->right;
            min_node->right->left = x;
            min_node->right = x;
        }
        
        void cascading_cut(Node* y) {
            Node* z = y->parent;
            if (z != nullptr) {
                if (!y->marked) {
                    y->marked = true;
                } else {
                    cut(y, z);
                    cascading_cut(z);
                }
            }
        }
        
    public:
        FibonacciHeap() : min_node(nullptr), node_count(0) {}
        
        bool empty() const {
            return min_node == nullptr;
        }
        
        void insert(int vertex, double key) {
            Node* node = new Node(vertex, key);
            vertex_to_node[vertex] = node;
            
            if (min_node == nullptr) {
                min_node = node;
            } else {
                node->left = min_node;
                node->right = min_node->right;
                min_node->right->left = node;
                min_node->right = node;
                
                if (key < min_node->key) {
                    min_node = node;
                }
            }
            node_count++;
        }
        
        std::pair<int, double> delete_min() {
            if (min_node == nullptr) {
                return {-1, INFINITE_COST};
            }
            
            Node* z = min_node;
            int vertex = z->vertex;
            double key = z->key;
            vertex_to_node.erase(vertex);
            
            if (z->child != nullptr) {
                Node* child = z->child;
                do {
                    Node* next = child->right;
                    child->parent = nullptr;
                    child->left = min_node;
                    child->right = min_node->right;
                    min_node->right->left = child;
                    min_node->right = child;
                    child = next;
                } while (child != z->child);
            }
            
            z->left->right = z->right;
            z->right->left = z->left;
            
            if (z == z->right) {
                min_node = nullptr;
            } else {
                min_node = z->right;
                consolidate();
            }
            
            delete z;
            node_count--;
            return {vertex, key};
        }
        
        void decrease_key(int vertex, double new_key) {
            Node* x = vertex_to_node[vertex];
            if (new_key > x->key) return;
            
            x->key = new_key;
            Node* y = x->parent;
            
            if (y != nullptr && x->key < y->key) {
                cut(x, y);
                cascading_cut(y);
            }
            
            if (x->key < min_node->key) {
                min_node = x;
            }
        }
        
        void meld(FibonacciHeap& other) {
            if (other.min_node == nullptr) return;
            
            if (min_node == nullptr) {
                min_node = other.min_node;
                node_count = other.node_count;
                vertex_to_node = std::move(other.vertex_to_node);
            } else {
                Node* this_right = min_node->right;
                Node* other_left = other.min_node->left;
                
                min_node->right = other.min_node;
                other.min_node->left = min_node;
                this_right->left = other_left;
                other_left->right = this_right;
                
                if (other.min_node->key < min_node->key) {
                    min_node = other.min_node;
                }
                
                node_count += other.node_count;
                vertex_to_node.merge(other.vertex_to_node);
            }
            
            other.min_node = nullptr;
            other.node_count = 0;
            other.vertex_to_node.clear();
        }
    };

    struct CompressedTree {
        std::vector<int> parent;
        std::vector<int> rank;
        std::vector<double> value;
        
        CompressedTree(int n) : parent(n), rank(n, 0), value(n, 0.0) {
            for (int i = 0; i < n; i++) {
                parent[i] = i;
            }
        }
        
        int find(int v) {
            if (parent[v] != v) {
                int old_parent = parent[v];
                parent[v] = find(parent[v]);
                value[v] += value[old_parent];
            }
            return parent[v];
        }
        
        double find_value(int v) {
            find(v);
            return value[v];
        }
        
        void change_value(double delta, int set_root) {
            value[set_root] += delta;
        }
        
        void unite_sets(int root1, int root2, int new_root) {
            if (rank[root1] < rank[root2]) {
                parent[root1] = new_root;
                parent[root2] = new_root;
            } else if (rank[root1] > rank[root2]) {
                parent[root1] = new_root;
                parent[root2] = new_root;
            } else {
                parent[root1] = new_root;
                parent[root2] = new_root;
                rank[new_root] = rank[root1] + 1;
            }
        }
    };

    struct ExitList {
        std::list<std::pair<int, double>> edges;
        
        void add_edge(int target, double cost) {
            edges.emplace_front(target, cost);
        }
        
        bool empty() const { return edges.empty(); }
        
        std::pair<int, double> get_active_edge() const {
            return edges.empty() ? std::make_pair(-1, INFINITE_COST) : edges.front();
        }
    };
    
    struct PassiveSet {
        std::unordered_set<int> edges;
        
        void add_edge(int edge_id) { edges.insert(edge_id); }
        void remove_edge(int edge_id) { edges.erase(edge_id); }
        bool empty() const { return edges.empty(); }
    };

    ArborescenceResult gabow_core_algorithm(DirectedGraph& graph, int root_vertex) {
        int n = graph.vertex_count();
        ArborescenceResult result(n, root_vertex);
        
        if (root_vertex < 0 || root_vertex >= n) {
            return result;
        }
        
        CompressedTree cost_tree(n);
        std::vector<ExitList> exit_lists(n);
        std::vector<PassiveSet> passive_sets(n);
        std::vector<FibonacciHeap> vertex_heaps(n);
        std::vector<int> growth_path = {root_vertex};
        std::vector<bool> on_growth_path(n, false);
        on_growth_path[root_vertex] = true;
        
        for (int v = 0; v < n; v++) {
            if (v == root_vertex) continue;
            
            auto sources = graph.get_sources_to(v);
            for (const auto& [u, cost] : sources) {
                exit_lists[u].add_edge(v, cost);
                
                if (exit_lists[u].get_active_edge().first == v) {
                    double adjusted_cost = cost + cost_tree.find_value(v);
                    vertex_heaps[v].insert(u, adjusted_cost);
                }
            }
        }
        
        while (growth_path.size() < n) {
            int current_root = growth_path[0];
            
            if (vertex_heaps[current_root].empty()) {
                return result;
            }
            
            auto [u, cost] = vertex_heaps[current_root].delete_min();
            int u_root = cost_tree.find(u);
            
            if (!on_growth_path[u_root]) {
                growth_path.insert(growth_path.begin(), u_root);
                on_growth_path[u_root] = true;
                
                auto sources = graph.get_sources_to(u_root);
                for (const auto& [x, x_cost] : sources) {
                    if (on_growth_path[cost_tree.find(x)]) continue;
                    
                    exit_lists[x].add_edge(u_root, x_cost);
                    if (exit_lists[x].get_active_edge().first == u_root) {
                        double adjusted_cost = x_cost + cost_tree.find_value(u_root);
                        vertex_heaps[u_root].insert(x, adjusted_cost);
                    }
                }
                
            } else {
                std::vector<int> cycle;
                int k = -1;
                for (size_t i = 0; i < growth_path.size(); i++) {
                    if (growth_path[i] == u_root) {
                        k = static_cast<int>(i);
                        break;
                    }
                }
                
                if (k == -1) {
                    continue;
                }
                
                for (int i = 0; i <= k; i++) {
                    cycle.push_back(growth_path[i]);
                }
                
                for (int i = 0; i <= k; i++) {
                    int v_i = cycle[i];
                    double min_cost = INFINITE_COST;
                    for (const auto& [source, edge_cost] : exit_lists[v_i].edges) {
                        double current_cost = edge_cost + cost_tree.find_value(v_i);
                        min_cost = std::min(min_cost, current_cost);
                    }
                    
                    if (min_cost < INFINITE_COST) {
                        cost_tree.change_value(-min_cost, v_i);
                    }
                }
                
                int cycle_root = cycle[0];
                for (int i = 1; i <= k; i++) {
                    cost_tree.unite_sets(cycle[i], cycle_root, cycle_root);
                }
                
                growth_path.erase(growth_path.begin(), growth_path.begin() + k + 1);
                growth_path.insert(growth_path.begin(), cycle_root);
                
                FibonacciHeap new_heap;
                for (int v : cycle) {
                    new_heap.meld(vertex_heaps[v]);
                }
                vertex_heaps[cycle_root] = std::move(new_heap);
                
                ExitList new_exit_list;
                for (int v : cycle) {
                    for (const auto& [target, edge_cost] : exit_lists[v].edges) {
                        int target_root = cost_tree.find(target);
                        if (std::find(cycle.begin(), cycle.end(), target_root) == cycle.end()) {
                            new_exit_list.add_edge(target, edge_cost);
                        }
                    }
                }
                exit_lists[cycle_root] = std::move(new_exit_list);
            }
        }
        
        result.total_tree_cost = 0.0;
        result.is_complete = true;
        
        for (int v = 0; v < n; v++) {
            if (v == root_vertex) {
                result.parent_of[v] = -1;
                result.edge_costs[v] = 0.0;
            } else {
                auto sources = graph.get_sources_to(v);
                double min_cost = INFINITE_COST;
                int best_source = -1;
                
                for (const auto& [u, edge_cost] : sources) {
                    double adjusted_cost = edge_cost + cost_tree.find_value(v);
                    if (adjusted_cost < min_cost) {
                        min_cost = adjusted_cost;
                        best_source = u;
                    }
                }
                
                if (best_source != -1) {
                    result.parent_of[v] = best_source;
                    result.edge_costs[v] = graph.connection_cost(best_source, v);
                    result.total_tree_cost += result.edge_costs[v];
                }
            }
        }
        
        return result;
    }



public:
    ArborescenceResult find_minimum_cost_arborescence(DirectedGraph& graph, int root_vertex) {
        return gabow_core_algorithm(graph, root_vertex);
    }

    ArborescenceResult build_segmentation_hierarchy(DirectedGraph& directed_graph, int root_pixel) {
        return find_minimum_cost_arborescence(directed_graph, root_pixel);
    }
    
    ArborescenceResult segment_image(const DirectedGraph& graph, double threshold, int min_size = 0) {
        int n = graph.vertex_count();
        DirectedGraph mutable_graph = graph;
        ArborescenceResult arborescence = find_minimum_cost_arborescence(mutable_graph, 0);
        
        if (!arborescence.is_complete) {
            return ArborescenceResult(n, -1);
        }
        
        std::vector<int> component(n, -1);
        std::vector<bool> visited(n, false);
        int current_component = 0;
        
        for (int i = 0; i < n; i++) {
            if (!visited[i]) {
                std::vector<int> current_comp;
                std::stack<int> stack;
                stack.push(i);
                
                while (!stack.empty()) {
                    int v = stack.top();
                    stack.pop();
                    
                    if (!visited[v]) {
                        visited[v] = true;
                        component[v] = current_component;
                        current_comp.push_back(v);
                        
                        auto children = arborescence.get_children_of(v);
                        for (int child : children) {
                            if (arborescence.edge_costs[child] <= threshold) {
                                stack.push(child);
                            }
                        }
                    }
                }
                
                if (min_size == 0 || static_cast<int>(current_comp.size()) >= min_size) {
                    current_component++;
                }
            }
        }
        
        ArborescenceResult result(n, -1);
        for (int i = 0; i < n; i++) {
            result.parent_of[i] = component[i];
        }
        result.is_complete = true;
        
        return result;
    }
};

#endif
