// Tests for the KeyingSink class.
#include <drivers.h>
#include <gtest/gtest.h>

using namespace FreeDV;

class KeyingSinkTest : public ::testing::Test {
protected:
	KeyingOutput *	k;

	KeyingSinkTest()
	: k(0)
	{
	}

  void	SetUp() {
    k = Driver::KeyingSink("");
  }

  void	TearDown() {
    delete k;
  }
};

TEST_F(KeyingSinkTest, CanCallKey) {
  k->key(true);
  k->key(false);
}

TEST_F(KeyingSinkTest, AlwaysReady) {
  EXPECT_EQ(SIZE_MAX, k->ready());
}
