/// The constant text driver, just outputs the same text over and over.

#include "drivers.h"
#include <stdexcept>
#include <cstring>

namespace FreeDV {
  /// This driver provides constant text.
  class TextConstant : public TextInput {
  private:
    const std::size_t	length;
    std::size_t		index;

  public:
    /// Instantiate the constant text driver.
    			TextConstant(const char * parameter);
    virtual		~TextConstant();
    
    /// Read the text data.
    std::size_t		read(char * buffer, std::size_t size);

    /// Return the amount of bytes ready for read.
    std::size_t		ready();
  };

  TextConstant::TextConstant(const char * parameters)
  : TextInput("constant", parameters), index(0), length(strlen(parameters))
  {
  }

  TextConstant::~TextConstant()
  {
  }

  std::size_t
  TextConstant::read(char * buffer, std::size_t size)
  {
    const std::size_t available = length - index;

    if ( available == 0 ) {
      /// A real I/O device would block if this happened, so throw an error.
      throw std::runtime_error("Text constant: read with no I/O ready.");
    }

    if ( size > available )
      size = available;

    if ( size > 0 ) {
      memcpy(buffer, &parameters[index], size);
      index += size;
    }
    return size;
  }

  std::size_t
  TextConstant::ready()
  {
    return length - index;
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
    driver_manager()->register_text_input("constant", Driver::TextConstant);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
