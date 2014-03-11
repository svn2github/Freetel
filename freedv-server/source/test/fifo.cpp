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
  ASSERT_EQ(100U, f->put_space());
  memset(f->put(100), 255, 100);
  ASSERT_EQ(0U, f->get_available());
  f->put_done(100);
  ASSERT_EQ(100U, f->get_available());
  ASSERT_EQ(0U, f->put_space());
  EXPECT_THROW(f->put(1), std::runtime_error);
  ASSERT_EQ(100U, f->get_available());
  ASSERT_NE((uint8_t *)0, f->get(100));
  f->get_done(100);
  ASSERT_EQ(0U, f->get_available());
  ASSERT_EQ(100U, f->put_space());
  ASSERT_EQ(100U, f->put_space());
  memset(f->put(100), 255, 100);
  ASSERT_EQ(0U, f->get_available());
  f->put_done(100);
  ASSERT_EQ(100U, f->get_available());
  ASSERT_EQ(0U, f->put_space());
}

TEST_F(FIFOTest, Reorder) {
  uint8_t * b;
  const uint8_t * r;

  ASSERT_EQ(100U, f->put_space());
  ASSERT_NE((uint8_t *)0, b = f->put(99));
  memset(b, 0, 98);
  b[98] = 'b';
  f->put_done(99);
  ASSERT_EQ(1U, f->put_space());
  ASSERT_EQ(99U, f->get_available());
  ASSERT_NE((uint8_t *)0, f->get(98));
  f->get_done(98);
  ASSERT_EQ(1U, f->get_available());
  ASSERT_EQ(99U, f->put_space());
  // This should cause reorder().
  ASSERT_NE((uint8_t *)0, b = f->put(2));
  f->put_done(0);
  ASSERT_EQ(99U, f->put_space());
  ASSERT_EQ(1U, f->get_available());
  ASSERT_NE((uint8_t *)0, r = f->get(1));
  ASSERT_EQ('b', *r);
  f->get_done(1);
}

TEST_F(FIFOTest, CanReset) {
  ASSERT_EQ(100U, f->put_space());
  ASSERT_EQ(0U, f->get_available());
  f->put_done(100);
  ASSERT_EQ(100U, f->get_available());
  ASSERT_EQ(0U, f->put_space());
  f->reset();
  ASSERT_EQ(0U, f->get_available());
  ASSERT_EQ(100U, f->put_space());
}
