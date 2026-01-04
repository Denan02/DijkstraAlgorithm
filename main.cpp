#include <iostream>
#include <vector>
#include <limits>
#include <unordered_map>
#include <cmath>
#include <chrono>
#include <sstream>
#include <fstream>

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
template<class T, class Comp = std::less<T>>
class FibHeap
{
 public:

  // node
  class FibNode
  {
  public:
    FibNode(T k, void *pl)
      :key(std::move(k)),mark(false),p(nullptr),left(nullptr),right(nullptr),child(nullptr),degree(-1),payload(pl)
    {
    }

    ~FibNode()
      {
      }

    T key;
    bool mark;
    FibNode *p;
    FibNode *left;
    FibNode *right;
    FibNode *child;
    int degree;
    void *payload;
  }; // end FibNode

  FibHeap() : FibHeap(std::less<T>())
    {
    }

  FibHeap(Comp comp)
      :n(0), min(nullptr), comp(comp)
  {
  }

  ~FibHeap()
    {
      clear();
    }

  void clear() {
      // delete all nodes.
      delete_fibnodes(min);
      min = nullptr;
      n = 0;
  }

  void delete_fibnodes(FibNode *x)
  {
    if (!x)
      return;

    FibNode *cur = x;
    while(true)
      {
	/*std::cerr << "cur: " << cur << std::endl;
	  std::cerr << "x: " << x << std::endl;*/
	if (cur->left && cur->left != x)
	  {
	    //std::cerr << "cur left: " << cur->left << std::endl;
	    FibNode *tmp = cur;
	    cur = cur->left;
	    if (tmp->child)
	      delete_fibnodes(tmp->child);
	    delete tmp;
	  }
	else
	  {
	    if (cur->child)
	      delete_fibnodes(cur->child);
	    delete cur;
	    break;
	  }
      }
  }
  void insert(FibNode *x)
  {
    // 1
    x->degree = 0;
    // 2
    x->p = nullptr;
    // 3
    x->child = nullptr;
    // 4
    x->mark = false;
    // 5
    if ( min == nullptr)
      {
	// 6, 7
	min = x->left = x->right = x;
      }
    else
      {
	// 8
	min->left->right = x;
	x->left = min->left;
	min->left = x;
	x->right = min;
	// 9
	if ( comp(x->key, min->key) )
	  {
	    // 10
	    min = x;
	  }
      }
    // 11
    ++n;
  }

  /*
   * The minimum node of the heap.
   */
  FibNode* minimum()
  {
    return min;
  }

  static FibHeap* union_fibheap(FibHeap *H1, FibHeap *H2)
  {
    // 1
    FibHeap* H = new FibHeap();
    // 2
    H->min = H1->min;
    // 3
    if ( H->min != nullptr && H2->min != nullptr )
      {
	H->min->right->left = H2->min->left;
	H2->min->left->right = H->min->right;
	H->min->right = H2->min;
	H2->min->left = H->min;
      }
    // 4
    if ( H1->min == nullptr || ( H2->min != nullptr && H1->comp(H2->min->key, H1->min->key) ) )
      {
	// 5
	H->min = H2->min;
      }
    // 6
    H->n = H1->n + H2->n;
    // 7
    return H;
  }

  FibNode* extract_min()
  {
    FibNode *z, *x, *next;
    FibNode ** childList;

    // 1
    z = min;
    // 2
    if ( z != nullptr )
      {
	// 3
	x = z->child;
	if ( x != nullptr )
	  {
	    childList = new FibNode*[z->degree];
	    next = x;
	    for ( int i = 0; i < (int)z->degree; i++ )
	      {
		childList[i] = next;
		next = next->right;
	      }
	    for ( int i = 0; i < (int)z->degree; i++ )
	      {
		x = childList[i];
		// 4
		min->left->right = x;
		x->left = min->left;
		min->left = x;
		x->right = min;
		// 5
		x->p = nullptr;
	      }
	    delete [] childList;
	  }
	// 6
	z->left->right = z->right;
	z->right->left = z->left;
	// 7
	if ( z == z->right )
	  {
	    // 8
	    min = nullptr;
	  }
	else
	  {
	    // 9
	    min = z->right;
	    // 10
	    consolidate();
	  }
	// 11
	n--;
      }
    // 12
    return z;
  }
  void consolidate()
  {
    FibNode* w, * next, * x, * y, * temp;
    FibNode** A, ** rootList;
    // Max degree <= log base golden ratio of n
    int d, rootSize;
    int max_degree = static_cast<int>(floor(log(static_cast<double>(n))/log(static_cast<double>(1 + sqrt(static_cast<double>(5)))/2)));

    // 1
    A = new FibNode*[max_degree+2]; // plus two both for indexing to max degree and so A[max_degree+1] == NIL
    // 2, 3
    std::fill_n(A, max_degree+2, nullptr);
    // 4
    w = min;
    rootSize = 0;
    next = w;
    do
      {
	rootSize++;
	next = next->right;
      } while ( next != w );
    rootList = new FibNode*[rootSize];
    for ( int i = 0; i < rootSize; i++ )
      {
	rootList[i] = next;
	next = next->right;
      }
    for ( int i = 0; i < rootSize; i++ )
      {
	w = rootList[i];
	// 5
	x = w;
	// 6
	d = x->degree;
	// 7
	while ( A[d] != nullptr )
	  {
	    // 8
	    y = A[d];
	    // 9
	    if ( comp(y->key, x->key) )
	      {
		// 10
		temp = x;
		x = y;
		y = temp;
	      }
	    // 11
	    fib_heap_link(y,x);
	    // 12
	    A[d] = nullptr;
	    // 13
	    d++;
	  }
	// 14
	A[d] = x;
      }
    delete [] rootList;
    // 15
    min = nullptr;
    // 16
    for ( int i = 0; i < max_degree+2; i++ )
      {
	// 17
	if ( A[i] != nullptr )
	  {
	    // 18
	    if ( min == nullptr )
	      {
		// 19, 20
		min = A[i]->left = A[i]->right = A[i];
	      }
	    else
	      {
		// 21
		min->left->right = A[i];
		A[i]->left = min->left;
		min->left = A[i];
		A[i]->right = min;
		// 22
		if ( comp(A[i]->key, min->key) )
		  {
		    // 23
		    min = A[i];
		  }
	      }
	  }
      }
    delete [] A;
  }
  void fib_heap_link( FibNode* y, FibNode* x )
  {
    // 1
    y->left->right = y->right;
    y->right->left = y->left;
    // 2
    if ( x->child != nullptr )
      {
	x->child->left->right = y;
	y->left = x->child->left;
	x->child->left = y;
	y->right = x->child;
      }
    else
      {
	x->child = y;
	y->right = y;
	y->left = y;
      }
    y->p = x;
    x->degree++;
    // 3
    y->mark = false;
  }
  void decrease_key( FibNode* x, T k )
  {
    FibNode* y;

    // 1
    if ( comp(x->key, k) )
      {
	// 2
	// error( "new key is greater than current key" );
	return;
      }
    // 3
    x->key = std::move(k);
    // 4
    y = x->p;
    // 5
    if ( y != nullptr && comp(x->key, y->key) )
      {
	// 6
	cut(x,y);
	// 7
	cascading_cut(y);
      }
    // 8
    if ( comp(x->key, min->key) )
      {
	// 9
	min = x;
      }
  }

  void cut( FibNode* x, FibNode* y )
  {
    // 1
    if ( x->right == x )
      {
	y->child = nullptr;
      }
    else
      {
	x->right->left = x->left;
	x->left->right = x->right;
	if ( y->child == x )
	  {
	    y->child = x->right;
	  }
      }
    y->degree--;
    // 2
    min->right->left = x;
    x->right = min->right;
    min->right = x;
    x->left = min;
    // 3
    x->p = nullptr;
    // 4
    x->mark = false;
  }

  void cascading_cut( FibNode* y )
  {
    FibNode* z;

    // 1
    z = y->p;
    // 2
    if ( z != nullptr )
      {
	// 3
	if ( y->mark == false )
	  {
	    // 4
	    y->mark = true;
	  }
	else
	  {
	    // 5
	    cut(y,z);
	    // 6
	    cascading_cut(z);
	  }
      }
  }
  void remove_fibnode( FibNode* x )
  {
    decrease_key(x,std::numeric_limits<T>::min());
    FibNode *fn = extract_min();
    delete fn;
  }

  bool empty() const
  {
    return n == 0;
  }

  FibNode* topNode()
  {
    return minimum();
  }

  T& top()
  {
    return minimum()->key;
  }

  void pop()
  {
    if (empty())
      return;
    FibNode *x = extract_min();
    if (x)
      delete x;
  }

  FibNode* push(T k, void *pl)
  {
    FibNode *x = new FibNode(std::move(k),pl);
    insert(x);
    return x;
  }

  FibNode* push(T k)
  {
    return push(std::move(k),nullptr);
  }

  unsigned int size()
  {
    return (unsigned int) n;
  }

  int n;
  FibNode *min;
  Comp comp;

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
    if(cvor2 > cvorovi.size() || cvor1 > cvorovi.size() || cvor2 < 0 || cvor1 < 0)
      throw std::domain_error("Cvor ne postoji!");

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
    if(posjecen[cvor2].first == true)
      return posjecen[cvor2].second;
    else
      throw std::domain_error("Put izmedju ova dva cvora ne postoji");
  }
double ShortestPathFibonacciHeap(int cvor1, int cvor2) {
    if(cvor2 > cvorovi.size() || cvor1 > cvorovi.size() || cvor2 < 0 || cvor1 < 0)
      throw std::domain_error("Cvor ne postoji!");
    std::vector<double> dist(cvorovi.size(), std::numeric_limits<double>::infinity());

    // 2. KLJUČ: Niz pokazivača (handles) koji nam kaže gdje se koji čvor nalazi u gomili
    // Ako je handles[i] == nullptr, čvor i nije u gomili.
    std::vector<FibHeap<double>::FibNode*> handles(cvorovi.size(), nullptr);

    FibHeap<double> hrpa;

    // Postavljamo početni čvor
    dist[cvor1] = 0;
    handles[cvor1] = hrpa.push(0.0, (void*)(intptr_t)cvor1);

    while (!hrpa.empty()) {
        FibHeap<double>::FibNode* minimalni = hrpa.extract_min();
        if (!minimalni) break;

        // Dobijamo ID čvora iz payload-a (ono što smo spremili pri push-u)
        int u = (int)(intptr_t)minimalni->payload;
        double trenutnaDistanca = minimalni->key;

        // VAŽNO: Pošto je izbačen iz gomile, poništavamo njegovu adresu u handles
        handles[u] = nullptr;

        // Obavezno obrisati memoriju jer extract_min samo izbacuje, ne briše objekat
        delete minimalni;

        // Ako smo stigli do cilja, možemo odmah vratiti rezultat
        if (u == cvor2) return dist[cvor2];

        // Ako smo izvukli distancu koja je veća od već pronađene, preskoči (optimizacija)
        if (trenutnaDistanca > dist[u]) continue;

        // Prođi kroz sve susjede čvora u
        for (auto& susjed : cvorovi[u]) {
            int v = susjed.first;
            double tezinaIvice = susjed.second;
            double novaDistanca = dist[u] + tezinaIvice;

            // Ako smo pronašli bolji put do susjeda v
            if (novaDistanca < dist[v]) {
                dist[v] = novaDistanca;

                if (handles[v] == nullptr) {
                    // AKO NE POSTOJI u gomili -> UBACI GA (Insert)
                    // Spremamo vraćenu adresu u handles[v]
                    handles[v] = hrpa.push(dist[v], (void*)(intptr_t)v);
                }
                else {
                    // AKO VEĆ POSTOJI u gomili -> SAMO PREPRAVI (Decrease Key)
                    // Koristimo sačuvanu adresu iz handles[v] da bi bilo O(1)
                    hrpa.decrease_key(handles[v], dist[v]);
                }
            }
        }
    }
    if (dist[cvor2] == std::numeric_limits<double>::infinity()) {
        return -1.0; // Vraćamo "malu" negativnu vrijednost kao signal za "nema puta"
    }
    return dist[cvor2];
}
};
int main()
{

  std::vector<std::vector<std::pair<int, double>>> listaSusjedstva1 = {
    {{4, 0.38}, {2, 0.26}}, // Cvor 0
    {{3, 0.29}}, // Cvor 1
    {{7, 0.34}}, // Cvor 2
    {{6, 0.52}}, // Cvor 3
    {{5, 0.35}, {7, 0.37}}, // Cvor 4
    {{4, 0.35}, {7, 0.28}, {1, 0.32}}, // Cvor 5
    {{2, 0.4}, {0, 0.58}, {4, 0.93}}, // Cvor 6
    {{5, 0.28}, {3, 0.39}} // Cvor 7
  };

    Graf mojGraf(listaSusjedstva1);
    std::cout << "Binary Heap Test 1:\n";

    auto startBinary = std::chrono::high_resolution_clock::now();
    std::cout << mojGraf.ShortestPathBinaryHeap(0, 7) << " ";
    auto endBinary = std::chrono::high_resolution_clock::now();



    std::chrono::duration<double, std::milli> timeBinary = endBinary - startBinary;

    std::cout << "(Time: " << timeBinary.count() << " ms)\n";



    std::cout << "Fibonacci Heap Test:\n";
    auto startFibo = std::chrono::high_resolution_clock::now();
    std::cout <<mojGraf.ShortestPathFibonacciHeap(0, 7)<<" ";

    auto endFibo = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> timeFibo = endFibo - startFibo;

    std::cout << "(Time: " << timeFibo.count() << " ms)\n";
    return 0;
}
