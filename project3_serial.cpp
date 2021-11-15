#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <chrono>

using namespace std;

template <class K>
class HashTable
{
    private:
        K a;
    public:
        HashTable (K one)
        {
            this->a = one;
        }
        K getA()
        {
            return a;
        }
};

int main(int argc, char** argv) 
{
    HashTable <int> a = HashTable<int>(5);
    printf ("%d", a.getA());
}
