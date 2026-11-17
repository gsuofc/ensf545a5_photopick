#include <chrono>
#include <iostream>
using namespace std;
//we are putting the FPS counting here so its seperate
std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

double timeSinceLastCall() {
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start); //we want us
    int64_t microseconds_count = duration.count();
    return microseconds_count/1000000.0;
}

void resetTime() {
    start = std::chrono::steady_clock::now();
}