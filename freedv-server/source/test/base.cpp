// Tests for the Base class.
#include <drivers.h>
#include <gtest/gtest.h>
#include <sstream>

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
  // The base class version of captive() and the one inherited by AudioSink,
  // must always return false. Only an interface that is owned by the
  // UserInterface class can return true.
  ASSERT_FALSE(b->captive());
}

TEST_F(BaseTest, PrintsItself) {
  std::stringstream stream;
  stream << *b;
  ASSERT_STREQ("sink:", stream.str().c_str());
}
