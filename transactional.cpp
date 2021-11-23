//Error: use of deleted function
#include <math.h>
#include <algorithm>
#include <stdio.h>
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
#include <atomic>

using namespace std;

template <class T>
class ValueWrap
{
    public:
        T value;
        int valid =  0;
        ValueWrap (T v, int r)  
        {
            value = v;
            valid = r;
        } 

};
template <class T>
class HashTable
{

    public:
        std::vector<ValueWrap<T>> values1;
        std::vector<ValueWrap<T>> values2;
        int sizeOfTable;
        int HASH1 = 2147483647;
        int HASH2 = 479001599;
        int START = 100000000; //for populate table

        HashTable (int sizeOfTable)
        {
            this->sizeOfTable = sizeOfTable;
            values1.assign(sizeOfTable, ValueWrap(0,0));
            values2.assign(sizeOfTable, ValueWrap(0,0));
        }

        int hash (T x, int nm)
        {
            return (int)nm%x%sizeOfTable;
        }

        bool contains (T x)
        {
            int hash1 = hash(x, HASH1);
            int hash2 = hash(x, HASH2);
            if (values1.at(hash1).value == x)
            {
                return true;
            }
            else if (values2.at(hash2).value == x)
            {
                return true;
            }
            return false;
        }


        //swap value
        ValueWrap<T> swap(ValueWrap<T> x, int i, int table)
        {
            if (table == 1)
            {
                ValueWrap<T> ret = values1.at(i);
                values1.at(i) =  x;
                return  ret;
            }
            else
            {
                ValueWrap<T> ret = values2.at(i);
                values2.at(i) =  x;
                return  ret;
            }
        }

        //double the sizeOfTable of both table
        //have to compute 
        void resize ()
        {
            int previousSize = sizeOfTable;
            sizeOfTable = 5*sizeOfTable;
            vector<ValueWrap<T>> values1_old = values1;
            vector<ValueWrap<T>> values2_old = values2;
            values1.assign(sizeOfTable, NULL);
            values2.assign(sizeOfTable, NULL);
            
            for (typename std::vector<ValueWrap<T>>::iterator it = values1_old.begin() ; it != values1_old.end(); ++it)
            {
                if ((*it).valid == 1){
                    //erase this and call add
                    T x = (*it).value;
                    add(x);
                }
            }
            for (typename std::vector<ValueWrap<T>>::iterator it = values2_old.begin() ; it != values2_old.end(); ++it)
            {
                if ((*it).valid == 1){
                    T x = (*it).value;
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
            ValueWrap<T> tmp = ValueWrap(x, 1);
            int LIMIT =  sizeOfTable/2;
            int tableToInsert = hash(x, HASH2)%2; //1 or 2, randomly select the first table to insert
            for (int i = 0; i < LIMIT; i++)
            {
                int hash1 = hash(x, HASH1); //suprisely, this is a prime number
                int hash2 = hash(x, HASH2); // another prime number
                int DEBUG = 0;
                
                if (tableToInsert == 1)
                {
                    tmp = swap(tmp, hash1, 1);            
                    if (tmp.valid == 0)
                    {
                        return true;
                    }
                    tableToInsert = 2;
                }
                else //tableToInsert == 2
                {
                    tmp = swap(tmp, hash2, 2);
                    if (tmp.valid == 0)
                    {
                        return true;
                    }
                    tableToInsert = 1;
                }
            }
            resize();
            return add(tmp.value);
        }

        bool remove(T x)
        {
            int hash1 = hash(x, HASH1); 
            int hash2 = hash(x, HASH2);
            ValueWrap<T> tmp = ValueWrap(0, 0);
            if (values1.at(hash1).value == x)
            {
                
                swap(tmp, hash1, 1);
                return true;
            }
            else if(values2.at(hash2).value == x)
            {
                swap(tmp, hash2, 2);
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
            for (typename std::vector<ValueWrap<T>>::iterator it = values1.begin() ; it != values1.end(); ++it)
            {
                if ((*it).valid ==  1){
                    count ++;
                }
            }
            for (typename std::vector<ValueWrap<T>>::iterator it = values2.begin() ; it != values2.end(); ++it)
            {
                if ((*it).valid == 1){
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
    int WORKS = 20000;
    int POPULATE = 100;
    int NUM_THREAD = 8;
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
            if (i % 10 ==5)
            {
                
                a.add(2*a.START+i);
                
                //printf("\n add %d", 2*a.START+i);
            }
            //10%remove
            //Unfair to this concurrent version,
            //Since thread are not balance loaded
            //first thread will remove succesfully whereas the rest can not.
            else if (i % 10 == 0)
            {
                
                a.remove(2*a.START+i*2-5);
                
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
