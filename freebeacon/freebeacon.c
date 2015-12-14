/* 
  freebeacon.c
  David Rowe 
  Created Dec 2015

  FreeDV 1600 beacon.  Listens for FreeDV signals, then transmits a
  reply. Places the received signal files on a web server. Requires a
  Linux machine with a sound card and RS232-PTT interface to your radio.
  Just one sound card is required.

  When a "trigger" string is detected in the rx FreeDV text message
  (e.g. "hello beacon", or the beacon callsign), the beacon will
  transmit a signal report back to you.

  It requires a "txfilename" wave file to transmit, e.g. some one
  saying "Hi, I am a FreeDV beacon blah blah".  The signal report is
  encoded into the transmit text message.  Make the wave file long
  enough so that the the signal report is repeated a few times, say 30
  seconds. Transmit will stop when the wave file is played once.

  Freebeacon saves the received audio from the radio AND the decoded
  audio as wavefiles.  The file length is limited to 60 seconds. If
  you run freebeacon in a webserver directory these will appear on the
  Web.  Add a cron job to your machine to clean these files up once a
  day.

  If your input audio device is stereo note we only listen to
  the left channel.  RTS and DTR is raised on transmit, and lowered
  otherwise.

  A whole lot of code was lifted from freedv-dev for this program.

  TODO:

  [X] 48 to 8 kHz sample rate conversion
  [X] Port Audio list devices
  [X] command line processing framework
  [X] beacon state machine
  [X] install codec2
  [X] attempt debug sound dongle
      [X] modify for half duplex
      [X] sample rate option
      [X] prog sound dongle debug
  [X] RS232 PTT code
  [X] writing to wave files
  [X] test mode to tx straight away then end, to check levels, debug RS232
  [ ] FreeDV 700 support
  [ ] daemonise
      + change all fprintfs to use log file in daemon mode
  [ ] test on laptop
  [ ] test on RPi
  [ ] writing text string to a web page (cat, create if doesn't exist)
  [ ] samples from stdin option to work from sdr
  [ ] monitor rx and tx audio on another sound device
  [ ] option to not tx, just log info, for rx only stations
  [ ] Hamlib support for keying different radios
  [ ] basic SM1000 version
      + add mode, state machine
      + has audio interfaces, PTT, so neat solution

  Building:
    Download and "make install && ldconfig" codec2.  On my Ubuntu 14 and RPi I had to add an extra search
    path to the ld.conf.d directory to match the path the codec2 .so was installed in.

    apt-get install libsamplerate0-dev portaudio19-dev libportaudio-dev libsndfile1-dev
    gcc -I/usr/local/include/codec2 freebeacon.c -o freebeacon -lsamplerate -lportaudio -lsndfile -lcodec2

    Plug in your USB sound card and USB RS232 devices.
    Use alsamixer to adjust levels on your sound card. 

  Usage:
    ./freebeacon -h

  List sound devices
    ./freebeacon -l

  Example usage:
    ./freebeacon -c /dev/ttyUSB1 --txfilename ~/codec2-dev/wav/vk5qi.wav --dev 4 -v --trigger hello

  Testing sound cards on RPi:

  $ arecord -l

   **** List of CAPTURE Hardware Devices ****
   card 1: Audio [RIGblaster Advantage Audio], device 0: USB Audio [USB Audio]
     Subdevices: 1/1
     Subdevice #0: subdevice #0

  $ arecord -D hw:1,0 -f S16_LE -r 48000 test.wav
  $ aplay test.wav

*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <ctype.h>

#include <samplerate.h>
#include <getopt.h>

#include "sndfile.h"
#include "portaudio.h"

#include "codec2_fifo.h"
#include "modem_stats.h"
#include "freedv_api.h"

#define MAX_CHAR            80
#define FS8                 8000                // codec audio sample rate fixed at 8 kHz
#define FS48                48000               // 48 kHz sampling rate rec. as we can trust accuracy of sound card
#define SYNC_FRAMES         50                  // frames of valid rx sync we need to see to change state
#define UNSYNC_FRAMES       25                  // frames of lost sync we need to see to change state
#define PEAK_COUNTER        10                  // how often to report peak input level 
#define COM_HANDLE_INVALID  -1
#define LOG_COUNTER         50

/* globals used to communicate with async events and callback functions */

volatile int keepRunning;
char txtMsg[MAX_CHAR], *ptxtMsg, triggerString[MAX_CHAR];
int triggered;
float snr_est, snr_sample;
int com_handle, verbose;

/* state machine defines */

#define SRX_IDLE          0      /* listening but no FreeDV signal                                   */
#define SRX_MAYBE_SYNC    1      /* We have sync but lets see if it goes away                        */
#define SRX_SYNC          2      /* We have sync on a valid FreeDV signal                            */
#define SRX_MAYBE_UNSYNC  3      /* We have lost sync but lets see if it's really gone               */
#define STX               4      /* transmitting reply                                               */

char *state_str[] = {
    "Rx Idle",
    "Rx Maybe Sync",
    "Rx Sync",
    "Rx Maybe UnSync",
    "Tx"
};


int openComPort(const char *name);
void closeComPort(void);
void raiseDTR(void);
void lowerDTR(void);
void raiseRTS(void);
void lowerRTS(void);


/*--------------------------------------------------------------------------------------------------------*\

                                                  FUNCTIONS

\*--------------------------------------------------------------------------------------------------------*/

/* Called on Ctrl-C */

void intHandler(int dummy) {
    keepRunning = 0;
    fprintf(stderr,"\nShutting Down ......\n");
}

/* returns number of output samples generated by resampling */

int resample(SRC_STATE *src,
            short      output_short[],
            short      input_short[],
            int        output_sample_rate,
            int        input_sample_rate,
            int        length_output_short, // maximum output array length in samples
            int        length_input_short
            )
{
    SRC_DATA src_data;
    float    input[length_input_short];
    float    output[length_output_short];
    int      ret;

    assert(src != NULL);

    src_short_to_float_array(input_short, input, length_input_short);

    src_data.data_in = input;
    src_data.data_out = output;
    src_data.input_frames = length_input_short;
    src_data.output_frames = length_output_short;
    src_data.end_of_input = 0;
    src_data.src_ratio = (float)output_sample_rate/input_sample_rate;
    //printf("%d %d src_ratio: %f \n", length_input_short, length_output_short, src_data.src_ratio);

    ret = src_process(src, &src_data);
    assert(ret == 0);

    assert(src_data.output_frames_gen <= length_output_short);
    src_float_to_short_array(output, output_short, src_data.output_frames_gen);

    return src_data.output_frames_gen;
}


void listAudioDevices(void) {
    const PaDeviceInfo *deviceInfo = NULL;
    int                 numDevices, devn;

    numDevices = Pa_GetDeviceCount();
    printf("Num                                               Name      API   InCh  OutCh  DefFs\n");
    printf("====================================================================================\n");
    for (devn = 0; devn<numDevices; devn++) {
        deviceInfo = Pa_GetDeviceInfo(devn);
        if (deviceInfo == NULL) {
            fprintf(stderr, "Couldn't open devNum: %d\n", devn);
            return;
        }
        printf(" %2d %50s %8s %6d %6d %6d\n", 
               devn, 
               deviceInfo->name,
               Pa_GetHostApiInfo(deviceInfo->hostApi)->name,
               deviceInfo->maxInputChannels,
               deviceInfo->maxOutputChannels,
               (int)deviceInfo->defaultSampleRate);
    }
}


void printHelp(const struct option* long_options, int num_opts, char* argv[])
{
	int i;
	char *option_parameters;

	fprintf(stderr, "\nFreeBeacon - FreeDV Beacon\n"
		"usage: %s [OPTIONS]\n\n"
                "Options:\n"
                "\t-l --list (audio devices)\n"
                "\t-c        (comm port for Tx PTT)\n"
                "\t-t        (tx on start up, useful for testing)\n"
                "\t-v        (verbose)\n", argv[0]);
        for(i=0; i<num_opts-1; i++) {
		if(long_options[i].has_arg == no_argument) {
			option_parameters="";
		} else if (strcmp("dev", long_options[i].name) == 0) {
			option_parameters = " DeviceNumber (-l --list to list devices)";
                } else if (strcmp("trigger", long_options[i].name) == 0) {
			option_parameters = " textString (used to trigger beacon)";
                } else if (strcmp("callsign", long_options[i].name) == 0) {
			option_parameters = " callsign (returned in text str to tx)";
                } else if (strcmp("txfilename", long_options[i].name) == 0) {
			option_parameters = " wavefile (to use for source audio on tramsmit)";
                } else if (strcmp("samplerate", long_options[i].name) == 0) {
			option_parameters = " sampleRateHz (audio device sample rate)";
                }
		fprintf(stderr, "\t--%s%s\n", long_options[i].name, option_parameters);
	}

	exit(1);
}


/* text message callbacks */

void callbackNextRxChar(void *callback_state, char c) {

    /* if we hit end of buffer wrap around to start */

    if ((ptxtMsg - txtMsg) < (MAX_CHAR-1))
        *ptxtMsg++ = c;
    else
        ptxtMsg = txtMsg;

    /* if end of string let see if we have a match for the trigger
       string.  Note tx may send trigger string many times.  We only
       need to receive it once to trigger a beacon tx cycle. */

    if (c == 13) {
        *ptxtMsg++ = c;
        *ptxtMsg = 0;
         ptxtMsg = txtMsg;
         if (verbose)
             fprintf(stderr, "  RX txtMsg: %s\n", txtMsg);
         if (strstr(txtMsg, triggerString) != NULL) {
             triggered = 1;
             snr_sample = snr_est;
             if (verbose)
                 fprintf(stderr, "  Tx triggered!\n");
         }
    } 
}

char callbackNextTxChar(void *callback_state) {
    if ((*ptxtMsg == 0) || ((ptxtMsg - txtMsg) >= MAX_CHAR))
        ptxtMsg = txtMsg;

    //fprintf(stderr, "TX txtMsg: %d %c\n", (int)*ptxtMsg, *ptxtMsg);
    return *ptxtMsg++;
}


SNDFILE *openPlayFile(char fileName[], int *sfFs)
{
    SF_INFO  sfInfo;
    SNDFILE *sfPlayFile;

    sfInfo.format = 0;

    sfPlayFile = sf_open(fileName, SFM_READ, &sfInfo);
    if(sfPlayFile == NULL) {
        const char *strErr = sf_strerror(NULL);
        fprintf(stderr, " %s Couldn't open: %s\n", strErr, fileName);
    }
    *sfFs = sfInfo.samplerate;

    return sfPlayFile;
}

  
SNDFILE *openRecFile(char fileName[], int sfFs)
{
    SF_INFO  sfInfo;
    SNDFILE *sfRecFile;

    sfInfo.format     = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    sfInfo.channels   = 1;
    sfInfo.samplerate = sfFs;

    sfRecFile = sf_open(fileName, SFM_WRITE, &sfInfo);
    if(sfRecFile == NULL) {
        const char *strErr = sf_strerror(NULL);
        fprintf(stderr, " %s Couldn't open: %s\n", strErr, fileName);
    }

    return sfRecFile;
}

  
/*--------------------------------------------------------------------------------------------------------*\

                                                  MAIN

\*--------------------------------------------------------------------------------------------------------*/

int main(int argc, char *argv[]) {
    struct freedv      *f;
    PaError             err;
    PaStreamParameters  inputParameters, outputParameters;
    const PaDeviceInfo *deviceInfo = NULL;
    PaStream           *stream = NULL;
    int                 j, src_error, inputChannels, nin, devNum;
    int                 outputChannels;
    int                 state, next_state;
    SRC_STATE          *rxsrc, *txsrc;
    SRC_STATE          *playsrc;
    struct FIFO        *fifo;
    char                txFileName[MAX_CHAR];
    SNDFILE            *sfPlayFile, *sfRecFileFromRadio, *sfRecFileDecAudio;
    int                 sfFs;
    int                 fssc;                 
    int                 triggerf, txfilenamef, callsignf, sampleratef;
    int                 sync;
    char                commport[MAX_CHAR];
    char                callsign[MAX_CHAR];
    FILE               *ftmp;
    unsigned int        sync_counter, peakCounter;
    unsigned int        tnout,mnout;
    short               peak;
    unsigned int        logCounter;

    /* debug raw file */

    ftmp = fopen("t.raw", "wb");
    assert(ftmp != NULL);

    /* Defaults -------------------------------------------------------------------------------*/

    devNum = 0;
    fssc = FS48;
    sprintf(triggerString, "FreeBeacon");
    sprintf(txFileName, "txaudio.wav");
    sprintf(callsign, "FreeBeacon");
    verbose = 0;
    com_handle = COM_HANDLE_INVALID;
    mnout = 60*FS8;
    state = SRX_IDLE;
    *txtMsg = 0;
    sfRecFileFromRadio = NULL;
    sfRecFileDecAudio = NULL;
    
    if (Pa_Initialize()) {
        fprintf(stderr, "Port Audio failed to initialize");
        exit(1);
    }
 
    /* Process command line options -----------------------------------------------------------*/

    char* opt_string = "hlvc:t";
    struct option long_options[] = {
        { "dev", required_argument, &devNum, 1 },
        { "trigger", required_argument, &triggerf, 1 },
        { "txfilename", required_argument, &txfilenamef, 1 },
        { "callsign", required_argument, &callsignf, 1 },
        { "samplerate", required_argument, &sampleratef, 1 },
        { "list", no_argument, NULL, 'l' },
        { "help", no_argument, NULL, 'h' },
        { NULL, no_argument, NULL, 0 }
    };
    int num_opts=sizeof(long_options)/sizeof(struct option);

    while(1) {
        int option_index = 0;
        int opt = getopt_long(argc, argv, opt_string,
                    long_options, &option_index);
        if (opt == -1)
            break;

        switch (opt) {
        case 0:
            if (strcmp(long_options[option_index].name, "dev") == 0) {
                devNum = atoi(optarg);
            } else if(strcmp(long_options[option_index].name, "trigger") == 0) {
                strcpy(triggerString, optarg);
            } else if(strcmp(long_options[option_index].name, "txfilename") == 0) {
                strcpy(txFileName, optarg);
            } else if(strcmp(long_options[option_index].name, "callsign") == 0) {
                strcpy(callsign, optarg);
            } else if (strcmp(long_options[option_index].name, "samplerate") == 0) {
                fssc = atoi(optarg);
            }
            break;

        case 'c':
            strcpy(commport, optarg);
            if (openComPort(commport) != 0) {
                fprintf(stderr, "Can't open comm port: %s\n",commport);
                exit(1);
            }
            break;

        case 'h':
            printHelp(long_options, num_opts, argv);
            break;

        case 'v':
            verbose = 1;
            break;

        case 't':
            sprintf(txtMsg,"tx Test");
            state = STX;
            break;

        case 'l':
            listAudioDevices();
            exit(0);
            break;

        default:
            /* This will never be reached */
            break;
        }
    }

    /* Open Sound Device and start processing --------------------------------------------------------------*/

    f = freedv_open(FREEDV_MODE_1600); assert(f != NULL);
    int   fsm   = freedv_get_modem_sample_rate(f);     /* modem sample rate                                   */
    int   n8m   = freedv_get_n_nom_modem_samples(f);   /* nominal modem sample buffer size at fsm sample rate */
    int   n48   = n8m*fssc/fsm;                        /* nominal modem sample buffer size at 48kHz           */
    
    if (verbose)
        fprintf(stderr, "fsm: %d n8m: %d n48: %d\n", fsm, n8m, n48);

    short stereo[2*n48];                               /* stereo I/O buffer from port audio                   */
    short rx48k[n48], tx48k[n48];                      /* signals at 48 kHz                                   */
    short rxfsm[n48];                                  /* rx signal at modem sample rate                      */

    freedv_set_callback_txt(f, callbackNextRxChar, callbackNextTxChar, NULL);

    fifo = fifo_create(4*n8m); assert(fifo != NULL);   /* fifo to smooth out variation in demod nin          */

    /* states for sample rate converters */

    rxsrc = src_new(SRC_SINC_FASTEST, 1, &src_error); assert(rxsrc != NULL);
    txsrc = src_new(SRC_SINC_FASTEST, 1, &src_error); assert(txsrc != NULL);
    playsrc = src_new(SRC_SINC_FASTEST, 1, &src_error); assert(playsrc != NULL);

    /* Open Port Audio device and set up config structures -----------------------------------------------------*/

    deviceInfo = Pa_GetDeviceInfo(devNum);
    if (deviceInfo == NULL) {
        fprintf(stderr, "Couldn't get device info from Port Audio for device: %d\n", devNum);
        exit(1);
    }
    if (deviceInfo->maxInputChannels == 1)
        inputChannels = 1;
    else
        inputChannels = 2;

    /* input device */

    inputParameters.device = devNum;
    inputParameters.channelCount = inputChannels;
    inputParameters.sampleFormat = paInt16;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultHighInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    /* output device */

    if (deviceInfo->maxOutputChannels == 1)
        outputChannels = 1;
    else
        outputChannels = 2;

    outputParameters.device = devNum;
    outputParameters.channelCount = outputChannels;
    outputParameters.sampleFormat = paInt16;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    /* open port audio for full duplex operation */

    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              &outputParameters,
              fssc,
              n48,         /* changed from 0 to n48 to get Rpi audio to work without clicks */ 
              paClipOff,    
              NULL,        /* no callback, use blocking API */
              NULL ); 

    if (err != paNoError) {
        fprintf(stderr, "Couldn't initialise sound device\n");       
        exit(1);
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "Couldn't start sound device\n");       
        exit(1);
    }


    /* Init for main loop ----------------------------------------------------------------------------*/

    fprintf(stderr, "\nCtrl-C to exit\n");
    fprintf(stderr, "trigger string: %s\ntxFileName: %s\n", triggerString, txFileName);
    fprintf(stderr, "PortAudio devNum: %d\nsamplerate: %d\n", devNum, fssc);

    signal(SIGINT, intHandler);  /* ctrl-C to exit gracefully */
    keepRunning = 1;
    ptxtMsg = txtMsg;
    triggered = 0;
    peakCounter = 0;
    logCounter = 0;
    if (com_handle != COM_HANDLE_INVALID) {
        lowerRTS(); lowerDTR();
    }
    if (state == STX) {
        raiseRTS(); raiseDTR();
        sfPlayFile = openPlayFile(txFileName, &sfFs);
    }

    /* Main loop -------------------------------------------------------------------------------------*/

    while(keepRunning) {

        if (state != STX) {
            short demod_in[freedv_get_n_max_modem_samples(f)];
            short speech_out[freedv_get_n_speech_samples(f)];

            /* Read samples froun sound card, resample to modem sample rate */

            Pa_ReadStream(stream, stereo, n48);

            if (inputChannels == 2) {
                for(j=0; j<n48; j++)
                    rx48k[j] = stereo[2*j]; /* left channel only */
            }
            else {
                for(j=0; j<n48; j++)
                    rx48k[j] = stereo[j]; 
            }
            //fwrite(rx48k, sizeof(short), n8m, ftmp);
            int n8m_out = resample(rxsrc, rxfsm, rx48k, fsm, fssc, n8m, n48);
          
            if (verbose) {
                /* crude input signal level meter */
                peak = 0;
                for (j=0; j<n8m_out; j++)
                    if (rxfsm[j] > peak)
                        peak = rxfsm[j];
                if (peakCounter++ == PEAK_COUNTER) {
                    peakCounter = 0;
                }
            }

            //fifo_write(fifo, rxfsm, n8m_out);

            /* demodulate to decoded speech samples */

            nin = freedv_nin(f);
            while (fifo_read(fifo, demod_in, nin) == 0) {
                int nout = freedv_rx(f, speech_out, demod_in);
                freedv_get_modem_stats(f, &sync, &snr_est);

                if (sfRecFileFromRadio)
                    sf_write_short(sfRecFileFromRadio, demod_in, nin);
                if (sfRecFileDecAudio)
                    sf_write_short(sfRecFileDecAudio, speech_out, nout);
                tnout += nout;
                if (tnout > mnout) {
                    sf_close(sfRecFileFromRadio);
                    sf_close(sfRecFileDecAudio);
                }
            }
        }


        if (state == STX) {
            short mod_out[freedv_get_n_max_modem_samples(f)];
            short speech_in[freedv_get_n_speech_samples(f)];
            
            if (sfPlayFile != NULL) {
                /* resample input sound file as can't guarantee 8KHz sample rate */

                unsigned int nsf = freedv_get_n_speech_samples(f)*sfFs/FS8;
                short        insf[nsf];
                unsigned int n = sf_read_short(sfPlayFile, insf, nsf);
                resample(playsrc, speech_in, insf, FS8, sfFs, freedv_get_n_speech_samples(f), nsf);

                //fwrite(speech_in, sizeof(short), freedv_get_n_nom_modem_samples(f), ftmp);

                if (n != nsf) {
                    /* end of file - this signals state machine we've finished */
                    sf_close(sfPlayFile);
                    sfPlayFile = NULL;
                }
            }

            freedv_tx(f, mod_out, speech_in);
            //fwrite(mod_out, sizeof(short), freedv_get_n_nom_modem_samples(f), ftmp);

            int n48_out = resample(txsrc, tx48k, mod_out, fssc, fsm, n48, n8m);
            //printf("n48_out: %d n48: %d n_nom: %d\n", n48_out, n48, n8m);
            //fwrite(tx48k, sizeof(short), n48_out, ftmp);
            for(j=0; j<n48_out; j++) {
                if (outputChannels == 2) {
                    stereo[2*j] = tx48k[j];   // left channel
                    stereo[2*j+1] = tx48k[j]; // right channel
                }
                else {
                    stereo[j] = tx48k[j];     // mono
                }
            }

            Pa_WriteStream(stream, stereo, n48_out);
        }

        /* state machine processing */

        next_state = state;

        switch(state) {
        case SRX_IDLE:
            if (sync) {
                next_state = SRX_MAYBE_SYNC;
                sync_counter = 0;
                *txtMsg = 0;
                ptxtMsg = txtMsg;
                triggered = 0;
                freedv_set_total_bit_errors(f, 0);
                freedv_set_total_bits(f, 0);
            }
            break;
        case SRX_MAYBE_SYNC:
            if (sync) {
                sync_counter++;
                if (sync_counter == SYNC_FRAMES) {
                    /* OK we really are in sync */

                    /* kick off recording of two files */

                    time_t ltime;     /* calendar time */
                    ltime=time(NULL); /* get current cal time */
                    char timeStr[MAX_CHAR];
                    sprintf(timeStr, "%s",asctime( localtime(&ltime) ) );
                    int i=0;
                    while (timeStr[i]) {
                        if (isspace(timeStr[i]) || (timeStr[i] == ':')) 
                            timeStr[i]='_';
                        else
                            timeStr[i] = tolower(timeStr[i]);
                        i++;
                    }
                    char recFileFromRadioName[MAX_CHAR], recFileDecAudioName[MAX_CHAR];
                    sprintf(recFileFromRadioName,"%s_from_radio.wav", timeStr);
                    sprintf(recFileDecAudioName,"%s_decoded_speech.wav", timeStr);
                    sfRecFileFromRadio = openRecFile(recFileFromRadioName, fsm);
                    sfRecFileDecAudio = openRecFile(recFileDecAudioName, FS8);
                    tnout = 0;

                    next_state = SRX_SYNC;
                }
            }
            else
                next_state = SRX_IDLE;
            break;
        case SRX_SYNC:
            sync_counter++;
            if (!sync) {
                sync_counter = 0;
                next_state = SRX_MAYBE_UNSYNC;
            }
            break;
        case SRX_MAYBE_UNSYNC:
            if (!sync) {
                sync_counter++;
                if (sync_counter == UNSYNC_FRAMES) {
                    /* we really are out of sync */

                    /* finish up any open recording files */

                    if (sfRecFileFromRadio)
                        sf_close(sfRecFileFromRadio);
                    if (sfRecFileDecAudio)
                        sf_close(sfRecFileDecAudio);

                    /* kick off a tx if triggered */

                    if (triggered) {
                        float ber = (float)freedv_get_total_bit_errors(f)/freedv_get_total_bits(f);
                        char tmpStr[MAX_CHAR];

                        sprintf(tmpStr, "SNR: %3.1f BER: %4.3f de %s\r",
                                snr_sample, ber, callsign);
                        strcpy(txtMsg, tmpStr);
                        //fprintf(stderr, "TX txtMsg: %s\n", txtMsg);
                        ptxtMsg = txtMsg;
                        sfPlayFile = openPlayFile(txFileName, &sfFs);

                        if (com_handle != COM_HANDLE_INVALID) {
                            raiseRTS(); raiseDTR();
                        }
                        next_state = STX;
                    }
                    else {
                        next_state = SRX_IDLE;
                    }
                }
            }
            else
                next_state = SRX_SYNC; /* sync is back so false alarm */
            break;
        case STX:
            if (sfPlayFile == NULL) {

                if (com_handle != COM_HANDLE_INVALID) {
                    lowerRTS(); lowerDTR();
                }
                next_state = SRX_IDLE;
            }
            break;
        }

        if (verbose) {
            if (logCounter++ == LOG_COUNTER) {
                logCounter = 0;
                fprintf(stderr, "state: %-20s  peak: %6d  sync: %d  SNR: %3.1f  triggered: %d\n", 
                        state_str[state], peak, sync, snr_est, triggered);
            }
        }

        state = next_state;
    }

    /* lower PTT lines */

    if (com_handle != COM_HANDLE_INVALID) {
        lowerRTS(); lowerDTR();
    }

    /* Shut down port audio */

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        fprintf(stderr, "Couldn't stop sound device\n");       
        exit(1);
    }
    Pa_CloseStream(stream);
    Pa_Terminate();

    fifo_destroy(fifo);
    src_delete(rxsrc);
    src_delete(txsrc);
    src_delete(playsrc);
    freedv_close(f);
    fclose(ftmp);

    return 0;
}


/*--------------------------------------------------------------------------------------------------------*\

                                      Comm port fuctions lifted from FreeDV

\*--------------------------------------------------------------------------------------------------------*/

//----------------------------------------------------------------
// openComPort() opens the com port specified by the string
// ie: "/dev/ttyUSB0" 
//----------------------------------------------------------------

int openComPort(const char *name)
{
    if(com_handle != COM_HANDLE_INVALID)
        closeComPort();

    {
        struct termios t;

        if((com_handle=open(name, O_NONBLOCK|O_RDWR))==COM_HANDLE_INVALID)
            return -1;

        if(tcgetattr(com_handle, &t)==-1) {
            close(com_handle);
            com_handle = COM_HANDLE_INVALID;
            return -1;
        }

        t.c_iflag = (
                     IGNBRK   /* ignore BREAK condition */
                     | IGNPAR   /* ignore (discard) parity errors */
                     );
        t.c_oflag = 0;	/* No output processing */
        t.c_cflag = (
                     CS8         /* 8 bits */
                     | CREAD       /* enable receiver */
                     /*
                       Fun snippet from the FreeBSD manpage:

                       If CREAD is set, the receiver is enabled.  Otherwise, no character is
                       received.  Not all hardware supports this bit.  In fact, this flag is
                       pretty silly and if it were not part of the termios specification it
                       would be omitted.
                     */
                     | CLOCAL      /* ignore modem status lines */
                     );
        t.c_lflag = 0;	/* No local modes */
        if(tcsetattr(com_handle, TCSANOW, &t)==-1) {
            close(com_handle);
            com_handle = COM_HANDLE_INVALID;
            return -1;
        }
		
    }

    return 0;
}

void closeComPort(void)
{
    close(com_handle);
    com_handle = COM_HANDLE_INVALID;
}

//----------------------------------------------------------------
// (raise|lower)(RTS|DTR)()
//
// Raises/lowers the specified signal
//----------------------------------------------------------------

void raiseDTR(void)
{
    if(com_handle == COM_HANDLE_INVALID)
        return;
    {	// For C89 happiness
        int flags = TIOCM_DTR;
        ioctl(com_handle, TIOCMBIS, &flags);
    }
}


void raiseRTS(void)
{
    if(com_handle == COM_HANDLE_INVALID)
        return;
    {	// For C89 happiness
        int flags = TIOCM_RTS;
        ioctl(com_handle, TIOCMBIS, &flags);
    }
}

void lowerDTR(void)
{
    if(com_handle == COM_HANDLE_INVALID)
        return;
    {	// For C89 happiness
        int flags = TIOCM_DTR;
        ioctl(com_handle, TIOCMBIC, &flags);
    }
}

void lowerRTS(void)
{
    if(com_handle == COM_HANDLE_INVALID)
        return;
    {	// For C89 happiness
        int flags = TIOCM_RTS;
        ioctl(com_handle, TIOCMBIC, &flags);
    }
}

