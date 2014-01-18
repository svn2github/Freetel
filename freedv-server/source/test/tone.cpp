// Tests for the Tone class.
#include <drivers.h>
#include <gtest/gtest.h>
#include <sstream>

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

//	/// Destroy an Tone device instance.
//	virtual		~Tone() = 0;
//
//	/// Get the current audio level.
//	/// \return The current audio level.
//	/// The value is normalized to the range of 0.0 to 1.0.
//	virtual float	level() = 0;
//
//	/// Set the current audio level.
//	/// \param value The new value for the current audio level.
//	/// The value must be normalized within the range of 0.0 to 1.0.
//	virtual void	level(float value) = 0;
//
//        /// Read audio into an array of the signed 16-bit integer type.
//	virtual std::size_t
//			read16(int16_t * array, std::size_t length) = 0;

TEST_F(ToneTest, InitialLevelIs1) {
  EXPECT_EQ(1.0, i->level());
}

TEST_F(ToneTest, LevelChange) {
  i->level(0.0);
  EXPECT_EQ(0.0, i->level());
  i->level(0.5);
  EXPECT_EQ(0.5, i->level());
  i->level(1.0);
  EXPECT_EQ(1.0, i->level());
}

TEST_F(ToneTest, EnforcesNormalization) {
  EXPECT_THROW(i->level(-1e-7), std::runtime_error);
  EXPECT_THROW(i->level(1.0 + 1e-7), std::runtime_error);
  EXPECT_NO_THROW(i->level(0.0));
  EXPECT_NO_THROW(i->level(0.5));
  EXPECT_NO_THROW(i->level(1.0));
}
