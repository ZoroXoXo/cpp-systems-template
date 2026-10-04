#include <iostream>
#include <thread>

int main() {
    std::cout << "--- C++20 Systems Environment Active ---\n";
    std::cout << "Hardware Concurrency (Cores): " 
              << std::thread::hardware_concurrency() << "\n";
    return 0;
}
