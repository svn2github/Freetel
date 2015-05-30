/*---------------------------------------------------------------------------*\
                                                                             
  FILE........: cohpsk_put_test_bits.c
  AUTHOR......: David Rowe  
  DATE CREATED: April 2015
                                                                             
  Sinks a stream of test bits generated by cohpsk_get_test_bits, useful for 
  testing coh psk mod and demod.

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

#include "codec2_cohpsk.h"
#include "test_bits_coh.h"
#include "octave.h"

#define LOG_FRAMES 100

int main(int argc, char *argv[])
{
    FILE         *fin, *foct;
    int           rx_bits[COHPSK_BITS_PER_FRAME];
    int          *ptest_bits_coh, *ptest_bits_coh_end;
    int           state, next_state, i, nbits, errors, nerrors;
    int           error_positions_hist[COHPSK_BITS_PER_FRAME], logframes;
    int           nerr_log[LOG_FRAMES];

    for(i=0; i<COHPSK_BITS_PER_FRAME; i++)
        error_positions_hist[i] = 0;

    if (argc < 2) {
	fprintf(stderr, "usage: %s InputOneBitPerIntFile [OctaveLogFile]\n", argv[0]);
	exit(1);
    }

    if (strcmp(argv[1], "-") == 0) fin = stdin;
    else if ( (fin = fopen(argv[1],"rb")) == NULL ) {
	fprintf(stderr, "Error opening input file: %s: %s.\n",
         argv[1], strerror(errno));
	exit(1);
    }

    foct = NULL;
    logframes = 0;
    if (argc == 3) {
        if ( (foct = fopen(argv[2],"wt")) == NULL ) {
            fprintf(stderr, "Error opening output Octave file: %s: %s.\n",
                    argv[2], strerror(errno));            
	exit(1);
        }
    }

    for(i=0; i<COHPSK_BITS_PER_FRAME; i++)
        error_positions_hist[i] = 0;

    ptest_bits_coh = (int*)test_bits_coh;
    ptest_bits_coh_end = (int*)test_bits_coh + sizeof(test_bits_coh)/sizeof(int);

    state = 0; nbits = 0; nerrors = 0;
    while (fread(rx_bits, sizeof(int), COHPSK_BITS_PER_FRAME, fin) ==  COHPSK_BITS_PER_FRAME) {

        errors = 0;
        for(i=0; i<COHPSK_BITS_PER_FRAME; i++) {
            errors += (rx_bits[i] & 0x1) ^ ptest_bits_coh[i];
            if (state == 1) {
                if ((state == 1 ) && (rx_bits[i] & 0x1) ^ ptest_bits_coh[i])
                    error_positions_hist[i]++;
            }
        }
        //printf("state: %d errors: %d nerrors: %d\n", state, errors, nerrors);

        /* state logic */

        next_state = state;

        if (state == 0) {
            if (errors < 4) {
                next_state = 1;
                ptest_bits_coh += COHPSK_BITS_PER_FRAME;
                nerrors = errors;
                nbits = COHPSK_BITS_PER_FRAME;
                if (logframes < LOG_FRAMES)
                    nerr_log[logframes++] = errors;
            }
        }

        if (state == 1) {
            nerrors += errors;
            nbits   += COHPSK_BITS_PER_FRAME;
            ptest_bits_coh += COHPSK_BITS_PER_FRAME;
            if (ptest_bits_coh >= ptest_bits_coh_end) {
                ptest_bits_coh = (int*)test_bits_coh;
            }
            if (logframes < LOG_FRAMES)
                nerr_log[logframes++] = errors;
        }

        state = next_state;

        if (fin == stdin) fflush(stdin);
    }

    if (foct != NULL) {
        octave_save_int(foct, "nerr_log_c", nerr_log, 1, logframes);  
        octave_save_int(foct, "error_positions_hist_c", error_positions_hist, 1, logframes);  
        fclose(foct);
    }

    fclose(fin);
    fprintf(stderr, "BER: %4.3f Nbits: %d Nerrors: %d\n", (float)nerrors/nbits, nbits, nerrors);

    return 0;
}

