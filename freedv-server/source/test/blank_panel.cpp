/// Tests for the BlankPanel class.
#include <drivers.h>
#include <gtest/gtest.h>
#include <climits>

using namespace FreeDV;

class BlankPanelTest : public ::testing::Test {
protected:
	UserInterface *	u;
        Interfaces *	i;

	BlankPanelTest()
	: u(0)
	{
	}

  void	SetUp() {
    i = new Interfaces();
    u = Driver::BlankPanel("", i);
  }

  void	TearDown() {
    delete u;
    delete i;
  }
};

TEST_F(BlankPanelTest, NeverReady) {
  EXPECT_EQ(0U, u->ready());
}
