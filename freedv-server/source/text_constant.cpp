/// The constant text driver, just outputs the same text over and over.

#include "drivers.h"

namespace FreeDV {
  /// This driver provides constant text.
  class TextConstant : public TextInput {
  public:
    /// Instantiate the constant text driver.
    			TextConstant(const char * parameter);
    virtual		~TextConstant();
    
    /// Return the amount of bytes ready for read. In this case, it always
    /// returns SIZE_MAX.
    size_t	ready();
  };

  TextConstant::TextConstant(const char * parameters)
  : TextInput("constant", parameters)
  {
  }

  TextConstant::~TextConstant()
  {
  }

  size_t
  TextConstant::ready()
  {
    return SIZE_MAX;
  }

  TextInput *
  Driver::TextConstant(const char * parameter)
  {
    return new ::FreeDV::TextConstant(parameter);
  }

#ifndef NO_INITIALIZERS
  static bool
  initializer()
  {
    init_driver_manager().register_text_input("constant", Driver::TextConstant);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
