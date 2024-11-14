#pragma once

#include <cassert>
#include <iostream>
#include <mutex>

template <typename T>
class RingBuffer {
 public:
  explicit RingBuffer(size_t capacity) noexcept
      : m_capacity{capacity}, m_capacityMask{capacity - 1} {
    assert(capacity % 2 == 0);

    // Place allocate memory for T element
    m_buffer = new T[capacity * sizeof(T)];
  };

  ~RingBuffer() noexcept {
    while (!empty()) {
      m_buffer[m_popCursor & m_capacityMask].~T();
      ++m_popCursor;
    }

    delete[] m_buffer;
  };

  RingBuffer(const RingBuffer&) = delete;
  RingBuffer(RingBuffer&&) = delete;
  RingBuffer& operator=(const RingBuffer&) = delete;
  RingBuffer& operator=(RingBuffer&&) = delete;

  [[nodiscard]] constexpr size_t capacity() const noexcept {
    return m_capacity;
  }

  [[nodiscard]] constexpr size_t size() const noexcept {
    std::lock_guard<std::mutex> guard(m_mutex);

    assert(m_pushCursor >= m_popCursor);
    return m_pushCursor - m_popCursor;
  }

  [[nodiscard]] constexpr bool empty() const noexcept {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_pushCursor == m_popCursor;
  }

  [[nodiscard]] constexpr bool full() const noexcept {
    std::lock_guard<std::mutex> guard(m_mutex);
    return (m_pushCursor - m_popCursor) == m_capacity;
  }

  bool push(T value) noexcept {
    if (full()) {
      return false;
    }

    std::lock_guard<std::mutex> guard(m_mutex);

    // Construct T object in the allocated place provided in constructor
    new (&m_buffer[m_pushCursor & m_capacityMask]) T(value);

    ++m_pushCursor;
    return true;
  }

  bool pop(T& value) noexcept {
    if (empty()) {
      return false;
    }

    std::lock_guard<std::mutex> guard(m_mutex);

    value = m_buffer[m_popCursor & m_capacityMask];
    m_buffer[m_popCursor & m_capacityMask].~T();

    ++m_popCursor;
    return true;
  }

 private:
  T* m_buffer;

  size_t m_capacity;

  // Mask for capacity so I can use bitwise operation for index instead of %
  size_t m_capacityMask;

  size_t m_pushCursor{};
  size_t m_popCursor{};

  mutable std::mutex m_mutex;
};