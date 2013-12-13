#include "drivers.h"
#include <iostream>

// This is a test driver that provides tones.
namespace FreeDV {
  class BlankPanel : public UserInterface {
  public:
    			BlankPanel(const char * parameter);
    virtual		~BlankPanel();
    
  };

  BlankPanel::BlankPanel(const char * parameter)
  {
  }

  BlankPanel::~BlankPanel()
  {
  }

  static UserInterface *
  creator(const char * parameter)
  {
    return new BlankPanel(parameter);
  }

  static bool
  initializer()
  {
    init_driver_manager().register_user_interface("blank-panel", creator);
    return true;
  }
  static const bool initialized = initializer();
}
