#!/bin/bash -x
#
# 3-build_gqrx_gr-satellites.sh
# LilacSat-1 Rx Setup for AREG Club Project
# Mark Jessop and David Rowe
# June 2017

# Install gqrx

#sudo pybombs install gqrx 

# Install libfec, which gr-satellites needs.  This is installed outside
# of pybombs ~/prefix/default environment

#git clone https://github.com/daniestevez/libfec.git
#cd libfec && ./configure && make && sudo make install && cd ..

# Hopefully by now we have everything needed to compile gr-satellites
# Note magic cmake command to set path that cost my 3 hours of my life
# that I want back

cd ~/prefix/default/
source setup_env.sh
cd src
git clone https://github.com/daniestevez/gr-satellites.git
cd gr-satellites && mkdir build_linux && cd build_linux
cmake -DCMAKE_INSTALL_PREFIX=$PYBOMBS_PREFIX ..
make && make install
cd ..
#./compile_hierarchical.sh

# OK now ready to run everything, back to REAME.txt
