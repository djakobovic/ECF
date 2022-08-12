// Domagoj Jakobovic
// makroi za bezuvjetnu provjeru
// 26.09.2002

#ifndef CHECK

#define CHECKMSG(condition, text) \
if(!(condition)) {fprintf(stderr,"file: " __FILE__ "\nline: %d\nmsg:  " text "\n",__LINE__); exit(1);}
#define CHECK(condition) \
if(!(condition)) {fprintf(stderr,"Assertion failed!\nfile: " __FILE__ "\nline: %d\n" ,__LINE__); exit(1);}

#endif
