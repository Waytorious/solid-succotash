#include <iostream>
#include <thread>

using namespace std;

std::atomic<int> global{0};

int main(){
    std::thread a{b};
    std::thread c{b};

    std::cout << global << std::endl;

    a.join();
    c.join();

    std::cout << global << std::endl;

    std::cout << "Hello World!" << std::endl;
    return 0;
}

void b(){
    for (int i=0; i<1000000; i++){
        global++;
    }
    
}