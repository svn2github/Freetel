/// \file blank_panel.cpp
/// The "blank panel" GUI driver, for testing.
///
/// \copyright Copyright (C) 2013-2014 Algoram. See the LICENSE file.
///

#include "drivers.h"

namespace FreeDV {
  /// This is a control-less GUI driver, for testing.
  ///
  class BlankPanel : public UserInterface {
  public:

    /// Instantiate the blank panel GUI driver.
    			BlankPanel(const char * parameter, Interfaces * interfaces);
    virtual		~BlankPanel();
    
    /// Return file descriptors for poll()
    /// \param array The address of an array that will be written
    /// with a sequence of file descriptors.
    /// \param space The maximum number of file descriptors that may be
    /// stored in the array.
    /// \return The number of file descriptors written to the array.
    virtual int	poll_fds(PollType * array, int space);

    /// Return the amount of bytes ready for read. In this case, it always
    /// returns 0.
    std::size_t	ready();
  };

  BlankPanel::BlankPanel(const char * parameter, Interfaces * interfaces)
  : UserInterface("blank-panel", parameter, interfaces)
  {
  }

  BlankPanel::~BlankPanel()
  {
  }

  int
  BlankPanel::poll_fds(PollType *, int)
  {
    return 0;
  }

  std::size_t
  BlankPanel::ready()
  {
    return 0;
  }

  UserInterface *
  Driver::BlankPanel(const char * parameter, Interfaces * interfaces)
  {
    return new ::FreeDV::BlankPanel(parameter, interfaces);
  }

  static std::ostream &
  BlankPanelEnumerator(std::ostream & stream)
  {
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_user_interface(
     "blank-panel",
     Driver::BlankPanel,
     BlankPanelEnumerator);
    return true;
  }
  static const bool UNUSED initialized = initializer();
}
