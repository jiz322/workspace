#include <iostream>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <chrono>

using namespace std;

template <class T>
class HashTable
{

    public:
        std::vector<T> values1;
        std::vector<T> values2;
        int size;

        HashTable (int size)
        {
            this->size = size;
            values1.assign(size, NULL);
            values2.assign(size, NULL);
        }

        int hash (T x, int nm)
        {
            return (int)nm%x%size;
        }

        bool contains (T x)
        {
            int hash1 = hash(x, 2147483647);
            int hash2 = hash(x, 2147483646);
            if (values1.at(hash1) == x)
            {
                return true;
            }
            else if (values2.at(hash2) == x)
            {
                return true;
            }
            return false;
        }

        //double the size of both table
        void resize ()
        {
            size = 2*size;
            values1.resize(size*2, NULL);
            values2.resize(size*2, NULL);
        }

        //swap value
        T swap(T x, int i, int table)
        {
            if (table == 1)
            {
                T ret = values1.at(i);
                values1.insert (values1.begin()+i, x);
                values1.erase (values1.begin()+i+1);
                return  ret;
            }
            else
            {
                T ret = values2.at(i);
                values2.insert (values2.begin()+i, x);
                values2.erase (values2.begin()+i+1);
                return  ret;
            }
        }

        bool add (T x)
        {
            if (contains(x))
            {
                return false;
            }
            int LIMIT =  size/2;
            int hash1 = hash(x, 2147483647);
            int hash2 = hash(x, 2147483646);
            int tableToInsert = hash1%2+1; //1 or 2, randomly select the first table to insert
            for (int i = 0; i < LIMIT; i++)
            {
                if (tableToInsert == 1)
                {
                    x = swap(x, hash1, 1);
                    if (x == NULL)
                    {
                        return true;
                    }
                    tableToInsert = 2;
                }
                else //tableToInsert == 2
                {
                    x = swap(x, hash2, 2);
                    if (x == NULL)
                    {
                        return true;
                    }
                    tableToInsert = 1;
                }


            }
            resize();
            return add(x);
        }


};

int main(int argc, char** argv) 
{
    HashTable <int> a = HashTable<int>(51);
    a.add(3);
    printf ("%d", a.size);
    a.add(4);
    a.add(5);
    a.add(6);
    a.add(7);

    printf ("%d", a.values1[0]);
    printf ("%d", a.values1[1]);
    printf ("%d", a.values1[2]);
    printf ("%d", a.values1[3]);
    printf ("%d", a.values1[4]);
    printf ("%d", a.values2[0]);
    printf ("%d", a.values2[1]);
    printf ("%d", a.values2[2]);
    printf ("%d", a.values2[3]);
    printf ("%d", a.values2[4]);
    // printf ("%d", a.contains(4));
    // printf ("%d", a.contains(3));
    // printf ("%d", a.contains(5));

}
