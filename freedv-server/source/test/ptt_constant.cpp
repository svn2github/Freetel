// Tests for the PTTInput class.
#include <drivers.h>
#include <gtest/gtest.h>
#include <climits>

using namespace FreeDV;

const char text[] =
"The time is out of joint. O cursed spite,"
"That ever I was born to set it right!";

class PTTInputTransmitTest : public ::testing::Test {
protected:
	PTTInput *	i;

	PTTInputTransmitTest()
	: i(0)
	{
	}

  void	SetUp() {
    i = Driver::PTTConstant("t");
  }

  void	TearDown() {
    delete i;
  }
};

class PTTInputReceiveTest : public ::testing::Test {
protected:
	PTTInput *	i;

	PTTInputReceiveTest()
	: i(0)
	{
	}

  void	SetUp() {
    i = Driver::PTTConstant("r");
  }

  void	TearDown() {
    delete i;
  }
};

TEST_F(PTTInputTransmitTest, ReadyAndRead) {
  EXPECT_EQ(1, i->ready());
  EXPECT_EQ(1, i->ready());
  EXPECT_TRUE(i->state());
  EXPECT_EQ(0, i->ready());
  EXPECT_EQ(0, i->ready());
  EXPECT_THROW(i->state(), std::runtime_error);
}

TEST_F(PTTInputReceiveTest, ReadyAndRead) {
  EXPECT_EQ(1, i->ready());
  EXPECT_EQ(1, i->ready());
  EXPECT_FALSE(i->state());
  EXPECT_EQ(0, i->ready());
  EXPECT_EQ(0, i->ready());
  EXPECT_THROW(i->state(), std::runtime_error);
}
