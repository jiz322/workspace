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
        int sizeOfTable;
        int HASH1 = 2147483647;
        int HASH2 = 479001599;
        int START = 1; //for populate table

        HashTable (int sizeOfTable)
        {
            this->sizeOfTable = sizeOfTable;
            values1.assign(sizeOfTable, NULL);
            values2.assign(sizeOfTable, NULL);
        }

        int hash (T x, int nm)
        {
            return (int)nm%x%sizeOfTable;
        }

        bool contains (T x)
        {
            int hash1 = hash(x, HASH1);
            int hash2 = hash(x, HASH2);
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

        //double the sizeOfTable of both table
        //have to compute 
        void resize ()
        {
            int previousSize = sizeOfTable;
            sizeOfTable = 2*sizeOfTable;
            values1.resize(sizeOfTable*2, NULL);
            values2.resize(sizeOfTable*2, NULL);
            //Waste compute power here since half of table is NULL
            for (typename std::vector<T>::iterator it = values1.begin() ; it != values1.end(); ++it)
            {
                if (*it != NULL){
                    //erase this and call add
                    T x = *it;
                    values1.erase(it);
                    add(x);
                }
            }
            for (typename std::vector<T>::iterator it = values2.begin() ; it != values2.end(); ++it)
            {
                if (*it != NULL){
                    T x = *it;
                    values2.erase(it);
                    add(x);
                }
            }
        }

        bool add (T x)
        {
            if (contains(x))
            {
                return false;
            }
            int LIMIT =  sizeOfTable/2;
            int hash1 = hash(x, HASH1); //suprisely, this is a prime number
            int hash2 = hash(x, HASH2); // another prime number
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

        bool remove(T x)
        {
            int hash1 = hash(x, HASH1); 
            int hash2 = hash(x, HASH2);
            if (values1.at(hash1) == x)
            {
                swap(NULL, hash1, 1);
                return true;
            }
            else if(values2.at(hash2) == x)
            {
                swap(NULL, hash2, 2);
                return true;
            }
            else
            {
                return false;
            }
        }
        
        int size()
        {
            int count = 0;
            for (typename std::vector<T>::iterator it = values1.begin() ; it != values1.end(); ++it)
            {
                if (*it != NULL){
                    count ++;
                }
            }
            for (typename std::vector<T>::iterator it = values2.begin() ; it != values2.end(); ++it)
            {
                if (*it != NULL){
                    count ++;
                }
            }
            return count;
        }

        //unsafe -- only works for int hashtable
        //unsafe -- 0 is not allowed
        //ToDO: change hash function
        //populate integers from 1 to size
        void populate(int size)
        {
            for (int i = START; i < size+START; i++){
                add(i);
            }
        }


};

int main(int argc, char** argv) 
{
    HashTable <int> a = HashTable<int>(5);
//     a.add(3);
//     a.add(4);
//     a.add(5);
//     a.add(6);
//     a.add(7);
//     //a.add(8);
//    // a.add(9);
//     printf("%d", a.sizeOfTable);

//     printf ("%d", a.values1[0]);
//     printf ("%d", a.values1[1]);
//     printf ("%d", a.values1[2]);
//     printf ("%d", a.values1[3]);
//     printf ("%d", a.values1[4]);
//     printf ("%d", a.values2[0]);
//     printf ("%d", a.values2[1]);
//     printf ("%d", a.values2[2]);
//     printf ("%d", a.values2[3]);
//     printf ("%d", a.values2[4]);

//     printf ("%d", a.contains(4));
//     printf ("%d", a.contains(3));
//     printf ("%d", a.contains(5));
//     printf ("size%d", a.size());
//     printf ("%d", a.remove(4));
//     printf ("%d", a.remove(3));
//     printf ("%d", a.remove(5));
//     printf ("%d", a.contains(4));
//     printf ("%d", a.contains(3));
//     printf ("%d", a.contains(5));
//     printf ("size%d", a.size());
    a.populate(10);
    printf ("%d", a.values1[0]);
    printf ("%d", a.values1[1]);
    printf ("%d", a.values1[2]);
    printf ("%d", a.values1[3]);
    printf ("%d", a.contains(3));
    printf ("size%d", a.size());

}
