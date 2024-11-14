#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

#include <iostream>
#include <thread>

#include "RingBuffer_Atomic.hpp"

#define PIN_THREADS

#ifdef PIN_THREADS
static void pinThread(int cpu) {
  if (cpu < 0) {
    return;
  }

  ::cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(cpu, &cpuset);

  if (::pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) ==
      -1) {
    std::perror("pthread_setaffinity_rp");
    std::exit(EXIT_FAILURE);
  }
}
#endif

static void BM_RingBufferPushPop(benchmark::State& state) {
  RingBuffer<int_fast64_t> ring{1024};

  auto consumer = std::jthread([&]() {
#ifdef PIN_THREADS
    pinThread(2);
#endif
    int_fast64_t value{};

    while (value != -1) {
      while (!ring.pop(value)) {
      }
    }
  });

#ifdef PIN_THREADS
  pinThread(1);
#endif

  int_fast64_t count{};
  for (auto _ : state) {
    while (!ring.push(count++)) {
    }
  }

  state.counters["ops/sec"] = benchmark::Counter(static_cast<double>(count),
                                                 benchmark::Counter::kIsRate);

  ring.push(-1);
}

BENCHMARK(BM_RingBufferPushPop)->Unit(benchmark::kSecond)->Repetitions(10);
BENCHMARK_MAIN();