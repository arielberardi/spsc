#pragma once

#include <atomic>
#include <cassert>
#include <iostream>

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
    size_t push = m_pushCursor.load(std::memory_order_acquire);
    size_t pop = m_popCursor.load(std::memory_order_acquire);

    assert(push >= pop);
    return push - pop;
  }

  [[nodiscard]] constexpr bool empty() const noexcept { return size() == 0; }
  [[nodiscard]] constexpr bool full() const noexcept {
    return size() == capacity();
  }

  bool push(T value) noexcept {
    size_t push = m_pushCursor.load(std::memory_order_relaxed);

    if (m_full(m_popCursorCached, push)) {
      m_popCursorCached = m_popCursor.load(std::memory_order_acquire);
      if (m_full(m_popCursorCached, push)) {
        return false;
      }
    }

    // Construct T object in the allocated place provided in constructor
    new (&m_buffer[push & m_capacityMask]) T(value);

    m_pushCursor.store(push + 1, std::memory_order_release);
    return true;
  }

  bool pop(T& value) noexcept {
    size_t pop = m_popCursor.load(std::memory_order_relaxed);

    if (m_empty(pop, m_pushCursorCached)) {
      m_pushCursorCached = m_pushCursor.load(std::memory_order_acquire);
      if (m_empty(pop, m_pushCursorCached)) {
        return false;
      }
    }

    value = m_buffer[pop & m_capacityMask];
    m_buffer[pop & m_capacityMask].~T();

    m_popCursor.store(pop + 1, std::memory_order_release);
    return true;
  }

 private:
  constexpr bool m_empty(size_t pop, size_t push) const noexcept {
    return pop == push;
  };

  constexpr bool m_full(size_t pop, size_t push) const noexcept {
    return (push - pop) == m_capacity;
  }

  T* m_buffer;
  size_t m_capacity;
  size_t m_capacityMask;

  static_assert(std::atomic<size_t>::is_always_lock_free);

  static constexpr auto kHardwareSize = 32;
  alignas(kHardwareSize) std::atomic<size_t> m_pushCursor{};
  alignas(kHardwareSize) std::atomic<size_t> m_popCursor{};

  size_t m_pushCursorCached{};
  size_t m_popCursorCached{};

  char m_padding[kHardwareSize - sizeof(size_t)];
};