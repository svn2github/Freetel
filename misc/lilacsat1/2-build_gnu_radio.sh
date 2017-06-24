#!/bin/sh
# 2-build_gnu_radio.sh
#
# LilacSat-1 Rx Setup for AREG Club Project
# Mark Jessop and David Rowe
# June 2017

# Setup pybombs, which handles the gnuradio installation

sudo easy_install -U pip
sudo pip install construct pybombs

# Building gnuradio - takes some time

cd ~
mkdir prefix
pybombs recipes add gr-recipes git+https://github.com/gnuradio/gr-recipes.git
pybombs recipes add gr-etcetera git+https://github.com/gnuradio/gr-etcetera.git
pybombs prefix init -a default prefix/default/ -R gnuradio-default

