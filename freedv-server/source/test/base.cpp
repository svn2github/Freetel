// Tests for the Base class.
#include <drivers.h>
#include <gtest/gtest.h>

using namespace FreeDV;

class BaseTest : public ::testing::Test {
protected:
	Base *	b;

	BaseTest()
	: b(0)
	{
	}

  void	SetUp() {
    // Base is a virtual base class, so we use one of its descendant classes.
    b = Driver::AudioSink("");
  }

  void	TearDown() {
    delete b;
  }
};

TEST_F(BaseTest, CaptiveIsFalse) {
  ASSERT_FALSE(b->captive());
}
