/*---------------------------------------------------------------------------*\

  FILE........: ofdm_demod.c
  AUTHOR......: David Rowe
  DATE CREATED: Mar 2018

  Given an input file of raw file (8kHz, 16 bit shorts) of OFDM modem
  samples.  Optionally outputs one char per bit (hard decision), or
  soft decision rx_np and rx_amp QPSK symbols information for LDPC decoder.

\*---------------------------------------------------------------------------*/

/*
  Copyright (C) 2015 David Rowe

  All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License version 2, as
  published by the Free Software Foundation.  This program is
  distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
  License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>

#include "codec2_ofdm.h"
#include "ofdm_internal.h"
#include "octave.h"
#include "test_bits_ofdm.h"

#define ASCALE  (2E5*1.1491/2.0) /* scale from shorts back to floats */
#define NFRAMES 100              /* just log the first 100 frames    */

int opt_exists(char *argv[], int argc, char opt[]) {
    int i;
    for (i=0; i<argc; i++) {
        if (strcmp(argv[i], opt) == 0) {
            return i;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    FILE          *fin, *fout, *foct;
    struct OFDM   *ofdm;
    int            nin_frame;

    float          phase_est_pilot_log[OFDM_ROWSPERFRAME*NFRAMES][OFDM_NC];
    COMP           rx_np_log[OFDM_ROWSPERFRAME*OFDM_NC*NFRAMES];
    float          rx_amp_log[OFDM_ROWSPERFRAME*OFDM_NC*NFRAMES];
    float          foff_hz_log[NFRAMES];
    int            timing_est_log[NFRAMES];

    int            i, j, f, oct, logframes, arg, sd;

    if (argc < 3) {
        fprintf(stderr, "\n");
	printf("usage: %s InputModemRawFile OutputFile [-o OctaveLogFile] [--sd] [-v VerboseLevel]\n", argv[0]);
        fprintf(stderr, "\n");
        fprintf(stderr, "              Default output file format is one byte per bit hard decision\n");
        fprintf(stderr, "  -v          Verbose info the stderr\n");
        fprintf(stderr, "  -o          Octave log file for testing\n");
        fprintf(stderr, "  --sd        soft decision output, four doubles per QPSK symbol\n");
        fprintf(stderr, "\n");
	exit(1);
    }

    if (strcmp(argv[1], "-")  == 0) fin = stdin;
    else if ( (fin = fopen(argv[1],"rb")) == NULL ) {
	fprintf(stderr, "Error opening input modem sample file: %s: %s.\n",
         argv[1], strerror(errno));
	exit(1);
    }

    if (strcmp(argv[2], "-") == 0) fout = stdout;
    else if ( (fout = fopen(argv[2],"wb")) == NULL ) {
	fprintf(stderr, "Error opening output file: %s: %s.\n",
         argv[2], strerror(errno));
	exit(1);
    }

    foct = NULL;
    oct = 0;
    if ((arg = opt_exists(argv, argc, "-o")) != 0) {
        if ( (foct = fopen(argv[arg+1],"wt")) == NULL ) {
            fprintf(stderr, "Error opening output Octave file: %s: %s.\n",
                    argv[4], strerror(errno));
	exit(1);
        }
        oct = 1;
        logframes = NFRAMES;
    }

    sd = 0;
    if (opt_exists(argv, argc, "--sd")) {
        sd = 1;
    }

    ofdm = ofdm_create(OFDM_CONFIG_700D);
    assert(ofdm != NULL);

    if ((arg = opt_exists(argv, argc, "-v")) != 0) {
        ofdm_set_verbose(ofdm, atoi(argv[arg+1]));
    }

    int Nbitsperframe = ofdm_get_bits_per_frame(ofdm);
    int Nmaxsamperframe = ofdm_get_max_samples_per_frame();
    
    short  rx_scaled[Nmaxsamperframe];
    COMP   rxbuf_in[Nmaxsamperframe];
    int    rx_bits[Nbitsperframe];
    char   rx_bits_char[Nbitsperframe];
    int    rx_uw[OFDM_UW_LEN];
    f = 0;

    nin_frame = ofdm_get_nin(ofdm);
    while(fread(rx_scaled, sizeof(short), nin_frame, fin) == nin_frame) {

	/* scale and demod */

	for(i=0; i<nin_frame; i++) {
	    rxbuf_in[i].real = (float)rx_scaled[i]/ASCALE;
            rxbuf_in[i].imag = 0.0;
        }

        if (strcmp(ofdm->sync_state,"searching") == 0) {
            ofdm_sync_search(ofdm, rxbuf_in);
        }
    
        if ((strcmp(ofdm->sync_state,"synced") == 0) || (strcmp(ofdm->sync_state,"trial_sync") == 0) ) {
            ofdm_demod(ofdm, rx_bits, rxbuf_in);
            
            if (sd == 0) {
                /* simple hard decision output for uncoded testing */
                for(i=0; i<Nbitsperframe; i++) {
                    rx_bits_char[i] = rx_bits[i];
                }
                fwrite(rx_bits_char, sizeof(char), Nbitsperframe, fout);
            }

            /* extract Unique Word bits */

            for(i=0; i<OFDM_UW_LEN; i++) {
                rx_uw[i] = rx_bits[i];
            }
        }
        
        ofdm_sync_state_machine(ofdm, rx_uw);

        nin_frame = ofdm_get_nin(ofdm);

        if (ofdm->verbose) {
            fprintf(stderr, "f: %2d state: %-10s uw_errors: %2d %1d foff: %3.1f\n",
             f, ofdm->last_sync_state, ofdm->uw_errors, ofdm->sync_counter, ofdm->foff_est_hz);
        }

        /* optional logging of states */
        
        if (oct) {
            /* note corrected phase (rx no phase) is one big linear array for frame */

            for (i = 0; i < OFDM_ROWSPERFRAME*OFDM_NC; i++) {
                rx_np_log[OFDM_ROWSPERFRAME*OFDM_NC*f + i].real = crealf(ofdm->rx_np[i]);
                rx_np_log[OFDM_ROWSPERFRAME*OFDM_NC*f + i].imag = cimagf(ofdm->rx_np[i]);
            }

            /* note phase/amp ests the same for each col, but check them all anyway */

            for (i = 0; i < OFDM_ROWSPERFRAME; i++) {
                for (j = 0; j < OFDM_NC; j++) {
                    phase_est_pilot_log[OFDM_ROWSPERFRAME*f+i][j] = ofdm->aphase_est_pilot_log[OFDM_NC*i+j];
                    rx_amp_log[OFDM_ROWSPERFRAME*OFDM_NC*f+OFDM_NC*i+j] = ofdm->rx_amp[OFDM_NC*i+j];
                }
            }

            foff_hz_log[f] = ofdm->foff_est_hz;
            timing_est_log[f] = ofdm->timing_est + 1;     /* offset by 1 to match Octave */
            if (f == (logframes-1))
                oct = 0;
        }

	/* if this is in a pipeline, we probably don't want the usual
	   buffering to occur */

        if (fout == stdout) fflush(stdout);
        if (fin == stdin) fflush(stdin);

        f++;
    }

    fclose(fin);
    fclose(fout);

    /* optionally dump Octave files */

    if (foct != NULL) {
        octave_save_float(foct, "phase_est_pilot_log_c", (float*)phase_est_pilot_log, OFDM_ROWSPERFRAME*NFRAMES, OFDM_NC, OFDM_NC);
        octave_save_complex(foct, "rx_np_log_c", (COMP*)rx_np_log, 1, OFDM_ROWSPERFRAME*OFDM_NC*NFRAMES, OFDM_ROWSPERFRAME*OFDM_NC*NFRAMES);
        octave_save_float(foct, "rx_amp_log_c", (float*)rx_amp_log, 1, OFDM_ROWSPERFRAME*OFDM_NC*NFRAMES, OFDM_ROWSPERFRAME*OFDM_NC*NFRAMES);
        octave_save_float(foct, "foff_hz_log_c", foff_hz_log, NFRAMES, 1, 1);
        octave_save_int(foct, "timing_est_log_c", timing_est_log, NFRAMES, 1);
        fclose(foct);
    }

    ofdm_destroy(ofdm);

    return 0;
}
