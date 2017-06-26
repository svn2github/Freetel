#!/bin/bash
nc -lu 7000 | c2dec 1300 - - | play -t raw -r 8000 -e signed-integer -b 16 -c 1 -

