#include <iostream>
#include <vector>
#include <limits>

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

//-------------------Graf--------------------//
class Graf{
private:
  std::vector<std::vector<std::pair<int, double>>> cvorovi;
public:
  Graf(std::vector<std::vector<std::pair<int, double>>> svi) {
    cvorovi = svi;
  }
  void ShortestPathBinaryHeap(int cvor1, int cvor2) {
    std::vector<std::pair<bool,double>> posjecen(cvorovi.size(), {false, std::numeric_limits<double>::infinity()});
    MinHrpa<double> hrpa;

    int trenutniCvor = cvor1;
    posjecen[trenutniCvor] = {true, 0};

    while(posjecen[cvor2].first == false){
      for(int i = 0; i < cvorovi[trenutniCvor].size(); i++) {
        hrpa.insert(cvorovi[trenutniCvor][i].first, posjecen[trenutniCvor].second+cvorovi[trenutniCvor][i].second);
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
    std::cout << "Najkraci potevi: ";
    for(int i = 0; i < posjecen.size(); i++)
      std::cout <<posjecen[i].second << " ";
    std::cout << "\n";
  }
};
int main()
{
  std::vector<std::vector<std::pair<int, double>>> listaSusjedstva = {
      { {1, 1}, {2, 3}, {3, 6}, {4, 10}, {5, 15} },
      { {0, 1}, {2, 2}, {3, 5}, {4, 9}, {5, 14} },
      { {0, 3}, {1, 2}, {3, 3}, {4, 7}, {5, 12} },
      { {0, 6}, {1, 5}, {2, 3}, {4, 4}, {5, 9} },
      { {0, 10}, {1, 9}, {2, 7}, {3, 4}, {5, 5} },
      { {0, 15}, {1, 14}, {2, 12}, {3, 9}, {4, 5} }
  };
  Graf mojGraf(listaSusjedstva);
  mojGraf.ShortestPathBinaryHeap(0,5);

  std::vector<std::vector<std::pair<int, double>>> listaSusjedstva2 = {
    // Čvor 0: -> 1 (težina 3), -> 2 (težina 5)
    { {1, 3}, {2, 5} },

    // Čvor 1: -> 3 (težina 2), -> 4 (težina 4)
    { {3, 2}, {4, 4} },

    // Čvor 2: Nema direktnih izlaznih grana (osim samog sebe)
    { },

    // Čvor 3: -> 2 (težina 7), -> 4 (težina 2), -> 5 (težina 3)
    { {2, 7}, {4, 2}, {5, 3} },

    // Čvor 4: -> 6 (težina 2)
    { {6, 2} },

    // Čvor 5: -> 6 (težina 4)
    { {6, 4} },

    // Čvor 6: Nema direktnih izlaznih grana (osim samog sebe)
    { }
  };
  std::cout << "Test 2:\n";
  Graf mojGraf2(listaSusjedstva2);
  mojGraf2.ShortestPathBinaryHeap(0,6);
  return 0;
}
