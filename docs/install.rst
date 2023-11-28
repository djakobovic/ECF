1. ECF installation / running the examples
==========================================

Installation
------------

Visual Studio
~~~~~~~~~~~~~

In the main directory you can find a "VS" subdirectory which contains
Visual Studio solution file **ECF_lib.sln.** Open it and just build the
solution (release and/or debug version). The solution produces library
*ECF.lib* (in either ./*release* or ./*debug* directory) which can be
used to build examples and user projects. Each example has its "VS"
subdirectory with a VS solution file that can be used to build either
debug or release version (having previously built the ECF_lib project). 

CMake
~~~~~

`CMake <https://cmake.org/>`__ project files ("CMakeLists.txt") are
provided in the main directory for the ECF library, and in example
subfolders for each example.

CLion
~~~~~

The ECF library and the examples can also be compiled with
`CLion <https://www.jetbrains.com/clion/>`__, for which the project
files are suplied in subdirectory .idea for each example.

Note: if you encounter CLion message "Error running 'Build': Cannot
start process, the working directory '\\cmake-build-release' does not
exist", reload the CMake project with Tools - CMake - Reload.

Autotools (Linux)
~~~~~~~~~~~~~~~~~

Standard installation procedure applies (you may need to
*chmod* the 'configure' script beforehand to be able to execute it):

::

    $ ./configure [ --prefix=DIR ]
    $ make
    $ make install

Should any errors occurr, or if *configure* is missing, try reconfiguring the package by running
aclocal, autoconf and automake (or just autoreconf -fi) before
./configure.

::

	$ autoreconf -fi

By default, the compiled libraries are installed in folder
'/usr/local/lib', while the headers are installed in
'/usr/local/include'.
You can specify a different installation folder by giving the option
'--prefix=<PATH>' to the 'configure' script, e.g.
--prefix=/home/someuser/ecf/.

Each directory in /examples contains a Makefile used to build a specific
example:

::

    $ cd examples/GAonemax
    $ make
    g++ -Wall OneMaxEvalOp.cpp -c -o OneMaxEvalOp.o
    g++ -Wall main.cpp -c -o main.o
    g++ -Wall OneMaxEvalOp.o main.o -lecf -o onemax

If necessary, modify the Makefile to include the ECF installation
library directory as the compilation flags (if not installed to the
default system location), e.g.:

::

   CFLAGS = $(WARN) -I/home/someuser/ecf/include
   LDFLAGS = $(WARN) -lecf -L/home/someuser/ecf/lib

Make sure libecf is installed and can be found by the linker (add the
above installation library directory defined with ./configure to
LD_LIBRARY_PATH if necessary, e.g. 

::

	$ export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

Each example is started by calling its name and given parameters file
(e.g. "./onemax parameters.txt"). The examples and parameters are
explained in more detail in the `Tutorial <tutorial.html>`__.

--------------
