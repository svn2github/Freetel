/// This is the main program for applications that are not space-limited.
/// Any application that is space limited should have its own main that
/// wires drivers to the Interfaces class without using DriverManager.
/// Thus, you can get rid of all of the STL template use, etc.
///
/// For the sake of correctness and optimization, I have written whatever I
/// can to be without side-effects, a style inherited from functional
/// programming. Thus, the excessive use of "const". - Bruce

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
  check_privileges();
  driver_manager()->print(cout);
}

static void help(const char * name)
{
  static const char message[] = 
    " [options]\n"
    "\nWhere options are these flags:\n\n"
    "--codec or -c\t\tSelect the voice codec.\n"
    "--config or -c\t\tPrint the configuration.\n"
    "--drivers or -d\t\tPrint a list of the available device drivers.\n"
    "--framer or -f\t\tSelect the communications protocol framer.\n"
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
  { "framer",		required_argument, 0, 'f' },
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

namespace FreeDV {
  const char * program_name = 0;
};

int
main(int argc, char * * argv)
{
  FreeDV::program_name = argv[0];
  Interfaces	i;
  const DriverManager * const m = driver_manager();

  i.fill_in();

  if ( argc > 1 ) {
    int command;
    while ((command = getopt_long(argc, argv, "c:Cdf:hi:k:l:m:M:p:P:r:t:x:", options, NULL)) != -1 ) {
      switch (command) {
      case 0:
        break;
      case 'd':
        drivers();
        exit(0);
      default:
      case 'h':
        help(argv[0]);
        exit(1);
      case 'C':
	// FIX: Operator overload doesn't work here.
        i.print(cout, argv[0]) << endl;
        exit(0);
      case 'c':
      case 'f':
      case 'i':
      case 'k':
      case 'l':
      case 'm':
      case 'M':
      case 'p':
      case 'P':
      case 'r':
      case 't':
      case 'x':
        const char * const	driver = optarg;
        char * const		colon = index(optarg, ':');
        const char *		parameter = "";
  
        if ( colon ) {
          *colon = 0;
          parameter = colon + 1;
        }
  
        Base * set = (Base *)1; 
  

        switch (command) {
        case 'c':
          set = i.codec = m->codec(driver, parameter);
          break;
        case 'f':
          set = i.framer = m->framer(driver, parameter);
          break;
        case 'i':
          set = i.user_interface = m->user_interface(driver, parameter, &i);
          break;
        case 'k':
          set = i.keying_output = m->keying_output(driver, parameter);
          break;
        case 'l':
          set = i.loudspeaker = m->audio_output(driver, parameter);
          break;
        case 'm':
          set = i.microphone = m->audio_input(driver, parameter);
          break;
        case 'M':
          set = i.modem = m->modem(driver, parameter);
          break;
        case 'p':
          set = i.ptt_input_digital = m->ptt_input(driver, parameter);
          break;
        case 'P':
          set = i.ptt_input_ssb = m->ptt_input(driver, parameter);
          break;
        case 'r':
          set = i.receiver = m->audio_input(driver, parameter);
          break;
        case 't':
          set = i.transmitter = m->audio_output(driver, parameter);
          break;
        case 'x':
          set = i.text_input = m->text_input(driver, parameter);
          break;
        }
        if ( set == 0 )
          exit(1);
      }
    }
  }
  else { // argc <= 1
    help(argv[0]);
    exit(1);
  }
  set_scheduler();
  check_privileges();
  return run(&i);
}
