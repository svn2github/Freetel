// Tests for the Tone class.
#include <drivers.h>
#include <gtest/gtest.h>
#include <sstream>
#include <iostream>

using namespace FreeDV;

class ToneTest : public ::testing::Test {
protected:
	AudioInput *	i;

	ToneTest()
	: i(0)
	{
	}

  void	SetUp() {
    i = Driver::Tone("");
  }

  void	TearDown() {
    delete i;
  }
};

TEST_F(ToneTest, InitialLevelIs1) {
  EXPECT_EQ(1.0, i->amplitude());
}

TEST_F(ToneTest, LevelChange) {
  i->amplitude(0.0);
  EXPECT_EQ(0.0, i->amplitude());
  i->amplitude(0.5);
  EXPECT_EQ(0.5, i->amplitude());
  i->amplitude(1.0);
  EXPECT_EQ(1.0, i->amplitude());
}

TEST_F(ToneTest, EnforcesNormalization) {
  EXPECT_THROW(i->amplitude(-1e-7), std::runtime_error);
  EXPECT_THROW(i->amplitude(1.0 + 1e-7), std::runtime_error);
  EXPECT_NO_THROW(i->amplitude(0.0));
  EXPECT_NO_THROW(i->amplitude(0.5));
  EXPECT_NO_THROW(i->amplitude(1.0));
}

// Maximum positive value of a signed 16-bit integer.
const int16_t	MaxS16((1 << 15) - 1);
// Half of the maximum positive value of a signed 16-bit integer.
const int16_t	HalfS16(1 << 14);

class ToneTest2 : public ::testing::Test {
protected:
	AudioInput *	i;

	ToneTest2()
	: i(0)
	{
	}

  void	SetUp() {
    std::stringstream	stream;
  
    stream << SampleRate / 2 << ",1";
  
    i = Driver::Tone(stream.str().c_str());
  }

  void	TearDown() {
    delete i;
  }
};

TEST_F(ToneTest2, AtNyquistLimit) {
  int16_t	buffer[4];

  i->read16(buffer, sizeof(buffer) / sizeof(*buffer));

  EXPECT_EQ(MaxS16, buffer[0]);
  EXPECT_EQ(-MaxS16, buffer[1]);
  EXPECT_EQ(MaxS16, buffer[2]);
  EXPECT_EQ(-MaxS16, buffer[3]);
}

TEST_F(ToneTest2, MasterAmplitudeIsZero) {
  int16_t	buffer[4];

  i->amplitude(0);
  i->read16(buffer, sizeof(buffer) / sizeof(*buffer));

  EXPECT_EQ(0, buffer[0]);
  EXPECT_EQ(0, buffer[1]);
  EXPECT_EQ(0, buffer[2]);
  EXPECT_EQ(0, buffer[3]);
}

TEST_F(ToneTest2, MasterAmplitudeIsZeroPointFive) {
  int16_t	buffer[4];

  i->amplitude(0.5);
  i->read16(buffer, sizeof(buffer) / sizeof(*buffer));

  EXPECT_EQ(HalfS16, buffer[0]);
  EXPECT_EQ(-HalfS16, buffer[1]);
  EXPECT_EQ(HalfS16, buffer[2]);
  EXPECT_EQ(-HalfS16, buffer[3]);
}

TEST(ToneTest3, AtHalfNyquistLimit) {
  int16_t	buffer[8];

  std::stringstream	stream;

  stream << SampleRate / 4 << ",1";

  AudioInput *		i = Driver::Tone(stream.str().c_str());

  i->read16(buffer, sizeof(buffer) / sizeof(*buffer));

  EXPECT_EQ(MaxS16, buffer[0]);
  EXPECT_EQ(0, buffer[1]);
  EXPECT_EQ(-MaxS16, buffer[2]);
  EXPECT_EQ(0, buffer[3]);
  EXPECT_EQ(MaxS16, buffer[4]);
  EXPECT_EQ(0, buffer[5]);
  EXPECT_EQ(-MaxS16, buffer[6]);
  EXPECT_EQ(0, buffer[7]);

  delete i;
}

TEST(ToneTest3, FrequencyIsZero) {
  int16_t	buffer[4];

  AudioInput *		i = Driver::Tone("0,1");

  i->read16(buffer, sizeof(buffer) / sizeof(*buffer));

  EXPECT_EQ(0, buffer[0]);
  EXPECT_EQ(0, buffer[1]);
  EXPECT_EQ(0, buffer[2]);
  EXPECT_EQ(0, buffer[3]);

  delete i;
}

TEST(ToneTest3, ToneAmplitudeIsZero) {
  int16_t	buffer[4];

  AudioInput *		i = Driver::Tone("1000,0");

  i->read16(buffer, sizeof(buffer) / sizeof(*buffer));

  EXPECT_EQ(0, buffer[0]);
  EXPECT_EQ(0, buffer[1]);
  EXPECT_EQ(0, buffer[2]);
  EXPECT_EQ(0, buffer[3]);

  delete i;
}

TEST(ToneTest3, WavesSumCorrectly) {
  int16_t	buffer[8];

  std::stringstream	stream;

  stream << SampleRate / 2 << ",0.5:";
  stream << SampleRate / 4 << ",0.5";

  AudioInput *		i = Driver::Tone(stream.str().c_str());

  i->read16(buffer, sizeof(buffer) / sizeof(*buffer));

  EXPECT_EQ(MaxS16, buffer[0]);
  EXPECT_EQ(-HalfS16, buffer[1]);
  EXPECT_EQ(0, buffer[2]);
  EXPECT_EQ(-HalfS16, buffer[3]);
  EXPECT_EQ(MaxS16, buffer[4]);
  EXPECT_EQ(-HalfS16, buffer[5]);
  EXPECT_EQ(0, buffer[6]);
  EXPECT_EQ(-HalfS16, buffer[7]);

  delete i;
}

TEST(ToneTest3, SumOfAmplitudesIsNormalizedCorrectly) {
  int16_t	buffer[8];

  std::stringstream	stream;

  stream << SampleRate / 2 << ",1:";
  stream << SampleRate / 4 << ",1";

  AudioInput *		i = Driver::Tone(stream.str().c_str());

  i->read16(buffer, sizeof(buffer) / sizeof(*buffer));

  EXPECT_EQ(MaxS16, buffer[0]);
  EXPECT_EQ(-HalfS16, buffer[1]);
  EXPECT_EQ(0, buffer[2]);
  EXPECT_EQ(-HalfS16, buffer[3]);
  EXPECT_EQ(MaxS16, buffer[4]);
  EXPECT_EQ(-HalfS16, buffer[5]);
  EXPECT_EQ(0, buffer[6]);
  EXPECT_EQ(-HalfS16, buffer[7]);

  delete i;
}
