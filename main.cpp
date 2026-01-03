#include <iostream>
#include <vector>
#include <limits>
#include <unordered_map>
#include <cmath>

//-------------------MinHeap--------------------//
template<typename T>
class MinHrpa{
private:
  std::vector<std::pair<int, T>> niz;
public:
  void insert(int cvor, T x) {
    niz.push_back({cvor, x});
    int i = niz.size()-1;
    while(true) {
      int temp = (i-1)/2;
      if(i-1 < 0 || niz[temp].second < niz[i].second) {
        break;
      }else {
        std::swap(niz[i], niz[temp]);
        i = temp;
      }
    }
  }
  int Heap_size() {
    return niz.size();
  }
  std::pair<int, T> getMin() {
    if(niz.size() == 0)
      throw std::domain_error("Hrpa je prazana!");
    std::pair<int, T> minBroj = niz[0];
    niz[0] = niz.back();
    niz.pop_back();
    this->Heapify(0);
    return minBroj;
  }
  void Heapify(int i) {
    int n = niz.size();
    int smallest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;

    if(left < n && niz[left].second < niz[smallest].second) smallest = left;
    if(right < n && niz[right].second < niz[smallest].second) smallest = right;

    if(smallest != i) {
        std::swap(niz[i], niz[smallest]);
        this->Heapify(smallest);
    }
  }
};

//-------------------Fibonacci Heap--------------------//
struct FibNode {
    int key;
    double value;
    int degree = 0;
    bool mark = false;
    FibNode *parent = nullptr, *child = nullptr, *left = nullptr, *right = nullptr;

    FibNode(int k, double v) : key(k), value(v) {
        left = right = this;
    }
};

class FibHeap {
private:
    FibNode* minNode = nullptr;
    int n = 0;
    std::unordered_map<int, FibNode*> nodeMap;

    void clear(FibNode* x) {
        if (x) {
            FibNode* start = x;
            FibNode* curr = x;
            do {
                FibNode* next = curr->right;
                clear(curr->child);
                delete curr;
                curr = next;
            } while (curr != start);
        }
    }

    void link(FibNode* y, FibNode* x) {
        y->left->right = y->right;
        y->right->left = y->left;

        y->parent = x;
        if (!x->child) {
            x->child = y;
            y->left = y->right = y;
        } else {
            y->left = x->child;
            y->right = x->child->right;
            x->child->right->left = y;
            x->child->right = y;
        }
        x->degree++;
        y->mark = false;
    }

    void consolidate() {
        int max_D = (n > 0) ? (int)(std::log(n) / std::log(1.618)) + 2 : 1;
        std::vector<FibNode*> A(max_D, nullptr);

        if (!minNode) return;

        std::vector<FibNode*> rootList;
        FibNode* curr = minNode;
        do {
            rootList.push_back(curr);
            curr = curr->right;
        } while (curr != minNode);

        for (FibNode* w : rootList) {
            FibNode* x = w;
            int d = x->degree;
            while (d < max_D && A[d]) {
                FibNode* y = A[d];
                if (x->value > y->value) std::swap(x, y);
                link(y, x);
                A[d] = nullptr;
                d++;
            }
            if (d < max_D) A[d] = x;
        }

        minNode = nullptr;
        for (FibNode* node : A) {
            if (node) {
                if (!minNode) {
                    minNode = node;
                    node->left = node->right = node;
                } else {
                    node->left = minNode;
                    node->right = minNode->right;
                    minNode->right->left = node;
                    minNode->right = node;
                    if (node->value < minNode->value) minNode = node;
                }
            }
        }
    }

    void cut(FibNode* x, FibNode* y) {

        if (x->right == x) {
            y->child = nullptr;
        } else {
            x->left->right = x->right;
            x->right->left = x->left;
            if (y->child == x) y->child = x->right;
        }
        y->degree--;


        x->left = minNode;
        x->right = minNode->right;
        minNode->right->left = x;
        minNode->right = x;

        x->parent = nullptr;
        x->mark = false;
    }

    void cascadingCut(FibNode* y) {
        FibNode* z = y->parent;
        if (z) {
            if (!y->mark) {
                y->mark = true;
            } else {
                cut(y, z);
                cascadingCut(z);
            }
        }
    }

public:
    FibHeap() = default;

    ~FibHeap() {
        clear(minNode);
    }

    void insert(int key, double value) {
        FibNode* node = new FibNode(key, value);
        nodeMap[key] = node;
        if (!minNode) {
            minNode = node;
        } else {
            node->left = minNode;
            node->right = minNode->right;
            minNode->right->left = node;
            minNode->right = node;
            if (node->value < minNode->value) minNode = node;
        }
        n++;
    }

    FibNode* extractMin() {
        FibNode* z = minNode;
        if (z) {
            if (z->child) {
                FibNode* child = z->child;
                std::vector<FibNode*> children;
                FibNode* curr = child;
                do {
                    children.push_back(curr);
                    curr = curr->right;
                } while (curr != child);

                for (FibNode* x : children) {
                    // Dodaj dete u root listu
                    x->left = minNode;
                    x->right = minNode->right;
                    minNode->right->left = x;
                    minNode->right = x;
                    x->parent = nullptr;
                }
            }

            z->left->right = z->right;
            z->right->left = z->left;

            if (z == z->right) {
                minNode = nullptr;
            } else {
                minNode = z->right;
                consolidate();
            }
            n--;
            nodeMap.erase(z->key);
        }
        return z;
    }

    void decreaseKey(int key, double newVal) {
        // Ako ne postoji, pozovi insert
        if (nodeMap.find(key) == nodeMap.end()) {
            insert(key, newVal);
            return;
        }
        FibNode* x = nodeMap[key];
        if (newVal > x->value) return;

        x->value = newVal;
        FibNode* y = x->parent;
        if (y && x->value < y->value) {
            cut(x, y);
            cascadingCut(y);
        }
        if (x->value < minNode->value) {
            minNode = x;
        }
    }

    void deleteNode(int key) {
        decreaseKey(key, -1e18);
        FibNode* z = extractMin();
        if (z) delete z;
    }

    bool empty() {
        return minNode == nullptr;
    }

    int size() {
        return n;
    }
};

//-------------------Graf--------------------//
class Graf{
private:
  std::vector<std::vector<std::pair<int, double>>> cvorovi;
public:
  Graf(std::vector<std::vector<std::pair<int, double>>> svi) {
    cvorovi = svi;
  }
  double ShortestPathBinaryHeap(int cvor1, int cvor2) {
    std::vector<std::pair<bool,double>> posjecen(cvorovi.size(), {false, std::numeric_limits<double>::infinity()});
    MinHrpa<double> hrpa;

    int trenutniCvor = cvor1;
    posjecen[trenutniCvor] = {true, 0};

    while(posjecen[cvor2].first == false){
      for(int i = 0; i < cvorovi[trenutniCvor].size(); i++) {
        if((posjecen[cvorovi[trenutniCvor][i].first]).first == false){
          hrpa.insert(cvorovi[trenutniCvor][i].first, posjecen[trenutniCvor].second+cvorovi[trenutniCvor][i].second);
        }
      }
      std::pair<int, double> minimalni = hrpa.getMin();
      if(posjecen[minimalni.first].first == false) {
        posjecen[minimalni.first] = {true, minimalni.second};
        trenutniCvor = minimalni.first;
      }else {
        while(posjecen[minimalni.first].first == true)
          minimalni = hrpa.getMin();
        posjecen[minimalni.first] = {true, minimalni.second};
        trenutniCvor = minimalni.first;
      }
    }
    return posjecen[cvor2].second;
  }
  double ShortestPathFibonacciHeap(int cvor1, int cvor2) {
    std::vector<std::pair<bool,double>> posjecen(cvorovi.size(), {false, std::numeric_limits<double>::infinity()});
    FibHeap hrpa;

    int trenutniCvor = cvor1;
    posjecen[trenutniCvor] = {true, 0};

    while(posjecen[cvor2].first == false){
      for(int i = 0; i < cvorovi[trenutniCvor].size(); i++) {
        hrpa.decreaseKey(cvorovi[trenutniCvor][i].first, posjecen[trenutniCvor].second+cvorovi[trenutniCvor][i].second);
      }
      FibNode* minimalni = hrpa.extractMin();
      if(posjecen[minimalni->key].first == false) {
        posjecen[minimalni->key] = {true, minimalni->value};
        trenutniCvor = minimalni->key;
      }else {
        while(posjecen[minimalni->key].first == true)
          minimalni = hrpa.extractMin();
        posjecen[minimalni->key] = {true, minimalni->value};
        trenutniCvor = minimalni->key;
      }
    }
    return posjecen[cvor2].second;
  }
};
int main()
{
   // ----------------- Testiranje sa Binary Heap -----------------
    std::vector<std::vector<std::pair<int, double>>> listaSusjedstva = {
        { {1, 1}, {2, 3}, {3, 6}, {4, 10}, {5, 15} },
        { {0, 1}, {2, 2}, {3, 5}, {4, 9}, {5, 14} },
        { {0, 3}, {1, 2}, {3, 3}, {4, 7}, {5, 12} },
        { {0, 6}, {1, 5}, {2, 3}, {4, 4}, {5, 9} },
        { {0, 10}, {1, 9}, {2, 7}, {3, 4}, {5, 5} },
        { {0, 15}, {1, 14}, {2, 12}, {3, 9}, {4, 5} }
    };
    Graf mojGraf(listaSusjedstva);
    std::cout << "Binary Heap Test 1:\n";
    std::cout << mojGraf.ShortestPathBinaryHeap(1,4) << " ";
    std::cout << mojGraf.ShortestPathFibonacciHeap(1,4);

    std::vector<std::vector<std::pair<int, double>>> listaSusjedstva2 = {
        { {1, 3}, {2, 5} },
        { {3, 2}, {4, 4} },
        { },
        { {2, 7}, {4, 2}, {5, 3} },
        { {6, 2} },
        { {6, 4} },
        { }
    };
    std::cout << "Binary Heap Test 2:\n";
    Graf mojGraf2(listaSusjedstva2);
    std::cout << mojGraf2.ShortestPathBinaryHeap(0,6) << " ";
    std::cout << mojGraf2.ShortestPathFibonacciHeap(0,6);
    return 0;
}
