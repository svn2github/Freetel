/// The "blank panel" GUI driver, for testing.

#include "drivers.h"

namespace FreeDV {
  /// This is control-less GUI driver, for testing.
  class BlankPanel : public UserInterface {
  public:

    /// Instantiate the blank panel GUI driver.
    			BlankPanel(const char * parameter, Interfaces * interfaces);
    virtual		~BlankPanel();
    
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

#ifndef NO_INITIALIZERS
  static bool
  initializer()
  {
    driver_manager()->register_user_interface("blank-panel", Driver::BlankPanel);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
