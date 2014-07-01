(paper-size 11.5 8.5)
(load (string-append gedadatarc "/gschem-lightbg")) ; light background
(output-type "limits")
(output-color "enabled")

(text-color 9 "green4" "green4" "0 0 0" 0 0 0)	    ; light background
(net-endpoint-color 2 "red" "red3" ".502 .094 .062" 128 24 16)	; light
(graphic-color 3 "green4" "green4" "0 0 0" 0 0 0)	; light background
(net-color 4 "blue2" "blue3" "0 0 0" 0 0 0)		; light background

(output-text "ps")
(gschem-use-rc-values)         ;You need call this after you call any
(gschem-print "dummyfilename") ;filename is specified on the command line
(gschem-exit)
