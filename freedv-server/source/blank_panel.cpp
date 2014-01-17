/// The "blank panel" GUI driver, for testing.

#include "drivers.h"

namespace FreeDV {
  /// This is control-less GUI driver, for testing.
  class BlankPanel : public UserInterface {
  public:

    /// Instantiate the blank panel GUI driver.
    			BlankPanel(const char * parameter, Interfaces * interfaces);
    virtual		~BlankPanel();
    
  };

  BlankPanel::BlankPanel(const char * parameter, Interfaces * interfaces)
  : UserInterface("blank-panel", parameter, interfaces)
  {
  }

  BlankPanel::~BlankPanel()
  {
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
    init_driver_manager().register_user_interface("blank-panel", Driver::BlankPanel);
    return true;
  }
  static const bool initialized = initializer();
#endif
}