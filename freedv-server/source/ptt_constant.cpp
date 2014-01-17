/// The constant PTT driver, for testing.

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
  : PTTInput("constant", parameters)
  {
  }

  PTTConstant::~PTTConstant()
  {
  }

  PTTInput *
  Driver::PTTConstant(const char * parameter)
  {
    return new ::FreeDV::PTTConstant(parameter);
  }

#ifndef NO_INITIALIZERS
  static bool
  initializer()
  {
    init_driver_manager().register_ptt_input("constant", Driver::PTTConstant);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
