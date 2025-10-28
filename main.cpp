#include <iostream>
#include <vector>

class MinHrpa{
private:
  std::vector<double> niz;
public:
  MinHrpa() {
    niz = std::vector<double>(0);
  }
  void insert(double x) {
    niz.push_back(x);
    int i = niz.size()-1;
    while(true) {
      int temp = (i-1)/2;
      if(i-1 < 0 || niz[temp] < niz[i]) {
        break;
      }else {
        std::swap(niz[i], niz[temp]);
        i = temp;
      }
    }
  }
  double getMin() {
    if(niz.size() == 0)
      throw std::domain_error("Hrpa je prazana!");
    double minBroj = niz[0];
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

    if(left < n && niz[left] < niz[smallest]) smallest = left;
    if(right < n && niz[right] < niz[smallest]) smallest = right;

    if(smallest != i) {
        std::swap(niz[i], niz[smallest]);
        this->Heapify(smallest);
    }
  }
  void ispisi() {
    for(int i = 0; i < niz.size(); i++) {
      std::cout << niz[i] << ", ";
    }
  }
};

int main()
{
  MinHrpa m = MinHrpa();
  m.insert(2);
  m.insert(3);
  m.insert(10);
  m.insert(4);
  m.insert(5);
  m.insert(1);
  m.ispisi();
  std::cout << "\n";
  std::cout << m.getMin() << " ";
  m.ispisi();
  std::cout << "\n";
  std::cout << m.getMin() << " ";
  m.ispisi();
    std::cout << "\n";
  std::cout << m.getMin() << " ";
  m.ispisi();
  std::cout << "\n";
  std::cout << m.getMin() << " ";
  m.ispisi();
    std::cout << "\n";
  std::cout << m.getMin() << " ";
  m.ispisi();
    std::cout << "\n";
  std::cout << m.getMin() << " ";
  m.ispisi();
    std::cout << "\n";
  return 0;
}
