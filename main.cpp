#include <iostream>
#include <vector>

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

template<typename T2>
class Graf{
private:
  std::vector<std::vector<std::pair<int, T2>>> cvorovi;
public:
  Graf(std::vector<std::vector<std::pair<int, T2>>> svi) {
    cvorovi = svi;
  }
  void ShortestPathBinaryHeap(int cvor1, int cvor2) {

  }
};
int main()
{

  return 0;
}
