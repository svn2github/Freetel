// Tests for the AudioSink class.
#include <drivers.h>
#include <gtest/gtest.h>

using namespace FreeDV;

class AudioSinkTest : public ::testing::Test {
protected:
	AudioOutput *	o;

	AudioSinkTest()
	: o(0)
	{
	}

  void	SetUp() {
    o = Driver::AudioSink("");
  }

  void	TearDown() {
    delete o;
  }
};

TEST_F(AudioSinkTest, WriteReturnsSize) {
  int16_t	buffer[8];

  size_t value = o->write16(buffer, sizeof(buffer) / sizeof(*buffer));

  EXPECT_EQ(sizeof(buffer) / sizeof(*buffer), value);
}
