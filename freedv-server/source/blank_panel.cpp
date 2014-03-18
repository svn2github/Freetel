/// The "blank panel" GUI driver, for testing.

#include "drivers.h"

namespace FreeDV {
  /// This is control-less GUI driver, for testing.
  class BlankPanel : public UserInterface {
  public:

    /// Instantiate the blank panel GUI driver.
    			BlankPanel(const char * parameter, Interfaces * interfaces);
    virtual		~BlankPanel();
    
    /// Return file descriptors for poll()
    /// \param size The address of a variable that will be written
    /// with the number of file descriptors in the array.
    /// \return The address of an array of integers containing the
    /// file descriptors.
    virtual int	poll_fds(struct pollfd * array, int space);

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
  BlankPanel::poll_fds(struct pollfd *, int)
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

  std::ostream &
  Enumerator::BlankPanel(std::ostream & stream)
  {
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_user_interface("blank-panel", Driver::BlankPanel, Enumerator::BlankPanel);
    return true;
  }
  static const bool initialized = initializer();
}
