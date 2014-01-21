// Tests for the TextInput class.
#include <drivers.h>
#include <gtest/gtest.h>
#include <climits>

using namespace FreeDV;

const char text[] =
"The time is out of joint. O cursed spite,"
"That ever I was born to set it right!";

class TextConstantTest : public ::testing::Test {
protected:
	TextInput *	i;

	TextConstantTest()
	: i(0)
	{
	}

  void	SetUp() {
    i = Driver::TextConstant(text);
  }

  void	TearDown() {
    delete i;
  }
};

TEST_F(TextConstantTest, ReadyAndRead) {
  char	buffer[1024];

  EXPECT_EQ(sizeof(text) - 1, i->ready());
  EXPECT_EQ(10, i->read(buffer, 10));
  EXPECT_EQ(sizeof(text) - 1 - 10, i->ready());
  EXPECT_EQ(0, i->read(buffer, 0));
  EXPECT_EQ(sizeof(text) - 1 - 10, i->ready());
  EXPECT_EQ(sizeof(text) - 1 - 10, i->read(buffer, sizeof(buffer)));
  EXPECT_EQ(0, i->ready());
  EXPECT_THROW(i->read(buffer, sizeof(buffer)), std::runtime_error);
}
