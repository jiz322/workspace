
#include <math.h>
#include <algorithm>
#include <stdio.h>
#include <random>
#include <thread>
#include <mutex>
#include <shared_mutex>
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
        int NUM_LOCKS =10;
        vector<std::shared_timed_mutex*> mutexes1;
        vector<std::shared_timed_mutex*> mutexes2;
        std::shared_timed_mutex mtx1;
        std::shared_timed_mutex mtx2;
        std::shared_timed_mutex mtx3;
        std::shared_timed_mutex mtx4;
        std::shared_timed_mutex mtx5;
        std::shared_timed_mutex mtx6;
        std::shared_timed_mutex mtx7;
        std::shared_timed_mutex mtx8;
        std::shared_timed_mutex mtx9;
        std::shared_timed_mutex mtx10;
        std::shared_timed_mutex mtx11;
        std::shared_timed_mutex mtx12;
        std::shared_timed_mutex mtx13;
        std::shared_timed_mutex mtx14;
        std::shared_timed_mutex mtx15;
        std::shared_timed_mutex mtx16;
        std::shared_timed_mutex mtx17;
        std::shared_timed_mutex mtx18;
        std::shared_timed_mutex mtx19;
        std::shared_timed_mutex mtx20;
        std::shared_timed_mutex mtx_resize;
        HashTable (int sizeOfTable)
        {
            this->sizeOfTable = sizeOfTable;
            values1.assign(sizeOfTable, NULL);
            values2.assign(sizeOfTable, NULL);

            mutexes1.push_back( &mtx1);
            mutexes1.push_back( &mtx2);
            mutexes1.push_back( &mtx3);
            mutexes1.push_back( &mtx4);
            mutexes1.push_back( &mtx5);
            mutexes2.push_back( &mtx6);
            mutexes2.push_back( &mtx7);
            mutexes2.push_back( &mtx8);
            mutexes2.push_back( &mtx9);
            mutexes2.push_back( &mtx10);
            mutexes1.push_back( &mtx11);
            mutexes1.push_back( &mtx12);
            mutexes1.push_back( &mtx13);
            mutexes1.push_back( &mtx14);
            mutexes1.push_back( &mtx15);
            mutexes2.push_back( &mtx16);
            mutexes2.push_back( &mtx17);
            mutexes2.push_back( &mtx18);
            mutexes2.push_back( &mtx19);
            mutexes2.push_back( &mtx20);
            for (int i = 0; i < NUM_LOCKS; i++)
            {
                
                printf("\n%x", mutexes1.at(i));
                
            }
        }

        int hash (T x, int nm)
        {
            return (int)nm%x%sizeOfTable;
        }

        bool contains (T x)
        {
            int hash1 = hash(x, HASH1);
            int hash2 = hash(x, HASH2);
            //Might be too conservative
            int l1 = hash1 % NUM_LOCKS;
            int l2 = hash2 % NUM_LOCKS;
           // std::shared_lock lock1 (*(mutexes1.at(l1)), std::defer_lock);
           // std::shared_lock lock2 (*(mutexes2.at(l2)), std::defer_lock);
            std::shared_lock<std::shared_timed_mutex> lock (mtx_resize);
            std::shared_lock lock1 (*(mutexes1.at(l1)));
            std::shared_lock lock2 (*(mutexes2.at(l2)));
            
            // printf("\n trylock value : %d", lock1.try_lock());
            if (values1.at(hash1) == x)
            {
                return true;
            }
            // lock1.unlock();
            // while (!lock2.try_lock());
            if (values2.at(hash2) == x)
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
            std::unique_lock<std::shared_timed_mutex> lock (mtx_resize);
            int previousSize = sizeOfTable;
            sizeOfTable = sizeOfTable*5;
            vector<T> values1_old = values1;
            vector<T> values2_old = values2;
            values1.assign(sizeOfTable, NULL);
            values2.assign(sizeOfTable, NULL);
            
            for (typename std::vector<T>::iterator it = values1_old.begin() ; it != values1_old.end(); ++it)
            {
                if (*it != NULL){
                    //erase this and call add
                    T x = *it;
                    add_single_thread(x);
                }
            }
            for (typename std::vector<T>::iterator it = values2_old.begin() ; it != values2_old.end(); ++it)
            {
                if (*it != NULL){
                    T x = *it;
                    add_single_thread(x);
                }
            }
        }

        //compare to add, no acquire of mtx_resize
        bool add_single_thread (T x)
        {
            
            int LIMIT =  sizeOfTable/2;
            int tableToInsert = hash(x, HASH1) % 2;
            for (int i = 0; i < LIMIT; i++)
            {
                int hash1 = hash(x, HASH1); //a prime number
                int hash2 = hash(x, HASH2); // another prime number
                int l1 = hash1 % NUM_LOCKS;
                int l2 = hash2 % NUM_LOCKS;
                
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
        bool add (T x)
        {
            std::shared_lock<std::shared_timed_mutex> lock (mtx_resize);
            if (contains(x))
            {
                return false;
            }
            int LIMIT =  sizeOfTable/2;
            int tableToInsert = hash(x, HASH1) % 2; 
            for (int i = 0; i < LIMIT; i++)
            {
                //DEADLOCK HERE
                
                

                int hash1 = hash(x, HASH1); //a prime number
                int hash2 = hash(x, HASH2); // another prime number

                int l1 = x % NUM_LOCKS;
                int l2 = x % NUM_LOCKS; //NB: maybe only one mutex is necessary
                
                int DEBUG = 0;
                if (tableToInsert == 1)
                {
                    std::unique_lock<std::shared_timed_mutex> lock1 (*(mutexes1.at(l1)));
                    x = swap(x, hash1, 1);            
                    if (x == NULL)
                    {
                        return true;
                    }
                    lock1.unlock();
                    tableToInsert = 2;
                    
                }
                else //tableToInsert == 2
                {
                    std::unique_lock<std::shared_timed_mutex> lock2 (*(mutexes2.at(l2)));
                    x = swap(x, hash2, 2);
                    if (x == NULL)
                    {
                        return true;
                    }
                    lock2.unlock();
                    tableToInsert = 1;
                    
                }
                
            }
            lock.unlock();
            resize();
            return add(x);
        }

        bool remove(T x)
        {
            std::shared_lock<std::shared_timed_mutex> lock (mtx_resize);  // Should before compute of hash
            int hash1 = hash(x, HASH1); 
            int hash2 = hash(x, HASH2);
            int l1 = x % NUM_LOCKS; //NB: maybe only one mutex is necessary
            int l2 = x % NUM_LOCKS;
            
            
            std::unique_lock lock1 (*(mutexes1.at(l1)));
            if (values1.at(hash1) == x)
            {
                
                swap(NULL, hash1, 1);
                return true;
            }
            lock1.unlock();
            std::unique_lock lock2 (*(mutexes2.at(l2)));
            if(values2.at(hash2) == x)
            {
                swap(NULL, hash2, 2);
                return true;
            }
            
            else
            {
                lock2.unlock();
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


};


int main(int argc, char** argv) 
{
    //Expected final size is about populated + (WORKS -  populated)/10
    int WORKS = 2000000;
    int POPULATE = 10000000;
    int NUM_THREAD = 4;
    int STEP = WORKS/NUM_THREAD;
    HashTable <int> a = HashTable<int>(4000);
    printf("populating...");
    a.populate(POPULATE);
    printf("\npopulated");
    printf("\ninit size is: %d",a.size());
    printf("\ninit table size is: %d",a.sizeOfTable);

    printf("\nDoing Works");  
    

    //lmd for calling do works
    auto lmd = [&a](int begin, int work){
        for (int i = begin; i < begin + work; i++){
            //10% add
            if (i % 10 ==9)
            {
                a.add(2*a.START+i);
                //printf("\n add %d", 2*a.START+i);
            }
            //10%remove
            //Unfair to this concurrent version,
            //Since thread are not balance loaded
            //first thread will remove succesfully whereas the rest can not.
            else if (i % 10 == 1)
            {
                a.remove(i);
            }
            //80% contains, always true in this serial one
            else
            {
                int c = a.contains(i);
                //printf("\n%d",c);
            }
        }
    };   
    auto begin = chrono::high_resolution_clock::now(); 
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
    printf("\n%d", a.contains(3*a.START+99));
    //Should print 0 
    printf("\n%d", a.contains(2*a.START+100));
    //Should print 0 
    printf("\n%d", a.contains(a.START+11));

}
