
#include <iostream>
#include <stdio.h>
#include <random>
int main(int argc, char** argv){
    auto do_work = [&](){
        printf("aaa");
    };
    std::vector<std::thread> threads;
            for (int i = 0; i < 4; i++){
                threads.push_back(std::thread(do_work));
            }
            for (auto &th : threads){
                th.join();
                printf("\nThe balance after join is";
    }
}