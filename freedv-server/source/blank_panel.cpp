#include "drivers.h"

namespace FreeDV {
  class BlankPanel : public UserInterface {
  public:
    			BlankPanel(const char * parameter, Interfaces * interfaces);
    virtual		~BlankPanel();
    
  };

  BlankPanel::BlankPanel(const char * parameter, Interfaces * interfaces)
  : UserInterface(parameter, interfaces)
  {
  }

  BlankPanel::~BlankPanel()
  {
  }

#ifndef NO_INITIALIZERS
  static UserInterface *
  creator(const char * parameter, Interfaces * interfaces)
  {
    return new BlankPanel(parameter, interfaces);
  }

  static bool
  initializer()
  {
    init_driver_manager().register_user_interface("blank-panel", creator);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
