#include "drivers.h"
#include <iostream>

// This is a test driver that provides tones.
namespace FreeDV {
  class TextConstant : public TextInput {
  public:
    			TextConstant(const char * parameter);
    virtual		~TextConstant();
    
  };

  TextConstant::TextConstant(const char * parameter)
  {
  }

  TextConstant::~TextConstant()
  {
  }

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
}
