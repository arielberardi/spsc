/*
First attemp would be using ring buffer with Mutex Locks
Second attemp woudl we will be using ring buffer with atomic
Third attemp would be using the lock free ring buffer from Boost library
Both need to have a different main and unite test for sanitazing
*/
#include <chrono>
#include <iostream>
#include <thread>

// #include "RingBuffer_Mutex.hpp"
#include "RingBuffer_Atomic.hpp"

int main() {
  constexpr const int kLimit = 10;
  RingBuffer<int_fast64_t> ring{1024};

  std::jthread producer([&]() {
    int_fast64_t value{0};

    while (value <= kLimit) {
      if (ring.push(value)) {
        std::cout << "Producer: " << value << std::endl;
        ++value;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
  });

  std::jthread consumer([&]() {
    int_fast64_t value{0};

    while (value < kLimit) {
      if (ring.pop(value)) {
        std::cout << "Consumer: " << value << std::endl;
      }

      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });
}
