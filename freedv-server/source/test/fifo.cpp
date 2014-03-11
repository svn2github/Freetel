// Tests for the Base class.
#include <drivers.h>
#include <gtest/gtest.h>
#include <stdexcept>

using namespace FreeDV;

class FIFOTest : public ::testing::Test {
protected:
	FIFO *	f;

	FIFOTest()
	: f(0)
	{
	}

  void	SetUp() {
    f = new FIFO(100);
  }

  void	TearDown() {
    delete f;
  }
};

TEST_F(FIFOTest, CanFillAndDrain) {
  ASSERT_EQ(100U, f->incoming_available());
  memset(f->incoming_buffer(100), 255, 100);
  ASSERT_EQ(0U, f->outgoing_available());
  f->incoming_done(100);
  ASSERT_EQ(100U, f->outgoing_available());
  ASSERT_EQ(0U, f->incoming_available());
  EXPECT_THROW(f->incoming_buffer(1), std::runtime_error);
  ASSERT_EQ(100U, f->outgoing_available());
  ASSERT_NE((uint8_t *)0, f->outgoing_buffer(100));
  f->outgoing_done(100);
  ASSERT_EQ(0U, f->outgoing_available());
  ASSERT_EQ(100U, f->incoming_available());
  ASSERT_EQ(100U, f->incoming_available());
  memset(f->incoming_buffer(100), 255, 100);
  ASSERT_EQ(0U, f->outgoing_available());
  f->incoming_done(100);
  ASSERT_EQ(100U, f->outgoing_available());
  ASSERT_EQ(0U, f->incoming_available());
}

TEST_F(FIFOTest, Reorder) {
  uint8_t * b;
  const uint8_t * r;

  ASSERT_EQ(100U, f->incoming_available());
  ASSERT_NE((uint8_t *)0, b = f->incoming_buffer(99));
  memset(b, 0, 98);
  b[98] = 'b';
  f->incoming_done(99);
  ASSERT_EQ(1U, f->incoming_available());
  ASSERT_EQ(99U, f->outgoing_available());
  ASSERT_NE((uint8_t *)0, f->outgoing_buffer(98));
  f->outgoing_done(98);
  ASSERT_EQ(1U, f->outgoing_available());
  ASSERT_EQ(99U, f->incoming_available());
  // This should cause reorder().
  ASSERT_NE((uint8_t *)0, b = f->incoming_buffer(2));
  f->incoming_done(0);
  ASSERT_EQ(99U, f->incoming_available());
  ASSERT_EQ(1U, f->outgoing_available());
  ASSERT_NE((uint8_t *)0, r = f->outgoing_buffer(1));
  ASSERT_EQ('b', *r);
  f->outgoing_done(1);
}

TEST_F(FIFOTest, CanReset) {
  ASSERT_EQ(100U, f->incoming_available());
  ASSERT_EQ(0U, f->outgoing_available());
  f->incoming_done(100);
  ASSERT_EQ(100U, f->outgoing_available());
  ASSERT_EQ(0U, f->incoming_available());
  f->reset();
  ASSERT_EQ(0U, f->outgoing_available());
  ASSERT_EQ(100U, f->incoming_available());
}
