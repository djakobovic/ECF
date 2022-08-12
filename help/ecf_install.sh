#!/usr/bin/env bash

# ECF has some dependencies:
# 	- SVN - for downloading the latest ECF version
#	- the C++ Boost library
#	- MPICH2 - for using multiple processor cores

SVN="subversion"
BOOSTLIB="libboost-dev"
LIBTOOL="libtool"
AUTORECONF="dh-autoreconf"

NCORES=$(cat /proc/cpuinfo |grep -c "model name")
CURR_DIR=$(pwd)

printf "%b\n"
echo "-----------------"
echo "-----------------" 
echo "SOME INFO ABOUT YOUR SYSTEM - WHEN REPORTING A PROBLEM, INCLUDE THIS INFO"
echo "-----------------"
uname -a
cat /etc/*-release
echo "-----------------"
echo "-----------------"
printf "%b\n" 
echo $NCORES 
echo "By default ECF installs commands under /usr/local/bin, include files under /usr/local/include, etc. If other location is preffered (e.g. /home/user/ECF/), read the INSTALL file and exit this script NOW"
printf "%b\n"

# -----------------
# |UBUNTU SPECIFIC|
# -----------------
echo "ECF needs Subversion, C++ Boost library, libtool and autoreconf. Install now ("yes") or skip if they exist? (if unsure enter "yes") [y/n]:"
read DEPS
if [[ $DEPS = y* ]]; then
	sudo apt-get install $SVN $BOOSTLIB $LIBTOOL $AUTORECONF
fi
echo "Downloading latest ECF version to 'trunk/'."
svn co svn://ecf.zemris.fer.hr/ecf/trunk


cd $CURR_DIR/trunk/

printf "%b\n"
echo "Number of CPU cores detected:" $NCORES
echo "-------------------------------"
printf "%b\n"
echo "Compiling..."
autoreconf -fi
./configure
make 
make 
sudo make install

echo
echo "Script complete, try running the examples in /trunk/examples/ or see the Tutorial in /trunk/help/"

exit 1
