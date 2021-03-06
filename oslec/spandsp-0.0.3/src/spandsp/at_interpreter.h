/*
 * SpanDSP - a series of DSP components for telephony
 *
 * at_interpreter.h - AT command interpreter to V.251, V.252, V.253, T.31 and the 3GPP specs.
 *
 * Written by Steve Underwood <steveu@coppice.org>
 *
 * Copyright (C) 2004, 2005, 2006 Steve Underwood
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
 * $Id: at_interpreter.h,v 1.10 2006/10/24 13:45:27 steveu Exp $
 */

/*! \file */

#if !defined(_AT_INTERPRETER_H_)
#define _AT_INTERPRETER_H_

/*! \page at_page AT command interpreter
\section at_page_sec_1 What does it do?
The AT interpreter module implements V.251, V.252, V.253, T.31 and various 3GPP
modem control commands.

\section at_page_sec_2 How does it work?
*/

typedef struct at_state_s at_state_t;

typedef int (at_modem_control_handler_t)(at_state_t *s, void *user_data, int op, const char *num);
typedef int (at_tx_handler_t)(at_state_t *s, void *user_data, const uint8_t *buf, size_t len);
typedef int (at_class1_handler_t)(at_state_t *s, void *user_data, int direction, int operation, int val);

enum at_rx_mode_e
{
    AT_MODE_ONHOOK_COMMAND,
    AT_MODE_OFFHOOK_COMMAND,
    AT_MODE_CONNECTED,
    AT_MODE_DELIVERY,
    AT_MODE_HDLC,
    AT_MODE_STUFFED
};

enum at_call_event_e
{
    AT_CALL_EVENT_ALERTING = 1,
    AT_CALL_EVENT_CONNECTED,
    AT_CALL_EVENT_ANSWERED,
    AT_CALL_EVENT_BUSY,
    AT_CALL_EVENT_NO_DIALTONE,
    AT_CALL_EVENT_NO_ANSWER,
    AT_CALL_EVENT_HANGUP
};

enum at_modem_control_operation_e
{
    AT_MODEM_CONTROL_CALL,
    AT_MODEM_CONTROL_ANSWER,
    AT_MODEM_CONTROL_HANGUP,
    AT_MODEM_CONTROL_OFFHOOK,
    AT_MODEM_CONTROL_ONHOOK,
    AT_MODEM_CONTROL_DTR,
    AT_MODEM_CONTROL_RTS,
    AT_MODEM_CONTROL_CTS,
    AT_MODEM_CONTROL_CAR,
    AT_MODEM_CONTROL_RNG,
    AT_MODEM_CONTROL_DSR,
    AT_MODEM_CONTROL_SETID,
    /* The remainder of the control functions should not get past the modem, to the
       application. */
    AT_MODEM_CONTROL_RESTART,
    AT_MODEM_CONTROL_DTE_TIMEOUT
};

enum
{
    AT_RESPONSE_CODE_OK = 0,
    AT_RESPONSE_CODE_CONNECT,
    AT_RESPONSE_CODE_RING,
    AT_RESPONSE_CODE_NO_CARRIER,
    AT_RESPONSE_CODE_ERROR,
    AT_RESPONSE_CODE_XXX,
    AT_RESPONSE_CODE_NO_DIALTONE,
    AT_RESPONSE_CODE_BUSY,
    AT_RESPONSE_CODE_NO_ANSWER,
    AT_RESPONSE_CODE_FCERROR,
    AT_RESPONSE_CODE_FRH3
};

/*!
    AT profile.
*/
typedef struct
{
    /*! TRUE if character echo is enabled */
    int echo;
    /*! TRUE if verbose reporting is enabled */
    int verbose;
    /*! TRUE if result codes are verbose */
    int result_code_format;
    /*! TRUE if pulse dialling is the default */
    int pulse_dial;
    /*! ??? */
    int double_escape;
    /*! ??? */
    int adaptive_receive;
    /*! The state of all possible S registers */
    uint8_t s_regs[100];
} at_profile_t;

/*!
    AT descriptor. This defines the working state for a single instance of
    the AT interpreter.
*/
struct at_state_s
{
    at_profile_t p;
    /*! Value set by +GCI */
    int country_of_installation;
    /*! Value set by +FIT */
    int dte_inactivity_timeout;
    /*! Value set by +FIT */
    int dte_inactivity_action;
    /*! Value set by L */
    int speaker_volume;
    /*! Value set by M */
    int speaker_mode;
    /*! This is no real DTE rate. This variable is for compatibility this serially
        connected modems. */
    /*! Value set by +IPR/+FPR */
    int dte_rate;
    /*! Value set by +ICF */
    int dte_char_format;
    /*! Value set by +ICF */
    int dte_parity;
    /*! Value set by &C */
    int rlsd_behaviour;
    /*! Value set by &D */
    int dtr_behaviour;
    /*! Value set by +FCL */
    int carrier_loss_timeout;
    /*! Value set by X */
    int result_code_mode;
    /*! Value set by +IDSR */
    int dsr_option;
    /*! Value set by +ILSD */
    int long_space_disconnect_option;
    /*! Value set by +ICLOK */
    int sync_tx_clock_source;
    /*! Value set by +EWIND */
    int rx_window;
    /*! Value set by +EWIND */
    int tx_window;
    
    int v8bis_signal;
    int v8bis_1st_message;
    int v8bis_2nd_message;
    int v8bis_sig_en;
    int v8bis_msg_en;
    int v8bis_supp_delay;

    uint8_t rx_data[256];
    int rx_data_bytes;

    int display_call_info;
    int call_info_displayed;
    char *call_date;
    char *call_time;
    char *originating_name;
    char *originating_number;
    char *originating_ani;
    char *destination_number;
    char *local_id;
    /*! The currently select FAX modem class. 0 = data modem mode. */
    int fclass_mode;
    int at_rx_mode;
    int rings_indicated;
    int do_hangup;
    int silent_dial;
    int ok_is_pending;
    int dte_is_waiting;
    /*! \brief TRUE if a carrier is presnt. Otherwise FALSE. */
    int rx_signal_present;
    /*! \brief TRUE if a modem has trained, Otherwise FALSE. */
    int rx_trained;
    int transmit;

    char line[256];
    int line_ptr;

    at_modem_control_handler_t *modem_control_handler;
    void *modem_control_user_data;
    at_tx_handler_t *at_tx_handler;
    void *at_tx_user_data;
    at_class1_handler_t *class1_handler;
    void *class1_user_data;

    /*! \brief Error and flow logging control */
    logging_state_t logging;
};

#ifdef __cplusplus
extern "C" {
#endif

void at_set_at_rx_mode(at_state_t *s, int new_mode);

void at_put_response(at_state_t *s, const char *t);

void at_put_numeric_response(at_state_t *s, int val);

void at_put_response_code(at_state_t *s, int code);

void at_reset_call_info(at_state_t *s);

/*! Set the call information for an AT interpreter.
    \brief Set the call information for an AT interpreter.
    \param s The AT interpreter context.
    \param call_date .
    \param call_time .
    \param originating_name .
    \param originating_number .
    \param originating_ani .
    \param destination_number . */
void at_set_call_info(at_state_t *s,
                      char const *call_date,
                      char const *call_time,
                      char const *originating_name,
                      char const *originating_number,
                      char const *originating_ani,
                      char const *destination_number);

void at_display_call_info(at_state_t *s);

int at_modem_control(at_state_t *s, int op, const char *num);

void at_call_event(at_state_t *s, int event);

void at_interpreter(at_state_t *s, const char *cmd, int len);

void at_set_class1_handler(at_state_t *s, at_class1_handler_t handler, void *user_data);

at_state_t *at_init(at_state_t *s,
                    at_tx_handler_t *at_tx_handler,
                    void *at_tx_user_data,
                    at_modem_control_handler_t *modem_control_handler,
                    void *modem_control_user_data);

#ifdef __cplusplus
}
#endif

#endif
/*- End of file ------------------------------------------------------------*/
