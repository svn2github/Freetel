#include "drivers.h"

namespace FreeDV {
  /// This driver provides constant text.
  class TextConstant : public TextInput {
  public:
    /// Instantiate the constant text driver.
    			TextConstant(const char * parameter);
    virtual		~TextConstant();
    
  };

  TextConstant::TextConstant(const char * parameters)
  : TextInput(parameters)
  {
  }

  TextConstant::~TextConstant()
  {
  }

#ifndef NO_INITIALIZERS
  static TextInput *
  creator(const char * parameter)
  {
    return new TextConstant(parameter);
  }

  static bool
  initializer()
  {
    init_driver_manager().register_text_input("constant", creator);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
