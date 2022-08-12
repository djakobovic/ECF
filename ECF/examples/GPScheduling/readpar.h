/* Razred za citanje parametara iz tekst datoteke */
/* Class for reading parameters from a text file */
/* last update 14.02.01. */

#include<stdio.h>

//#define STRING 0
//#define INTEGER 1
//#define DOUBLE 2
#define TAB 9
#define SPACE 32
#define NEWLINE '\n'

const int MAX_LINE = 10000;

class ReadPar
{	public:
		static const int STRING = 0;
		static const int INTEGER = 1;
		static const int DOUBLE = 2;
		ReadPar(void);
		~ReadPar(void);

		int OpenFile(const char *fname);
		int CloseFile(void);
		int ReadParameter(const char *par, int type, void *val, int vector=1, int item=1);	
		// type: 0 - string, 1 - integer, 2 - double
		// vector: 1-... number of elements

	private:
		FILE *fp;
		char buf[MAX_LINE];
};

