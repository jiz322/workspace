
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <random>
#include <thread>
#include <mutex>
#include <chrono>
#include <future>
#include <unistd.h>
#include <iostream>
#include <ctime>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <string>
#include <stdlib.h>

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
        int START = 100000000; //for populate table
        int NUM_LOCKS = 30;
        vector<std::mutex> mutexes1(int NUM_LOCKS);
        vector<std::mutex> mutexes2(int NUM_LOCKS);
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
                values1.at(i) =  x;
                return  ret;
            }
            else
            {
                T ret = values2.at(i);
                values2.at(i) =  x;
                return  ret;
            }
        }

        //double the sizeOfTable of both table
        //have to compute 
        void resize ()
        {
            int previousSize = sizeOfTable;
            sizeOfTable = 2*sizeOfTable;
            vector<T> values1_old = values1;
            vector<T> values2_old = values2;
            values1.assign(sizeOfTable, NULL);
            values2.assign(sizeOfTable, NULL);
            
            for (typename std::vector<T>::iterator it = values1_old.begin() ; it != values1_old.end(); ++it)
            {
                if (*it != NULL){
                    //erase this and call add
                    T x = *it;
                    add(x);
                }
            }
            for (typename std::vector<T>::iterator it = values2_old.begin() ; it != values2_old.end(); ++it)
            {
                if (*it != NULL){
                    T x = *it;
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
            
            for (int i = 0; i < LIMIT; i++)
            {
                int hash1 = hash(x, HASH1); //suprisely, this is a prime number
                int hash2 = hash(x, HASH2); // another prime number
                int tableToInsert = hash1%2+1; //1 or 2, randomly select the first table to insert
                int DEBUG = 0;
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

        //populate integers from START to START+size-1
        void populate(int size)
        {
            for (int i = START; i < size+START; i++){
                add(i);
            }
        }

        void doWork(int begin, int work)
        {
            for (int i = 0; i < work; i++){
                //10% add
                if (i % 10 ==9)
                {
                    add(2*begin+i);
                }
                //10%remove
                //Unfair to this concurrent version,
                //Since thread are not balance loaded
                //first thread will remove succesfully whereas the rest can not.
                else if (i % 10 == 1)
                {
                    remove(begin+i);
                }
                //80% contains, always true in this serial one
                else
                {
                    int c = contains(begin+i);
                    //printf("\n%d",c);
                }
            }
        }


};

void doWork(HashTable<int> h, int begin, int work)
{
    for (int i = 0; i < work; i++){
        //10% add
        if (i % 10 ==9)
        {
            h.add(2*begin+i);
        }
        //10%remove
        //Unfair to this concurrent version,
        //Since thread are not balance loaded
        //first thread will remove succesfully whereas the rest can not.
        else if (i % 10 == 1)
        {
            h.remove(begin+i);
        }
        //80% contains, always true in this serial one
        else
        {
            int c = h.contains(begin+i);
            //printf("\n%d",c);
        }
    }
}

int main(int argc, char** argv) 
{
    //Expected final size is about populated + (WORKS -  populated)/10
    int WORKS = 11000;
    int POPULATE = 10000;
    int NUM_THREAD = 8;
    int STEP = WORKS/NUM_THREAD;
    HashTable <int> a = HashTable<int>(4000);
    printf("populating...");
    a.populate(POPULATE);
    printf("\npopulated");
    printf("\ninit size is: %d",a.size());
    printf("\ninit table size is: %d",a.sizeOfTable);

    printf("\nDoing Works");  
    auto begin = chrono::high_resolution_clock::now(); 

    //lmd for calling do works
    auto lmd = [&a](int begin, int work){
        for (int i = 0; i < work; i++){
            //10% add
            if (i % 10 ==9)
            {
                a.add(2*begin+i);
            }
            //10%remove
            //Unfair to this concurrent version,
            //Since thread are not balance loaded
            //first thread will remove succesfully whereas the rest can not.
            else if (i % 10 == 1)
            {
                a.remove(begin+i);
            }
            //80% contains, always true in this serial one
            else
            {
                int c = a.contains(begin+i);
                //printf("\n%d",c);
            }
        }
    };   
    vector<std::thread> threads;
    for (int i = 0; i < NUM_THREAD; i++){
            
			threads.push_back(std::thread (lmd, a.START+STEP*i, STEP));
	}
	for (auto &th : threads){
		th.join();
		
	}



    //doWork(a, a.START, WORKS);
    auto end = chrono::high_resolution_clock::now();
    cout << "TOTAL EXECUTION TIME = "<<std::chrono::duration_cast<std::chrono::microseconds>(end-begin).count()<<"\n";
    //check size and table size
    printf("\nfinal size is: %d",a.size());
    printf("\nfinal table size is: %d",a.sizeOfTable);

    //check contains 
    //Should print 1 
    printf("\n%d", a.contains(2*a.START+99));
    //Should print 0 
    printf("\n%d", a.contains(2*a.START+100));
    //Should print 0 
    printf("\n%d", a.contains(a.START+11));

}
