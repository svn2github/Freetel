# Makefile for Wispcar schematics
# Generates sch PDF
# Derived from the BlackfinOne Makefile - thanks Ivan

SCH_FILES = wispcarb
PROJECT = wispcarb
REV     := $(shell svn info | grep Revision | sed 's/Revision: //')

SCHPS     = $(addsuffix .ps,$(SCH_FILES))
SCHPDF    = $(addsuffix .sch.ps,$(SCH_FILES))
PWD       = $(shell pwd)

PCB_PRINT_OPTIONS = --ps-color --media A4

all: schpdf 

%.ps: %.sch
	gschem -o $@ -s gschem-print.scm $^

schpdf: $(SCHPS)
	-rm *_sch.ps
	# sed stuff enlarges gschem font which is too small for my eyes :-)
	cat $(SCHPS) | sed 's/scalefont/1.5 mul scalefont/' > $(PROJECT)_sch.ps
	ps2pdf $(PROJECT)_sch.ps $(PROJECT)_sch.pdf

