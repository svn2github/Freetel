/// The POSIX event handler, mainly for running without a GUI. 

#include "drivers.h"

namespace FreeDV {
  /// Event handler class for POSIX.
  class EventHandlerPOSIX : public EventHandler {
  protected:
	/// Run one iteration of the event handler.
	void		iterate();

  public:
	/// Create an event handler instance.
			EventHandlerPOSIX(const char * parameters);

	virtual		~EventHandlerPOSIX();

	/// Monitor a file descriptor in the event loop. Call a function if the
	/// file descriptor is ready for I/O.
	/// \param fd The file descriptor to monitor.
	/// \param type A bit-field of values defined in this class,
	///  indicating the kinds of events to listen for.
	/// \param private_data Private data to be passed to the event
	///  function.
	/// \param event A coroutine to call when there is a status change
	///  on the file descriptor. The arguments of the coroutine are
	///  - fd: The file descriptor that has an event.
	///  - type: A bit-field of FDStatus values indicating the events
	///    received.
	///  - private: The address of opaque data to be passed to the driver.
	virtual void	monitor(int fd, unsigned int type, void * private_data,
			 void (*event)(int fd, unsigned int type, void * private_data)
			 );

	/// Remove all monitoring of the given file descriptor by the event
	/// loop handler.
	/// \param fd The file descriptor to be removed from monitoring.
	virtual void	unmonitor(int fd);
  };

  EventHandlerPOSIX::EventHandlerPOSIX(const char * parameters)
  : EventHandler("posix", parameters)
  {
  }

  EventHandlerPOSIX::~EventHandlerPOSIX()
  {
  }

  void
  EventHandlerPOSIX::iterate()
  {
  }

  EventHandler *
  Driver::EventHandlerPOSIX(const char * parameter)
  {
    return new ::FreeDV::EventHandlerPOSIX(parameter);
  }

  void
  EventHandlerPOSIX::monitor(int fd, unsigned int type, void * private_data,
   void (*event)(int fd, unsigned int type, void * private_data))
  {
  }

  void
  EventHandlerPOSIX::unmonitor(int fd)
  {
  }

#ifndef NO_INITIALIZERS
  static bool
  initializer()
  {
    init_driver_manager().register_codec("no-op", Driver::CodecNoOp);
    return true;
  }
  static const bool initialized = initializer();
#endif
}
