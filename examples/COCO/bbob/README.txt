This is the C version of the COCO software
    --- http://coco.gforge.inria.fr/ ---

It is meant to be used with any ANSI C compliant compiler.
It was tested using with GCC/G++ 4.4.3 (Ubuntu 4.4.3-4ubuntu5), MingW gcc 3.4.5 and Visual C++ 2010

This source files should also compile as C++ code.

As usual, this software comes with absolutely no warranty (see Warranty at end).

It is here assumed that you know what COCO/BBOB is about http://coco.gforge.inria.fr/doku.php?id=bbob-2010 and that you have read the corresponding documentation http://coco.gforge.inria.fr/

Installation
============
If you are reading this, you already succeeded in unpacking the COCO tar file :-)

After untarring the archive, the bbob.vXX.XX/c directory should contain the following files:

bbobStructures.h
benchmarksinfo.txt
benchmarks.c
benchmarksdeclare.c
benchmarksdeclare.h
benchmarks.h
benchmarkshelper.c
benchmarkshelper.h
benchmarksnoisy.c
benchmarksnoisy.h
createfolders.py               creates the output folder structure
createfolders.tar.gz           contains an empty output folder structure
dirOK.c
dirOK-linux.c
exampleexperiment.c
exampleexperiment.out
examplenoisyexperiment.out     example console output when everything goes well
exampletiming.c
fgeneric.c
Makefile
MY_OPTIMIZER.c
README.txt

At Unix prompt, simply type "make" and two executable files should be compiled:

* exampleexperiment - that launches a very quick but complete experiment using the random MY_OPTIMIZER.c optimizer (see code). The output should match the file exampleexperiment.out, set srand correctly to have the exact same figures though.

* exampletiming - that launches the timing experiment (warning, this program takes at least half a minute before the first output to console, be patient :-)

Before you can run the examples, you will need to create a folder/subfolder structure. The whole folder/subfolder structure is in the archive createfolders.tar.gz which, when untarred, outputs the folder PUT_MY_BBOB_DATA_PATH. This folder must be renamed to match the data path of your experiment. An alternative scripted way for generating the folder structure is given below (see Python folder creation).

For instructions on installing Python, please refer to the documentation: http://coco.gforge.inria.fr/

If compilation fails, or some run-time error appears when running either of those programs, get in touch immediately with bbob@lri.fr indicating all information about your system and compiler, as well as the complete error message(s).

Customization
=============
you might at some point want to try your own optimizer: this is fairly easy, assuming you know how to write C code and how to modify a 'flat' Makefile.

1- edit exampleexperiment.c and replace the strings "PUT_..." with the correct names. If you modify the 'PUT_MY_BBOB_DATA_PATH' string, you need to run the createfolder.py python script with the corresponding new string, see above.

2- write a MY_OPTIMIZER.c function (in MY_OPTIMIZER.c file) that sets up everything for your optimizer, and calls it with the correct arguments

3- in case your optimizer does not use a function pointer in its calling arguments, you can directly use the objective (fitness) function 

double fgeneric_evaluate(double *);

which remains the same for all benchmark functions. The function declaration can also be found at the end of file bbobStructures.h 

4- Modify Makefile so that all the files that are needed by your optimizer are compiled, and linked with exampleexperiment.o

You're done :-)

In case of problem, send a mail to bbob@lri.fr but please make sure first that your optimizer runs smoothly as a standalone application.
To discuss more general issues about BBOB: bbob-discuss@lists.lri.fr

Linux/Unix Users
================
To skip the step of creating the folder structure with the createfolders.py module you need to compile the dirOK-linux.c file and link to dirOK-linux.o instead of the dirOK.o object file. To do so, you need to edit the Makefile that we provide and change the setting of variable DIROK.

MS Visual C++ Users
===================
Here are the command to execute to compile the executables
From the command line:
>cl /c /TC /Wall *.c
>link benchmarks.obj benchmarksdeclare.obj benchmarkshelper.obj benchmarksnoisy.obj dirOK.obj exampleexperiment.obj fgeneric.obj MY_OPTIMIZER.obj /OUT:exampleexperiment.exe 
>link benchmarks.obj benchmarksdeclare.obj benchmarkshelper.obj benchmarksnoisy.obj dirOK.obj exampletiming.obj fgeneric.obj MY_OPTIMIZER.obj /OUT:exampletiming.exe

The first line compiles the *.c files as C source code. Compiling dirOK-linux.c WILL generate an error (obviously). To compile the code as C++ source code, you need to use the /TP option instead of /TC.
The seconde and third lines link the object files to output the executables exampleexperiment.exe and exampletiming.exe

Python folder creation
======================
Before you can run the examples, you will need to create a folder/subfolder structure. The python module createfolders is provided to generate the whole folder structure necessary for fgeneric to work. To generate folder PUT_MY_BBOB_DATA_PATH (of course any other name can be used, if you modify exampleexperiment.c accordingly) and the necessary folder structure in the current working directory, type from a command-line: 
    python createfolders.py PUT_MY_BBOB_DATA_PATH
or from the python command-line use the commands:
 >>> import createfolders
 >>> createfolders.main("PUT_MY_BBOB_DATA_PATH")

Acknowledgments
===============
The BBOBies would like to acknowledge Miguel Nicolau who contributed by testing this version of the code.
We would also like to acknowledge Alvaro Fialho who helped transposing the C-version to the C++ language.

Warranty
========
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL ANY CONTRIBUTOR TO THIS SOFTWARE BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

