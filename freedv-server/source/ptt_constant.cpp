/// The constant PTT driver, for testing.

#include "drivers.h"
#include <iostream>
#include <stdexcept>

namespace FreeDV {
   /// PTT driver that is constant transmit or constant receive. For testing.
  class PTTConstant : public PTTInput {
  private:
    /// This is true if ready has not yet been sent.
    bool		ready_one_shot;
    bool		pressed;
  public:
    /// Instantiate push-to-talk source with constant input, for testing.
    			PTTConstant(const char * parameters);
    virtual		~PTTConstant();
    
    /// Return file descriptors for poll()
    /// \param size The address of a variable that will be written
    /// with the number of file descriptors in the array.
    /// \return The address of an array of integers containing the
    /// file descriptors.
    virtual int	poll_fds(PollType * array, int space);

    /// Return the amount of bytes ready for read.
    std::size_t	ready();

    /// Return true if the PTT input is pressed.
    bool	state();
  };

  PTTConstant::PTTConstant(const char * parameters)
  : PTTInput("constant", parameters), ready_one_shot(true)
  {
    if ( *parameters == 't' )
      pressed = true;
    else if ( *parameters == 'r' || *parameters == '\0' )
      pressed = false;
    else {
      std::cerr << "PTT input constant: bad parameter string "
       << parameters << std::endl;
      pressed = false;
    }
  }

  PTTConstant::~PTTConstant()
  {
  }

  int
  PTTConstant::poll_fds(PollType *, int)
  {
    return 0;
  }

  std::size_t
  PTTConstant::ready()
  {
    if ( ready_one_shot )
      return 1;
    else
      return 0;
  }

  bool
  PTTConstant::state()
  {
    if ( ready_one_shot )
      ready_one_shot = false;
    else {
      /// A real I/O source would block, so throw an error here.
      throw std::runtime_error("PTT constant state read with no I/O ready.");
    }

    return pressed;
  }

  PTTInput *
  Driver::PTTConstant(const char * parameter)
  {
    return new ::FreeDV::PTTConstant(parameter);
  }

  std::ostream &
  Enumerator::PTTConstant(std::ostream & stream)
  {
    return stream;
  }

  static bool
  initializer()
  {
    driver_manager()->register_ptt_input("constant", Driver::PTTConstant, Enumerator::PTTConstant);
    return true;
  }
  static const bool initialized = initializer();
}
