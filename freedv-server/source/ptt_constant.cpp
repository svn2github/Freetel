#include "drivers.h"

namespace FreeDV {
   /// PTT driver that is constant transmit or constant receive. For testing.
  class PTTConstant : public PTTInput {
  public:
    /// Instantiate push-to-talk source with constant input, for testing.
    			PTTConstant(const char * parameters);
    virtual		~PTTConstant();
    
  };

  PTTConstant::PTTConstant(const char * parameters)
  : PTTInput(parameters)
  {
  }

  PTTConstant::~PTTConstant()
  {
  }

#ifndef NO_INITIALIZERS
  static PTTInput *
  creator(const char * parameter)
  {
    return new PTTConstant(parameter);
  }

  static bool
  initializer()
  {
    init_driver_manager().register_ptt_input("constant", creator);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
