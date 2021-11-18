#include <atomic>
#include <iostream>       // std::cout
#include <thread>
void foo(std::atomic<int> x)
{
    __transaction_relaxed 
    {
        x = 1;
        while (x != 0) {}
    }
}
void bar(std::atomic<int> x)
{
    while (x  != 1) {}
    x = 0;
}
int main(int argc, char** argv) 
{
    std::atomic<int> x = 0;
    
    std::thread first (foo, x);     // spawn new thread that calls foo()
    std::thread second (bar,x);

    std::cout << "main, foo and bar now execute concurrently...\n";

        // synchronize threads:
    first.join();                // pauses until first finishes
    second.join();               // pauses until second finishes

    std::cout << "foo and bar completed.\n";

    return 0;
}