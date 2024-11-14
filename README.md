# Single Producer - Single Consumer FIFO

The SPSC (single-producer single-consumer) pattern is a common case in multithreading applications
where one thread pushes data (producer) and another thread reads it (consumer).

I've choose to use Ring/Circular buffer as is a common way to resolve by different companies,
it's not just fast but and easy to understand/implement, which is the perfect scenario for learning. 

There are three examples of Ring Buffers using Atomic, Mutex and the Boost library

### Benchmarks Results

* LockFree: `ops/sec=203.243M/s` (172.077M/s no threads pinned)
* Mutex: `ops/sec=15.961M/s` (12.1346M/s no threads pinned)  
* Boost/SPSC: `ops/sec=121.009M/s` (88.0499M/s no threads pinned)

## Setup

You must have cmake and some C++ compiler installed already

1) mkdir build
2) cd build; cmake ..
3) make
4) ./RingBuffer_***_bench (to run the benchmarks)

## Resources
- [CPP Reference: std::atomic](https://en.cppreference.com/w/cpp/atomic/atomic)
- [Single Producer Single Consumer Lock-free FIFO From the Ground Up - Charles Frasch - CppCon 2023
](https://www.youtube.com/watch?v=K3P_Lmq6pw0)
- [Tony Van Eerd: Introduction to Lock-free Programming](https://www.youtube.com/watch?v=RWCadBJ6wTk)
- [Tony Van Eerd: Lock-free by Example](https://www.youtube.com/watch?v=kH59-vLMNys)

## Contact

For any questions or suggestions, please reach out [Ariel Berardi](https://www.linkedin.com/in/aberardi95/).
