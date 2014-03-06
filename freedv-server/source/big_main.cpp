/// This is the main program for applications that are not space-limited.
/// Any application that is space limited should have its own main that
/// wires drivers to the Interfaces class without using DriverManager.
/// Thus, you can get rid of all of the STL template use, etc.
///
/// For the sake of correctness and optimization, I have written whatever I
/// can to be without side-effects, a style inherited from functional
/// programming. Thus, the excessive use of "const". - Bruce

#ifndef NO_INITIALIZERS

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <getopt.h>
#include <codec2.h>
#include <codec2_fdmdv.h>

#include "drivers.h"

using namespace std;
namespace FreeDV {
  /// Run the main loop of the program, this is called after arguments are set.
  extern int run(struct Interfaces *);
}
using namespace FreeDV;


static void drivers()
{
  driver_manager.print(cout);
}

static void help(const char * name)
{
  static const char message[] = 
    " [options]\n"
    "\nWhere options are these flags:\n\n"
    "--codec or -c\t\tSelect the voice codec.\n"
    "--drivers or -d\t\tPrint a list of the available device drivers.\n"
    "--gui or -g\t\tSelect the graphical user interface.\n"
    "--help or -h\t\tPrint this message.\n"
    "--interface or -i\tSelect the user-interface (graphical or otherwise).\n"
    "--keying or -k\t\tSelect the transmitter keying interface.\n"
    "--loudspeaker or -l\tSelect the operator audio output interface.\n"
    "--microphone or -m\tSelect the operator audio input interface.\n"
    "--modem or -M\t\tSelect RF modem.\n"
    "--ptt-digital or -p\tSelect the push-to-talk input interface for "
    "digital voice.\n"
    "--ptt-ssb or -P\t\tSelect the push-to-talk input interface for "
    "SSB.\n"
    "--receiver or -r\tSelect the interface for audio input from the "
    "receiver.\n"
    "--text or -x\t\tSelect the interface for text to be transmitted.\n"
    "--transmitter or -t\tSelect the interface for audio output to the "
    "transmitter.\n"
    "\nLong flags with parameters are in the form of --<flag>=<parameter>\n"
    "Short flags with parameters are in the form of -<letter> <parameter>\n"
    "\nFor example, both of these flags have the same effect:\n"
    "\t-M codec2:1600\n"
    "\t--modem=codec2:1600\n"
    "\nAll of the flags except for -h or --help must have a "
    "\"<driver>:<parameter>\"\n"
    "argument, where <driver> is the name of a device driver and <parameter> is\n"
    "specific to the driver.\n"
    "\nExample:\n"
    "\t --loudspeaker=alsa:default\n"
  ;
  cerr << "\nUsage: " << name << message << endl;
}

static const struct option options[] = {
  { "codec",		required_argument, 0, 'c' },
  { "config",		no_argument,	   0, 'C' },
  { "drivers",		no_argument,	   0, 'd' },
  { "default",		no_argument,	   0, 'D' },
  { "framer",		required_argument, 0, 'f' },
  { "gui",		required_argument, 0, 'g' },
  { "help",		no_argument,	   0, 'h' },
  { "interface",	required_argument, 0, 'i' },
  { "keying",		required_argument, 0, 'k' },
  { "loudspeaker",	required_argument, 0, 'l' },
  { "microphone",	required_argument, 0, 'm' },
  { "modem",		required_argument, 0, 'M' },
  { "ptt-digital",	required_argument, 0, 'p' },
  { "ptt-ssb",		required_argument, 0, 'P' },
  { "receiver",		required_argument, 0, 'r' },
  { "text",		required_argument, 0, 'x' },
  { "transmitter",	required_argument, 0, 't' },
  { 0, 0, 0, 0 }
};

int
main(int argc, char * * argv)
{
  int		command;
  Interfaces	i;
  const char *	driver;
  const char *	parameter;

  if ( argc > 1 ) {
    while ((command = getopt_long(argc, argv, "c:dhi:k:l:m:M:n:p:r:t:x:", options, NULL)) != -1) {
      switch (command) {
      case 'f':
      case 'g':
      case 'i':
      case 'k':
      case 'l':
      case 'm':
      case 'p':
      case 'P':
      case 'r':
      case 't':
      case 'x':
        char * const colon(index(optarg, ':'));

        if ( colon == 0 || colon[0] != ':' || colon[1] == 0 ) {
          cerr << argv[optind - 1] << ": Missing colon. Argument must be of the form \"<driver>:<parameter>\"" << endl;
          exit(1);
        }
	
        *colon = 0;
        driver = optarg;
        parameter = &colon[1];
      }

      switch (command) {
      case 'c':
        i.codec = driver_manager.codec(driver, parameter);
        break;
      case 'd':
        drivers();
        exit(0);
        break;
      case 'D':
        i.fill_in();
        break;
      case 'f':
        i.framer = driver_manager.framer(driver, parameter);
        break;
      case 'g':
        i.user_interface = driver_manager.user_interface(driver, parameter, &i);
        break;
      default:
      case 'h':
        help(argv[0]);
        exit(1);
        break;
      case 'k':
        i.keying_output = driver_manager.keying_output(driver, parameter);
        break;
      case 'l':
        i.loudspeaker = driver_manager.audio_output(driver, parameter);
        break;
      case 'm':
        i.microphone = driver_manager.audio_input(driver, parameter);
        break;
      case 'M':
        i.modem = driver_manager.modem(driver, parameter);
        break;
      case 'p':
        i.ptt_input_digital = driver_manager.ptt_input(driver, parameter);
        break;
      case 'P':
        i.ptt_input_ssb = driver_manager.ptt_input(driver, parameter);
        break;
      case 'r':
        i.receiver = driver_manager.audio_input(driver, parameter);
        break;
      case 't':
        i.transmitter = driver_manager.audio_output(driver, parameter);
        break;
      case 'x':
        i.text_input = driver_manager.text_input(driver, parameter);
        break;
      case 'C':
	i.fill_in();
	// FIX: Operator overload doesn't work here.
        i.print(cout) << endl;
        exit(0);
      case 0:
        break;
      }
    }
  }
  else { // argc <= 1
    help(argv[0]);
    exit(1);
  }
  set_scheduler();
  return run(&i);
}
#endif
