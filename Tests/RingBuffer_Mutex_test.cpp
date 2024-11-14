#include "RingBuffer_Mutex.hpp"

#include <gtest/gtest.h>

#include <iostream>

TEST(RingBuffer, initialConditions) {
  RingBuffer<int_fast64_t> ring{1024};

  EXPECT_EQ(ring.capacity(), 1024);
  EXPECT_EQ(ring.size(), 0);
  EXPECT_TRUE(ring.empty());
  EXPECT_FALSE(ring.full());
}

TEST(RingBuffer, canPushAValue) {
  RingBuffer<int_fast64_t> ring{1024};

  EXPECT_TRUE(ring.push(10));
  EXPECT_FALSE(ring.empty());
  EXPECT_EQ(ring.size(), 1);
}

TEST(RingBuffer, canPopAValue) {
  RingBuffer<int_fast64_t> ring{1024};

  ring.push(10);

  int_fast64_t value;

  EXPECT_TRUE(ring.pop(value));
  EXPECT_TRUE(ring.empty());
  EXPECT_EQ(ring.size(), 0);
}

TEST(RingBuffer, cantPushWhenFull) {
  RingBuffer<int_fast64_t> ring{1};

  ring.push(10);
  EXPECT_FALSE(ring.push(11));

  int_fast64_t value;
  ring.pop(value);

  EXPECT_TRUE(ring.push(11));
}

TEST(RingBuffer, cantPopWhenEmpty) {
  RingBuffer<int_fast64_t> ring{1};
  int_fast64_t value;

  EXPECT_FALSE(ring.pop(value));
}
