#include "drivers.h"

// Codec "no-op", just copies its input to its output. For plain SSB voice, and for testing.
namespace FreeDV {
  class CodecNoOp : public Codec {
  public:

  		CodecNoOp(const char *);
		~CodecNoOp();

	void	key(bool value);

  };

  CodecNoOp::CodecNoOp(const char *)
  {
  }

  CodecNoOp::~CodecNoOp()
  {
  }

#ifndef NO_INITIALIZERS
  static Codec *
  creator(const char * parameter)
  {
    return new CodecNoOp(parameter);
  }

  static bool
  initializer()
  {
    init_driver_manager().register_codec("no-op", creator);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
