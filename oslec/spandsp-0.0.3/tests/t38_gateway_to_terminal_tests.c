/*
 * SpanDSP - a series of DSP components for telephony
 *
 * t38_gateway_to_terminal_tests.c - Joint tests for the T.38 FoIP terminal and gateway modules.
 *
 * Written by Steve Underwood <steveu@coppice.org>
 *
 * Copyright (C) 2005, 2006 Steve Underwood
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2, as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: t38_gateway_to_terminal_tests.c,v 1.31 2006/12/09 04:56:20 steveu Exp $
 */

/*! \file */

/*! \page t38_gateway_to_terminal_tests_page T.38 mixed gateway and termination tests
\section t38_gateway_to_terminal_tests_page_sec_1 What does it do?
These tests exercise the path

    FAX machine -> T.38 gateway -> T.38 termination
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#if defined(HAVE_TGMATH_H)
#include <tgmath.h>
#endif
#if defined(HAVE_MATH_H)
#include <math.h>
#endif
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#define __USE_MISC
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/time.h>
#include <audiofile.h>
#include <tiffio.h>

#include "spandsp.h"

#include "ip_network_model.h"

#define SAMPLES_PER_CHUNK       160

#define INPUT_FILE_NAME         "../itutests/fax/itutests.tif"
#define OUTPUT_FILE_NAME        "t38.tif"
#define OUTPUT_FILE_NAME_WAVE   "t38_gateway_to_terminal.wav"

fax_state_t fax_state_a;
t38_gateway_state_t t38_state_a;
t38_terminal_state_t t38_state_b;

ip_network_model_state_t *path_a_to_b;
ip_network_model_state_t *path_b_to_a;

int done[2] = {FALSE, FALSE};
int succeeded[2] = {FALSE, FALSE};

static void phase_b_handler(t30_state_t *s, void *user_data, int result)
{
    int i;
    
    i = (int) (intptr_t) user_data;
    printf("%c: Phase B handler on channel %c - (0x%X) %s\n", i, i, result, t30_frametype(result));
}
/*- End of function --------------------------------------------------------*/

static void phase_d_handler(t30_state_t *s, void *user_data, int result)
{
    int i;
    t30_stats_t t;
    char ident[21];

    i = (int) (intptr_t) user_data;
    printf("%c: Phase D handler on channel %c - (0x%X) %s\n", i, i, result, t30_frametype(result));
    t30_get_transfer_statistics(s, &t);
    printf("%c: Phase D: bit rate %d\n", i, t.bit_rate);
    printf("%c: Phase D: ECM %s\n", i, (t.error_correcting_mode)  ?  "on"  :  "off");
    printf("%c: Phase D: pages transferred %d\n", i, t.pages_transferred);
    printf("%c: Phase D: image size %d x %d\n", i, t.width, t.length);
    printf("%c: Phase D: image resolution %d x %d\n", i, t.x_resolution, t.y_resolution);
    printf("%c: Phase D: bad rows %d\n", i, t.bad_rows);
    printf("%c: Phase D: longest bad row run %d\n", i, t.longest_bad_row_run);
    printf("%c: Phase D: coding method %s\n", i, t4_encoding_to_str(t.encoding));
    printf("%c: Phase D: image size %d\n", i, t.image_size);
    t30_get_local_ident(s, ident);
    printf("%c: Phase D: local ident '%s'\n", i, ident);
    t30_get_far_ident(s, ident);
    printf("%c: Phase D: remote ident '%s'\n", i, ident);
}
/*- End of function --------------------------------------------------------*/

static void phase_e_handler(t30_state_t *s, void *user_data, int result)
{
    int i;
    t30_stats_t t;
    char ident[21];
    
    i = (int) (intptr_t) user_data;
    printf("%c: Phase E handler on channel %c - (%d) %s\n", i, i, result, t30_completion_code_to_str(result));
    t30_get_transfer_statistics(s, &t);
    printf("%c: Phase E: bit rate %d\n", i, t.bit_rate);
    printf("%c: Phase E: ECM %s\n", i, (t.error_correcting_mode)  ?  "on"  :  "off");
    printf("%c: Phase E: pages transferred %d\n", i, t.pages_transferred);
    printf("%c: Phase E: image size %d x %d\n", i, t.width, t.length);
    printf("%c: Phase E: image resolution %d x %d\n", i, t.x_resolution, t.y_resolution);
    printf("%c: Phase E: bad rows %d\n", i, t.bad_rows);
    printf("%c: Phase E: longest bad row run %d\n", i, t.longest_bad_row_run);
    printf("%c: Phase E: coding method %s\n", i, t4_encoding_to_str(t.encoding));
    printf("%c: Phase E: image size %d bytes\n", i, t.image_size);
    t30_get_local_ident(s, ident);
    printf("%c: Phase E: local ident '%s'\n", i, ident);
    t30_get_far_ident(s, ident);
    printf("%c: Phase E: remote ident '%s'\n", i, ident);
    succeeded[i - 'A'] = (result == T30_ERR_OK)  &&  (t.pages_transferred == 12);
    done[i - 'A'] = TRUE;
}
/*- End of function --------------------------------------------------------*/

static int tx_packet_handler_a(t38_core_state_t *s, void *user_data, const uint8_t *buf, int len, int count)
{
    t38_terminal_state_t *t;

    /* This routine queues messages between two instances of T.38 processing */
    t = (t38_terminal_state_t *) user_data;
    span_log(&s->logging, SPAN_LOG_FLOW, "Send seq %d, len %d, count %d\n", s->tx_seq_no, len, count);

    ip_network_model_send(path_a_to_b, s->tx_seq_no, count, buf, len);
    return 0;
}
/*- End of function --------------------------------------------------------*/

static int tx_packet_handler_b(t38_core_state_t *s, void *user_data, const uint8_t *buf, int len, int count)
{
    t38_gateway_state_t *t;

    /* This routine queues messages between two instances of T.38 processing */
    t = (t38_gateway_state_t *) user_data;
    span_log(&s->logging, SPAN_LOG_FLOW, "Send seq %d, len %d, count %d\n", s->tx_seq_no, len, count);

    ip_network_model_send(path_b_to_a, s->tx_seq_no, count, buf, len);
    return 0;
}
/*- End of function --------------------------------------------------------*/

int main(int argc, char *argv[])
{
    int16_t t38_amp_a[SAMPLES_PER_CHUNK];
    int16_t t30_amp_a[SAMPLES_PER_CHUNK];
    int16_t out_amp[2*SAMPLES_PER_CHUNK];
    int t38_len_a;
    int t30_len_a;
    int msg_len;
    uint8_t msg[1024];
    int log_audio;
    int outframes;
    int t38_version;
    int use_ecm;
    AFfilesetup filesetup;
    AFfilehandle wave_handle;
    const char *input_file_name;
    int i;
    int seq_no;

    log_audio = FALSE;
    t38_version = 1;
    use_ecm = FALSE;
    input_file_name = INPUT_FILE_NAME;
    for (i = 1;  i < argc;  i++)
    {
        if (strcmp(argv[i], "-e") == 0)
        {
            use_ecm = TRUE;
            continue;
        }
        if (strcmp(argv[i], "-i") == 0)
        {
            i++;
            input_file_name = argv[i];
            continue;
        }
        if (strcmp(argv[i], "-l") == 0)
        {
            log_audio = TRUE;
            continue;
        }
        if (strcmp(argv[i], "-v") == 0)
        {
            i++;
            t38_version = atoi(argv[i]);
            continue;
        }
    }

    printf("Using T.38 version %d\n", t38_version);
    if (use_ecm)
        printf("Using ECM\n");

    filesetup = AF_NULL_FILESETUP;
    wave_handle = AF_NULL_FILEHANDLE;
    if (log_audio)
    {
        if ((filesetup = afNewFileSetup()) == AF_NULL_FILESETUP)
        {
            fprintf(stderr, "    Failed to create file setup\n");
            exit(2);
        }
        afInitSampleFormat(filesetup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);
        afInitRate(filesetup, AF_DEFAULT_TRACK, (float) SAMPLE_RATE);
        afInitFileFormat(filesetup, AF_FILE_WAVE);
        afInitChannels(filesetup, AF_DEFAULT_TRACK, 2);

        wave_handle = afOpenFile(OUTPUT_FILE_NAME_WAVE, "w", filesetup);
        if (wave_handle == AF_NULL_FILEHANDLE)
        {
            fprintf(stderr, "    Cannot create wave file '%s'\n", OUTPUT_FILE_NAME_WAVE);
            exit(2);
        }
    }

    if ((path_a_to_b = ip_network_model_init(800, 2000, 0)) == NULL)
    {
        fprintf(stderr, "Failed to start IP network path model\n");
        exit(2);
    }
    if ((path_b_to_a = ip_network_model_init(800, 2000, 0)) == NULL)
    {
        fprintf(stderr, "Failed to start IP network path model\n");
        exit(2);
    }

    fax_init(&fax_state_a, TRUE);
    t30_set_local_ident(&fax_state_a.t30_state, "11111111");
    t30_set_tx_file(&fax_state_a.t30_state, input_file_name, -1, -1);
    t30_set_phase_b_handler(&fax_state_a.t30_state, phase_b_handler, (void *) (intptr_t) 'A');
    t30_set_phase_d_handler(&fax_state_a.t30_state, phase_d_handler, (void *) (intptr_t) 'A');
    t30_set_phase_e_handler(&fax_state_a.t30_state, phase_e_handler, (void *) (intptr_t) 'A');
    t30_set_ecm_capability(&fax_state_a.t30_state, use_ecm);
    if (use_ecm)
        t30_set_supported_compressions(&fax_state_a.t30_state, T30_SUPPORT_T4_1D_COMPRESSION | T30_SUPPORT_T4_2D_COMPRESSION | T30_SUPPORT_T6_COMPRESSION);
    span_log_set_level(&fax_state_a.logging, SPAN_LOG_DEBUG | SPAN_LOG_SHOW_TAG | SPAN_LOG_SHOW_SAMPLE_TIME);
    span_log_set_tag(&fax_state_a.logging, "FAX-A ");
    span_log_set_level(&fax_state_a.t30_state.logging, SPAN_LOG_DEBUG | SPAN_LOG_SHOW_TAG | SPAN_LOG_SHOW_SAMPLE_TIME);
    span_log_set_tag(&fax_state_a.t30_state.logging, "FAX-A ");
    memset(t30_amp_a, 0, sizeof(t30_amp_a));

    if (t38_gateway_init(&t38_state_a, tx_packet_handler_a, &t38_state_b) == NULL)
    {
        fprintf(stderr, "Cannot start the T.38 channel\n");
        exit(2);
    }
    t38_set_t38_version(&t38_state_a.t38, t38_version);
    t38_gateway_ecm_control(&t38_state_a, use_ecm);
    span_log_set_level(&t38_state_a.logging, SPAN_LOG_DEBUG | SPAN_LOG_SHOW_TAG | SPAN_LOG_SHOW_SAMPLE_TIME);
    span_log_set_tag(&t38_state_a.logging, "T.38-A");
    span_log_set_level(&t38_state_a.t38.logging, SPAN_LOG_DEBUG | SPAN_LOG_SHOW_TAG | SPAN_LOG_SHOW_SAMPLE_TIME);
    span_log_set_tag(&t38_state_a.t38.logging, "T.38-A");
    memset(t38_amp_a, 0, sizeof(t38_amp_a));

    if (t38_terminal_init(&t38_state_b, FALSE, tx_packet_handler_b, &t38_state_a) == NULL)
    {
        fprintf(stderr, "Cannot start the T.38 channel\n");
        exit(2);
    }
    t38_set_t38_version(&t38_state_b.t38, t38_version);
    span_log_set_level(&t38_state_b.logging, SPAN_LOG_DEBUG | SPAN_LOG_SHOW_TAG | SPAN_LOG_SHOW_SAMPLE_TIME);
    span_log_set_tag(&t38_state_b.logging, "T.38-B");
    span_log_set_level(&t38_state_b.t38.logging, SPAN_LOG_DEBUG | SPAN_LOG_SHOW_TAG | SPAN_LOG_SHOW_SAMPLE_TIME);
    span_log_set_tag(&t38_state_b.t38.logging, "T.38-B");
    span_log_set_level(&t38_state_b.t30_state.logging, SPAN_LOG_DEBUG | SPAN_LOG_SHOW_TAG | SPAN_LOG_SHOW_SAMPLE_TIME);
    span_log_set_tag(&t38_state_b.t30_state.logging, "T.38-B");

    t30_set_local_ident(&t38_state_b.t30_state, "22222222");
    t30_set_rx_file(&t38_state_b.t30_state, OUTPUT_FILE_NAME, -1);
    t30_set_phase_b_handler(&t38_state_b.t30_state, phase_b_handler, (void *) (intptr_t) 'B');
    t30_set_phase_d_handler(&t38_state_b.t30_state, phase_d_handler, (void *) (intptr_t) 'B');
    t30_set_phase_e_handler(&t38_state_b.t30_state, phase_e_handler, (void *) (intptr_t) 'B');
    t30_set_ecm_capability(&t38_state_b.t30_state, use_ecm);
    if (use_ecm)
        t30_set_supported_compressions(&t38_state_b.t30_state, T30_SUPPORT_T4_1D_COMPRESSION | T30_SUPPORT_T4_2D_COMPRESSION | T30_SUPPORT_T6_COMPRESSION);

    for (;;)
    {
        span_log_bump_samples(&fax_state_a.logging, SAMPLES_PER_CHUNK);
        span_log_bump_samples(&fax_state_a.t30_state.logging, SAMPLES_PER_CHUNK);
        span_log_bump_samples(&t38_state_a.logging, SAMPLES_PER_CHUNK);
        span_log_bump_samples(&t38_state_a.t38.logging, SAMPLES_PER_CHUNK);
        span_log_bump_samples(&t38_state_b.logging, SAMPLES_PER_CHUNK);
        span_log_bump_samples(&t38_state_b.t38.logging, SAMPLES_PER_CHUNK);
        span_log_bump_samples(&t38_state_b.t30_state.logging, SAMPLES_PER_CHUNK);
        memset(out_amp, 0, sizeof(out_amp));

        t38_terminal_send_timeout(&t38_state_b, SAMPLES_PER_CHUNK);

        t30_len_a = fax_tx(&fax_state_a, t30_amp_a, SAMPLES_PER_CHUNK);
        /* The receive side always expects a full block of samples, but the
           transmit side may not be sending any when it doesn't need to. We
           may need to pad with some silence. */
        if (t30_len_a < SAMPLES_PER_CHUNK)
        {
            memset(t30_amp_a + t30_len_a, 0, sizeof(int16_t)*(SAMPLES_PER_CHUNK - t30_len_a));
            t30_len_a = SAMPLES_PER_CHUNK;
        }
        if (log_audio)
        {
            for (i = 0;  i < t30_len_a;  i++)
                out_amp[2*i] = t30_amp_a[i];
        }
        if (t38_gateway_rx(&t38_state_a, t30_amp_a, t30_len_a))
            break;
    
        t38_len_a = t38_gateway_tx(&t38_state_a, t38_amp_a, SAMPLES_PER_CHUNK);
        if (t38_len_a < SAMPLES_PER_CHUNK)
        {
            memset(t38_amp_a + t38_len_a, 0, sizeof(int16_t)*(SAMPLES_PER_CHUNK - t38_len_a));
            t38_len_a = SAMPLES_PER_CHUNK;
        }
        if (log_audio)
        {
            for (i = 0;  i < t38_len_a;  i++)
                out_amp[2*i + 1] = t38_amp_a[i];
        }
        if (fax_rx(&fax_state_a, t38_amp_a, SAMPLES_PER_CHUNK))
            break;

        while ((msg_len = ip_network_model_get(path_a_to_b, SAMPLES_PER_CHUNK, msg, 1024, &seq_no)) >= 0)
            t38_core_rx_ifp_packet(&t38_state_b.t38, seq_no, msg, msg_len);
        while ((msg_len = ip_network_model_get(path_b_to_a, SAMPLES_PER_CHUNK, msg, 1024, &seq_no)) >= 0)
            t38_core_rx_ifp_packet(&t38_state_a.t38, seq_no, msg, msg_len);

        if (log_audio)
        {
            outframes = afWriteFrames(wave_handle, AF_DEFAULT_TRACK, out_amp, SAMPLES_PER_CHUNK);
            if (outframes != SAMPLES_PER_CHUNK)
                break;
        }

        if (done[0]  &&  done[1])
            break;
    }
    if (log_audio)
    {
        if (afCloseFile(wave_handle) != 0)
        {
            fprintf(stderr, "    Cannot close wave file '%s'\n", OUTPUT_FILE_NAME_WAVE);
            exit(2);
        }
        afFreeFileSetup(filesetup);
    }
    if (!succeeded[0]  ||  !succeeded[1])
    {
        printf("Tests failed\n");
        exit(2);
    }
    printf("Tests passed\n");
    return  0;
}
/*- End of function --------------------------------------------------------*/
/*- End of file ------------------------------------------------------------*/
