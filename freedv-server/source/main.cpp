/*
 * For the sake of correctness and optimization, I have written whatever I can to be without
 * side-effects, a style inherited from functional programming. Thus, the excessive use of
 * "const". - Bruce
 */
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <getopt.h>
#include <codec2.h>
#include <codec2_fdmdv.h>

using namespace std;

static void drivers()
{
  cout << "To be implemented." << endl;
}

static void help(const char * name)
{
  static const char message[] = 
    " [options]\n"
    "\n\tWhere options are these flags:\n\n"
    "\t\t--drivers or -d\t\tPrint a list of the available device drivers.\n"
    "\t\t--help or -h\t\tPrint this message.\n"
    "\t\t--interface or -i\tSelect the user-interface (graphical or otherwise).\n"
    "\t\t--keying or -k\t\tSelect the transmitter keying interface.\n"
    "\t\t--loudspeaker or -l\tSelect the operator audio output interface.\n"
    "\t\t--microphone or -m\tSelect the operator audio input interface.\n"
    "\t\t--mode or -n\t\tSelect the codec mode and modulation.\n"
    "\t\t--ptt or -p\t\tSelect the push-to-talk input interface.\n"
    "\t\t--receiver or -r\tSelect the interface for audio input from the receiver.\n"
    "\t\t--text or -x\t\tSelect the interface for text to be transmitted.\n"
    "\t\t--transmitter or -t\tSelect the interface for audio output to the transmitter.\n"
    "\n\tLong flags with parameters are in the form of --<flag>=<parameter>\n"
    "\tShort flags with parameters are in the form of -<letter> <parameter>\n"
    "\n\tFor example, both of these flags have the same effect:\n"
    "\t\t-m 1600\n"
    "\t\t--mode=1600\n"
    "\n\tMode may be one of:\n"
    "\t\t--mode=1600\t\tNormal 1600 bit-per-second in 1.275 kHz RF bandwidth.\n"
    "\t\t--mode=1600-wide\t1600 bit-per-second in 2.125 kHz, wider guard-bands for improved\n"
    "\t\t\t\t\tDX performance.\n"
    "\n\tFlags used to select devices must have a \"<driver>:<parameter>\" argument\n"
    "\twhere <driver> is the name of a device driver for the selected input/output device,\n"
    "\tand <parameter> is the name or address of the selected device.\n"
    "\n\tExample:\n"
    "\t\t --loudspeaker=alsa:default\n"
  ;
  cerr << "\nUsage: " << name << message << endl;
}

struct parameters {
	const char * * interface;
	const char * * keying;
	const char * * loudspeaker;
	const char * * microphone;
	const char *   mode;
	const char * * ptt;
	const char * * receiver;
	const char * * text;
	const char * * transmitter;
};

static int run(struct parameters * p)
{
  return 0;
}

int
main(int argc, char * * argv)
{
  int command;
  struct parameters p = { 0,0,0,0,0,0,0,0,0 };

  static const struct option options[] = {
   { "drivers",		no_argument,	   0, 'd' },
   { "help",		no_argument,	   0, 'h' },
   { "interface",	required_argument, 0, 'i' },
   { "keying",		required_argument, 0, 'k' },
   { "loudspeaker",	required_argument, 0, 'l' },
   { "microphone",	required_argument, 0, 'm' },
   { "mode",		required_argument, 0, 'n' },
   { "ptt",		required_argument, 0, 'p' },
   { "receiver",	required_argument, 0, 'r' },
   { "text",		required_argument, 0, 'x' },
   { "transmitter",	required_argument, 0, 't' },
   { 0, 0, 0, 0 }
  };

  if ( argc > 1 ) {
    const char * * vector(new const char *[2]);

    while ((command = getopt_long(argc, argv, "dhi:k:l:m:n:p:r:t:x:", options, NULL)) != -1) {
      switch (command) {
      case 'i':
      case 'k':
      case 'l':
      case 'm':
      case 'p':
      case 'r':
      case 't':
      case 'x':
        char * const colon(index(optarg, ':'));

        if ( colon == 0 || colon[0] != ':' || colon[1] == 0 ) {
          cerr << argv[optind - 1] << ": Missing colon. Argument must be of the form \"<driver>:<parameter>\"" << endl;
          exit(1);
        }
	
        *colon = 0;
        vector[0] = optarg;
        vector[1] = &colon[1];
      }

      switch (command) {
      case 'd':
        drivers();
        exit(0);
        break;
      default:
      case 'h':
        help(argv[0]);
        exit(1);
        break;
      case 'i':
        p.interface = vector;
        break;
      case 'k':
        p.keying = vector;
        break;
      case 'l':
        p.loudspeaker = vector;
        break;
      case 'm':
        p.microphone = vector;
        break;
      case 'n':
        p.mode = optarg;
        break;
      case 'p':
        p.ptt = vector;
        break;
      case 'r':
        p.receiver = vector;
        break;
      case 't':
        p.transmitter = vector;
        break;
      case 'x':
        p.text = vector;
        break;
      case 0:
        break;
      }
    }
  }
  else { // argc <= 1
    help(argv[0]);
    exit(1);
  }
  run(&p);
  return 0;
}
