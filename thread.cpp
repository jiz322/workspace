
#include <iostream>
#include <stdio.h>
#include <random>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char** argv){
    auto do_work = [&](){
        printf("aaa");
    };
    std::vector<std::cread> threads;
    for (int i = 0; i < 4; i++){
        threads.push_back(std::cread(do_work));
    }
    for (auto &th : threads){
        th.join();
        printf("\nThe balance after join is");
    }
}