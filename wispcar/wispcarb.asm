; wispcarb.asm
; David Rowe June 2009
;
; WISPCAR modified for EV battery testing.  When watchdog fires PowerSwitch
; (pin 6) stays off until the host sends a "w".  This switches off current
; to the battery under test if the host side dies.  This mod is just a hack
; for now.
;
;
; PIC12F510 Assembler program, MPLAB V8.10 IDE used to build.
;
; ********************************************************
; * Wifi Station Power Controller And Reporter - WiSPCaR *
; ********************************************************
;
;---------------------------------------------------------------------
; Copyright (C) 2008 David Rowe
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 2 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
; USA
;---------------------------------------------------------------------
;
; FUNCTIONS
; =========
;
; 1/ Monitor Voltage
; 2/ Monitor Current
; 3/ Watchdog timer
; 4/ Sleep timer
;
;
; PINOUT
; ======
;                   ____  ____
;                  |    \/    |   
;            Vcc --| 1      8 |-- GND
;                  |          |
;      Spare I/O --| 2      7 |-- Vsense (in)
;                  |          |
; RS232 Tx (out) --| 3      6 |-- PowerSwitch (out)
;                  |          |
;  RS232 Rx (in) --| 4      5 |-- Isense (in)
;                  |__________|
;
;
;             Pin | Function    | PIC Name
;            -----|-------------|---------
;              2  | Spare I/O   |  GP5 
;              3  | RS232 Tx    |  GP4
;              4  | RS232 Rx    |  GP3
;              5  | Isense      |  AN2
;              6  | PowerSwitch |  GP1
;              7  | Vsense      |  AN0
;
; 
; PIC PROGRAMMING OPTIONS
; =======================
;
; 8MHz clock, MCLRE=I/O, INTOSC
;
;
; 4800 BAUD RS232 OUTPUT
; ======================
; 
; The following line is printed every time you send 'w' or '.', if you
; send nothing to wispcar it will remain silent.  The specific chars
; minimise the chance of wispcar interfering with your boot loader.
;
; Name of script on host that parses this line
; |       Vsense
; |       |   Isense
; |       |   |   char received (ASCII)
; |       |   |   | Last char received (decimal)
; |       |   |   | |   Watchdog timeout counter
; |       |   |   | |   |   Watchdog fire counter
; |       |   |   | |   |   |   Sleep state machine state
; |       |   |   | |   |   |   | Sleep timeout counter
; |       |   |   | |   |   |   | |    
; wispcar 040 031 - 032 012 000 0 000 w---reason for last restart
;                                     b - Wispcar (re)booted  
;                                     w - watchdog timer fired 
;                                     s - we went to sleep
;                                     - - restart flag reset
;
; COMMANDS
; ========
;
; 1/ Send a 'w' every WD_TIMEOUT seconds to prevent Watchdog timer
; firing and cutting power.
;
; 2/ Send a '.' to report status without resetting watchdog.
;
; 3/ Send the string 'sleepxyz' to cut the power for xyz seconds,
; where x,y and z are digits in the range 0-9. If any part of the
; string is invalid it will be ignored.  Wait 5 seconds and start
; again.
;
; 4/ Wispcar cannot rx and tx RS232 at the same time.  Wait until after
; the current output has finished to send the next command. It is best to
; wait until just after the output line is received by the host, then
; send your command.  If the command (such as sleep) gets messed up,
; just wait 5 seconds and wispcar will reset it's internal state
; machine.  You can then try again.
;
; 4/ The final field is reset to a space when a 'w' is sent to
; Wispcar. This field lets the host know just _why_ it was rebooted.


#include <p12f510.inc>

; VARIABLES ---------------------------------------------------------------------

TEMP		equ	0x10
SERBUF		equ	0x11	; RS232 char to transmit
count		equ	0x12    ; temp reg used by "BIN2BCD" and "baud" routines
BIN		equ	0x13	
huns		equ	0x14
tens		equ	0x15
ones		equ	0x16
thirty		equ	0x17	; constant set to (guess what) 30
SERBUFI		equ	0x18	; last RS232 character we received
wd_timeout	equ	0x19	; current watchdog timer value (in secs)
				; when this hits zero we turn the power off
wd_fire		equ	0x1a    ; count down timer when watchdog fires (in secs)
restart_flag	equ     0x1b    ; tells host why we just restarted (WD or sleep)

sleep_state	equ	0x1c
sleep_timeout	equ	0x1d    ; current value of sleep time out (in secs)
				; measures time between rx-ed chars in
				; s-l-e-e-p-x-y-z sequence.
				; If this hits zero (like s-l- (BIG GAP))
				; we assume sleep command
				; was invalid and we reset the input state machine

				; three digit sleep time (up to 999 secs)
				; these count down to zero while we are sleeping
				; when count down hits 0 power is switched back on

sleeping	equ	0x1e	; asserted if we are sleeping (power off)

; following variables are in bank1, which makes life difficult....

d1		equ	0x10
d2		equ	0x11
d3		equ	0x12

x		equ	0x13	; sleep time (hundreds of secs)
y		equ	0x14	; sleep time (tens of secs)
z		equ	0x15	; sleep time (secs)

; CONSTANTS ---------------------------------------------------------------------

WD_TIMEOUT	equ	d'18'	; watchdog timeout in seconds
WD_FIRE		equ	d'5'	; watchdog firing time in seconds
				; how long power is cut off to host

SLEEP_TIMEOUT   equ     d'5'	; timeout between rx-ed chars in sleep input
				; sequence.

; Values for restart_flag - tell host why we just cut power after we woke up

BOOT_FLAG	equ	'b'	; Wispcar just rebooted
WD_FLAG		equ	'w'	; Watchdog fired
SLEEP_FLAG	equ	's'	; We slept

; START PROGRAM ----------------------------------------------------------------

; set up osc cal (temp code just for experimenting)
; TODO - FIX ME for general case

        movlw   16
	movwf   OSCCAL

; configure AN[1:0] as analog inputs

	movlw	b'10110001'
	movwf	ADCON0	

; configure GP[3] as digital input   (RS232 RX)
; configure GP[4] as digital output  (RS232 TX)
; configure GP[1] as digital output  (PowerSwitch)

; disble the comparitor so we can use GP[1]

	bcf	CM1CON0,3

	; set up GPIO directions

	movlw	b'00001101'
	tris	GPIO

; turn on PowerSwitch 

	bsf	GPIO,1	

; set up a convenient constant

        movlw	h'30'
	movwf	thirty

; init watchdog 

	movlw	WD_TIMEOUT
	movwf	wd_timeout
	clrf	wd_fire

; init	sleep

	clrf	sleep_state
	clrf	sleeping  
	clrf	sleep_timeout  
	bsf	FSR,5	       ; x,y,z are in bank 1 (grumble)
	clrf	x
	clrf	y
	clrf	z
	bcf	FSR,5	       ; x,y,z are in bank 1 (grumble)

; init restart_flag

       movlw	BOOT_FLAG
       movwf	restart_flag

       goto	main

; START FUNCTIONS ------------------------------------------------------------

; BIN2BCD
;
;   Convert 8 bit value in BIN to a 3 digit decimal value.
;
;   in.......: BIN
;   out......: huns, tens, ones
;   Reference: http://www.piclist.com/techref/microchip/math/radix/b2a-8b3d-ab.htm
;
;   Uses temp reg 'count'

; uses ADD-3 algorithm

BIN2BCD
	movlw 8
	movwf count
	clrf huns
	clrf tens
	clrf ones
BCDADD3
	movlw 5
	subwf huns, 0
	btfsc STATUS, C
	CALL ADD3HUNS

	movlw 5
	subwf tens, 0
	btfsc STATUS, C
	CALL ADD3TENS

	movlw 5
	subwf ones, 0
	btfsc STATUS, C
	CALL ADD3ONES

	decf count, 1
	bcf STATUS, C
	rlf BIN, 1
	rlf ones, 1
	btfsc ones,4 ; 
	CALL CARRYONES
	rlf tens, 1

	btfsc tens,4 ; 
	CALL CARRYTENS
	rlf huns,1
	bcf STATUS, C

	movf count, 0
	btfss STATUS, Z
	GOTO BCDADD3

	movf huns, 0    ; add ASCII Offset
	addwf thirty, 0
	movwf huns

	movf tens, 0    ; add ASCII Offset
	addwf thirty, 0
	movwf tens

	movf ones, 0    ; add ASCII Offset
	addwf thirty, 0
	movwf ones

	retlw 0

ADD3HUNS
	movlw 3
	addwf huns,1
	retlw 0

ADD3TENS
	movlw 3
	addwf tens,1
	retlw 0

ADD3ONES
	movlw 3
	addwf ones,1
	retlw 0

CARRYONES
	bcf ones, 4
	bsf STATUS, C
	retlw 0

CARRYTENS
	bcf tens, 4
	bsf STATUS, C
	retlw 0	

; INCH_N is from "PIC Software UART Routines"
; John Massa, Datadog Systems (C) 2005

; ********************************************************************
;                            INCH_N
; THIS ROUTINE INPUTS RS232 DATA USING AN INVERTER, LIKE THE MAX232.
; THIS ROUTINE USES A 8-DATA BIT PER CHARACTER PROTOCOL
; GPIO,0 = RX (MARK = 1, SPACE = 0).
; TO RECIEVE A CHARACTER, CALL inch_n, THE RECEIVED CHARACTER IS PLACED
; IN THE REG 'W' AND IN THE REG 'SERBUF'.
; THE RECEIVED CHARACTER WILL ECHO IF 'RETLW   0' IS REM-ED OUT.
; VARIABLES USED: REG 'TEMP' AND REG 'SERBUF' BOTH VARIABLES ARE
;                 SHARED WITH THE 'outch_n' ROUTINE.
; ROUTINES CALLED: 'half_baud'AND 'baud' TO SET THE BAUD-RATE
; ********************************************************************

; Modified by David Rowe May 2008 to combine a 1 second delay with
; character reception.

inch_n_delay

	; clear previous rx char

	movlw	'-'
	movwf	SERBUFI

	; init delay loop for 1999996 cycles

	bsf     FSR,5	      ; d1, d2, d3 in Bank 1 (groan)
	movlw	0x11
	movwf	d1
	movlw	0x5D
	movwf	d2
	movlw	0x05
	movwf	d3

wait_for_start
	bcf     FSR,5	      ; just in case goto below fires
        btfss   GPIO,3          ; SKIP UNLESS WE GET A START BIT = 1 (A "MARK")
        goto    start_serial    ; start bit - process character

	; process delay loop while we look for start bit

	bsf     FSR,5
	decfsz	d1, f
	goto	$+2
	decfsz	d2, f
	goto	$+2
	decfsz	d3, f
	goto	wait_for_start

	; delay finished

	bcf	FSR,5	
	retlw	0
 
start_serial
	bsf	GPIO,5
	bcf	GPIO,5

        movlw   8               ; START SERIAL INPUT SEQUENCE
        movwf   TEMP            ; COLLECT 8 DATA BITS
        clrf    SERBUFI         ; CLEAR SERIAL CHARACTER BUFFER
        call    half_baud       ; DELAY FOR ONE HALF BAUD TIME
        btfsc   GPIO,3          ; FALL THRU IF START BIT STILL = 1 (A "MARK")
        goto    wait_for_start  ; ELSE IT WAS JUST A NOISE SPIKE, KEEP LOOKING
inch_n1
        call    baud            ; DELAY ONE BAUD-BIT TIME ( = 1/BAUD-RATE)
        bcf     STATUS,0        ; CLEAR THE CARRY BIT
        rrf     SERBUFI,F       ; ROTATE CRY -> MSB, ROTATE MSB RIGHT
        btfsc   GPIO,3          ; IS IT A "MARK" ?
        bsf     SERBUFI,7       ; ...SKIP IF YES, ELSE SET BIT TO LOGIC '1'
        decfsz	TEMP,F          ; EIGHT COUNTS YET?
        goto    inch_n1         ; ...NO, GET ANOTHER BIT
        call    baud            ; DELAY FOR THE STOP BIT
        movf    SERBUFI,W       ; PUT THE RECEIVED CHARACTER IN REG 'W'

	; process received character, this can be considered an "event
	; handler" for rx-chars.  We expect either a 'w' to reset the
	; watchdog or a tightly defined sequence of sleep characters.

	movlw	'w'		
	subwf	SERBUFI,0	; is it a 'w' for watchdog reset?
	btfss	STATUS,Z	; skip if received char matches
	goto	handle_sleepin	; only process sleep input if not a 'w'

	; handle 'w' input 
	
	; Note if we are sleeping or WD is firing we will keep power
	; off and receiving 'w' will have no effect.  Unlikely unless
	; device sending rx chars is powered indepednantly (ie during
	; testing).

	movlw	WD_TIMEOUT	; reset watchdog timer
	movwf	wd_timeout

	; DR June 2009 - mods
	bsf		GPIO,1

	movlw	'-'		; clear reboot flag
        movwf	restart_flag

	clrf	sleep_state     ; reset sleep state machine for good measure
	clrf	sleep_timeout   ; just in case we get s-l-w-e-e-p or something

	goto	wait_for_start

handle_sleepin
	call	sleep_input
	goto	wait_for_start

; OUTCH_N and BAUD are from "PIC Software UART Routines"
; John Massa, Datadog Systems (C) 2005

;*********************************************************************
;                               OUTCH_N
; THIS ROUTINE OUTPUTS RS232 DATA THROUGH AN INVERTER.
; THIS ROUTINE USES AN 8-DATA BIT PER CHARACTER PROTOCOL.
; TO PRINT A CHARACTER, LOAD BYTE INTO REG 'W' AND CALL outch_n.
; GPIO,1 = TX (MARK = 1, SPACE = 0)   ; USE INVERTER ON THE OUTPUT
; VARIABLES USED: REG 'TEMP' AND SHARE REG 'SERBUF' WITH THE ROUTINE
; 'inch_n'
; CALLS THE ROUTINE 'baud' TO SET THE BAUD-RATE TIMING.
;*********************************************************************

outch_n                         ; THIS ROUTINE USES 8 DATA BITS
        movwf   SERBUF          ; SERBUF CONTAINS CHARACTER TO XMT
        movlw   8               ; THE CHARACTER HAS 8 BITS
        movwf   TEMP
        bcf     GPIO,4          ; SET START-BIT TO A "SPACE"
        call    baud            ; WAIT ONE BAUD TIME
outch_n1
        rrf     SERBUF,F        ; ROTATE THE FIRST BIT INTO CARRY
        btfss   STATUS,0        ; TEST THE CARRY BIT
        bcf     GPIO,4          ; IF BIT IS 0 SET OUTPUT PIN TO A "0" (SPACE)
        btfsc   STATUS,0        ; TEST THE CARRY BIT AGAIN
        bsf     GPIO,4          ; IF BIT IS 1 SET OUTPUT PIN TO A "1" (MARK)
        call    baud            ; ONE BAUD-BIT DELAY
        decfsz TEMP,F           ; IF COUNT IS ZERO THEN XMIT A STOP BIT
        goto    outch_n1        ; ...ELSE XMIT NEXT BIT
        rrf     SERBUF,F        ; ROTATE CARRY, GET THE MSB BACK INTO BIT 7
        bsf     GPIO,4          ; SET PIN TO A 1 (A "MARK") FOR THE STOP BIT
        call    baud            ; FIRST BAUD-BIT DELAY
        call    baud            ; SECOND BAUD-BIT DELAY
        retlw   0               ; RETURN WITH THE CHARACTER IN SERBUF
	
; ********************************************************************
;                       BAUD ROUTINE @ 4 MHz
; BAUD RATE:      CONSTANT:
;   1200 Baud     D'137'
;   2400 Baud     D'68'
;   4800 Baud     D'34'
;   9600 Baud     D'16'
; 19200 Baud      D'8'
; 38400 Baud and up - use 'NOP' delays
; VARIABLES USED: REG 'count'
; ROUTINES CALLED: NONE
; ********************************************************************

baud                            ; AT 2400 BAUD THE PERIOD IS 416.6 US
                                ; CLK = 4MHz
        movlw   D'68'           ; 1 US   (BAUD RATE CONSTANT)
        movwf   count           ; 1 US
baud1
        decfsz  count,F         ; 1 US (+ 1 US MORE IF SKIP)
        goto    baud1           ; 2 US
                                ; FALL THRU...AFTER 1+1+3x68+1 = 207 US
half_baud
        movlw   D'68'           ; 1 US
        movwf   count           ; 1 US
hbaud1
        decfsz  count,F         ; 1 US (+ 1 US MORE IF SKIP)
        goto    hbaud1          ; 2 US
        retlw   0               ; ...AFTER 1+1+3x68+1 = 207 US (X2=414 US)

; http://www.piclist.com/techref/piclist/codegen/delay.htm
; Delay = 1 seconds
; Clock frequency = 8 MHz

; Actual delay = 1 seconds = 2000000 cycles
; Error = 0 %

delay
			;1999996 cycles
	bsf	FSR,5
	movlw	0x11
	movwf	d1
	movlw	0x5D
	movwf	d2
	movlw	0x05
	movwf	d3
Delay_0
	decfsz	d1, f
	goto	$+2
	decfsz	d2, f
	goto	$+2
	decfsz	d3, f
	goto	Delay_0

			;4 cycles
	goto	$+1
	goto	$+1

	bcf	FSR,5
	retlw	0

; sleep input state machine
; 
; we expect the chars s-l-e-e-p-x-y-z where xyz are digits
; if wrong char received reset
; if 5 sec timout between chars reset
; if xyz are not digits reset

sleep_input

	; big case statement to work out what state we are in

	movlw	0
	subwf	sleep_state,0
	btfsc	STATUS,Z	; Z set if in state 's'
	goto	sleep_state_s
	movlw	1
	subwf	sleep_state,0
	btfsc	STATUS,Z	; Z set if in state 'l'
	goto	sleep_state_l
	movlw	2
	subwf	sleep_state,0
	btfsc	STATUS,Z	; Z set if in state 'e1'
	goto	sleep_state_e1
	movlw	3
	subwf	sleep_state,0
	btfsc	STATUS,Z	; Z set if in state 'e2'
	goto	sleep_state_e2
	movlw	4
	subwf	sleep_state,0
	btfsc	STATUS,Z	; Z set if in state 'p'
	goto	sleep_state_p
	movlw	5
	subwf	sleep_state,0
	btfsc	STATUS,Z	; Z set if in state 'x'
	goto	sleep_state_x
	movlw	6
	subwf	sleep_state,0
	btfsc	STATUS,Z	; Z set if in state 'y'
	goto	sleep_state_y
	movlw	7
	subwf	sleep_state,0
	btfsc	STATUS,Z	; Z set if in state 'z'
	goto	sleep_state_z

	; should never get here but just in case

	goto	sleep_reset

	; if we have rx-ed a 's' continue to next state
sleep_state_s
	movlw	's'
	subwf	SERBUFI,0
	btfss	STATUS,Z	; Z set if we received a 's'
	goto	sleep_reset
	goto	sleep_cont

	; if we have rx-ed a 'l' continue to next state
sleep_state_l
	movlw	'l'
	subwf	SERBUFI,0
	btfss	STATUS,Z	; Z set if we received a 'l'
	goto	sleep_reset
	goto	sleep_cont

	; if we have rx-ed a 'e' continue to next state
sleep_state_e1
	movlw	'e'
	subwf	SERBUFI,0
	btfss	STATUS,Z	; Z set if we received a 'e'
	goto	sleep_reset
	goto	sleep_cont

	; if we have rx-ed a 'e' continue to next state
sleep_state_e2
	movlw	'e'
	subwf	SERBUFI,0
	btfss	STATUS,Z	; Z set if we received a 'e'
	goto	sleep_reset
	goto	sleep_cont

	; if we have rx-ed a 'p' continue to next state
sleep_state_p
	movlw	'p'
	subwf	SERBUFI,0
	btfss	STATUS,Z	; Z set if we received a 'p'
	goto	sleep_reset
	goto	sleep_cont

	; if we have rx-ed a digit 0-9 continue to next state
sleep_state_x

	; check if >= '0'
	movlw	'0'
	subwf	SERBUFI,0
	btfss	STATUS,C	; C set if valid
	goto	sleep_reset

	; check if x <= 9
	bsf	FSR,5		; x is in bank 1
	movwf	x
	movlw	d'10'
	subwf	x,0
	bcf	FSR,5
	btfsc	STATUS,C	; C reset if valid
	goto	sleep_reset
	goto	sleep_cont

	; if we have rx-ed a digit 0-9 continue to next state
sleep_state_y

	; check if >= '0'
	movlw	'0'
	subwf	SERBUFI,0
	btfss	STATUS,C	; C set if valid
	goto	sleep_reset

	; check if y <= 9
	bsf	FSR,5		; y is in bank 1
	movwf	y
	movlw	d'10'
	subwf	y,0
	bcf	FSR,5
	btfsc	STATUS,C	; C reset if valid
	goto	sleep_reset
	goto	sleep_cont

	; if we have rx-ed a digit 0-9 continue to next state
sleep_state_z

	; check if >= '0'
	movlw	'0'
	subwf	SERBUFI,0
	btfss	STATUS,C	; C set if valid
	goto	sleep_reset

	; check if z <= 9
	bsf	FSR,5		; z is in bank 1
	movwf	z
	movlw	d'10'
	subwf	z,0
	bcf	FSR,5
	btfsc	STATUS,C	; C reset if valid
	goto	sleep_reset

	; OK we made it! Lets go to SLEEP, ZZZZZzzzzzzzzzz

	clrf	sleep_state
	clrf	sleep_timeout
	movlw	1
	movwf	sleeping
	bcf     GPIO,1		; power switch off

	retlw	0

	; we bombed out in rx-ing sleep string
sleep_reset
	clrf	sleep_state
	clrf	sleep_timeout
	retlw	0
	
	; increment to next state, and reset sleep timeout
sleep_cont
	incf	sleep_state,1
	movlw	SLEEP_TIMEOUT
	movwf	sleep_timeout
	retlw	0

; START MAIN LOOP -----------------------------------------------

main	
	call	inch_n_delay	; RS232 RX and 1 second delay

	; sample V, I, and print if we received a 'w' or a ' '

	movlw	'w'
	subwf	SERBUFI,0
	btfsc	STATUS,Z	; skip if SERBUFI != 'w'
	goto	sample	

	movlw	'.'
	subwf	SERBUFI,0
	btfss	STATUS,Z	; skip if SERBUFI == ' '
	goto	no_print	

	; sample AN[0] (voltage)
sample
	movlw	b'10110001'
	movwf	ADCON0	
	bsf	ADCON0,1	; start A/D conversion
wait_adc_v
	btfsc	ADCON0,1	; skip if A/D conversion finished
	goto	wait_adc_v

	; print 'wispcar' to get host to call script called wispcar.
	; This script can then parse wispcar output

	movlw   'w'
 	call	outch_n
	movlw   'i'
 	call	outch_n
	movlw   's'
 	call	outch_n
	movlw   'p'
 	call	outch_n
	movlw   'c'
 	call	outch_n
	movlw   'a'
 	call	outch_n
	movlw   'r'
 	call	outch_n
	movlw   ' '
 	call	outch_n

	; print voltage to RS232

	movf	ADRES, 0
	movwf	BIN
	call	BIN2BCD         ; convert voltage to decimal ASCII 

	movf	huns, 0		
	call	outch_n
	movf	tens, 0
	call	outch_n
	movf	ones, 0
	call	outch_n

	; print space

	movlw   ' '
 	call	outch_n

	; sample AN[1] (current)

	movlw	b'10111001'
	movwf	ADCON0	
	bsf	ADCON0,1	; start A/D conversion
wait_adc_i
	btfsc	ADCON0,1	; skip if A/D conversion finished
	goto	wait_adc_i

	; print current to RS232

	movf	ADRES, 0
	movwf	BIN
	call	BIN2BCD         ; convert current to decimal & ASCII 

	movf	huns, 0		
	call	outch_n
	movf	tens, 0
	call	outch_n
	movf	ones, 0
	call	outch_n

	; print rx char

	movlw   ' '
	call	outch_n
	movf    SERBUFI, 0
	call	outch_n

	; print rx char in dec

	movlw   ' '
	call	outch_n
	movf	SERBUFI, 0
	movwf	BIN
	call	BIN2BCD         ; convert rx char to decimal & ASCII 

	movf	huns, 0		
	call	outch_n
	movf	tens, 0
	call	outch_n
	movf	ones, 0
	call	outch_n

	; print wd_timeout in dec

	movlw   ' '
	call	outch_n
	movf	wd_timeout, 0
	movwf	BIN
	call	BIN2BCD         

	movf	huns, 0		
	call	outch_n
	movf	tens, 0
	call	outch_n
	movf	ones, 0
	call	outch_n

	; print wd_fire in dec

	movlw   ' '
	call	outch_n
	movf	wd_fire, 0
	movwf	BIN
	call	BIN2BCD         

	movf	huns, 0		
	call	outch_n
	movf	tens, 0
	call	outch_n
	movf	ones, 0
	call	outch_n

	; print sleep_state 

	movlw   ' '
	call	outch_n
	movlw	'0'		
	addwf	sleep_state,0
	call	outch_n

	; print sleep timer in dec

	movlw   ' '
	call	outch_n
	movlw	'0'	; print x
	bsf	FSR,5
	addwf	x,0
	bcf	FSR,5
	call	outch_n
	movlw	'0'	; print y
	bsf	FSR,5
	addwf	y,0
	bcf	FSR,5
	call	outch_n
	movlw	'0'	; print z
	bsf	FSR,5
	addwf	z,0
	bcf	FSR,5
	call	outch_n

	; print restart_flag

	movlw   ' '
	call	outch_n
	movf    restart_flag, 0
	call	outch_n

	; print CR-LF

	movlw   '\r'
	call	outch_n
	movlw   '\n'
	call	outch_n

no_print
	; has watchdog fired (wd_timeout == 0) ?

	movf    wd_timeout,1
	btfsc	STATUS,Z	; skip if not fired
	goto	watchdog_firing	; Yes? Then goto watchdog firing code
	
	; are we sleeping (sleeping == 1)?

	movf    sleeping,1
	btfss	STATUS,Z	; skip if not sleeping
	goto	snooze		; Yes? Then goto sleeping code

	; No? Then count down watchdog

	movlw	1
	subwf	wd_timeout,1

	; Has watchdog just fired (wd_timeout == 0)?

	btfss	STATUS,Z	; skip if wd_countdown is zero
	goto	dec_sleeptimeout ; No - continue

	; Yes - start watchdog firing code

	bcf	GPIO,1		; PowerSwitch Off
	movlw	WD_FIRE
	movwf	wd_fire
	goto	main

dec_sleeptimeout
	; has sleep state machine accepted a rx char?

	movf    sleep_state,1
	btfsc	STATUS,Z	
	goto	main		; No? Then continue processing

	decfsz	sleep_timeout,1
	goto	main

	; sleep state machine timeout, reset state machine
	clrf	sleep_state
	goto	main

	; watchdog firing code

watchdog_firing

	movlw	1

	; DR June 2009 - make watchdog fire forerver, cutting power
	; from battery indefinately until host returns
	subwf	wd_fire,0

	; watchdog firing complete? (wd_fire == 0)?

	btfss	STATUS,Z	; skip if wd_fire is zero
	goto	main            ; No - continue

	; Yes - reset watchdog

	bsf	GPIO,1		; PowerSwitch On
	movlw	WD_TIMEOUT
	movwf	wd_timeout

	; Indicate WD has fired since last 'w' sent by host.  A rx of
	; 'w' resets this flag.
	
	movlw	WD_FLAG
	movwf	restart_flag
	
	goto	main

	; sleeping code
	; sleep for xyz seconds, need to count down in decimal
snooze
	; 3 digit decimal count down timer xyz, e.g. 999, 998,...001,000
	; it's time's like these I wonder why I volunteered for this project!! :-)
	; see countdown.c for C version of this algorithm

	bsf	FSR,5
	movf	z,1
	btfss	STATUS,Z
	goto	z_not_zero
	movf	y,1
	btfss	STATUS,Z
	goto	y_not_zero
	movf	x,1
	btfss	STATUS,Z
	goto	x_not_zero
	
	; x=y=z=0 so we are finished

	bcf	FSR,5
	goto sleep_finished

z_not_zero
	decf	z,1
	goto	continue_sleep

y_not_zero
	decf	y,1
	movlw	9
	movwf	z
	goto	continue_sleep

x_not_zero
	decf	x,1
	movlw	9
	movwf	y
	movwf	z
	goto	continue_sleep

	; sleep finished

sleep_finished
	bsf	GPIO,1		; PowerSwitch On
	movlw	WD_TIMEOUT	; reset watchdog for good measure
	movwf	wd_timeout
	clrf	wd_fire
	clrf	sleep_timeout   ; clear sleep timeout counter
	clrf	sleeping	; clear sleep flag
	
	; Indicate we have just woken up from a sleep. A rx of 'w'
	; resets this.
	
	movlw	SLEEP_FLAG	
	movwf	restart_flag
	goto	main
	
continue_sleep
	bcf	FSR,5
	goto	main
	
; END MAIN LOOP -----------------------------------------------

        end


