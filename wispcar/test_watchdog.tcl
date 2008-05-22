#!/usr/local/bin/expect  -f 
# test_watchdog.tcl - Expect script to test wispcar watch dog timer.
#
# Assumes /etc//etc/minirc.mini4800.dfl config file configured for
# 4800 baud

set rs232 dragonballz
set send_slow {1 0.2}
set timeout -1

# connect to machine with RS232 port

spawn ssh $rs232
expect "$ "
send "killall -9 minicom\r"
expect "$ "
send "minicom mini4800.dfl\r"

# Keep sending 'w' every second
# This shoud mean WD _never_ times out

set fire 0
set count 0
while {1} {
    send "w"
    expect -re "(...) (...) (.) (...) (...) (...) (.) (...) (.)"
    set v $expect_out(1,string)
    set w $expect_out(9,string)
    if {$w == "w"} {
	# watchdog has fired
        set fire [expr $fire+1]
	send "w"
    }
    set count [expr $count+1]
    puts "count: $count  fire: $fire"
}

close
