int main(int argc, char** argv) 
{
    atomic<int> x = 0;
    __transaction_relaxed {
    x = 1;
    while (x != 0) {}
    }
    while (x  != 1) {}
    x = 0;
}