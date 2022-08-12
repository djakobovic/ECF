#include <stdio.h>
#include <string.h>
#include "benchmarkshelper.h"

/* Checks if sDir exists,
   Fatal ERROR if it fails
*/
void dirOK(char *sDir)
{
    char fileNameLoc[1024];
    FILE * f;
    createFullFileName(fileNameLoc, sDir, (char*)"toto");
    f = fopen(fileNameLoc,"w");
    if(f == NULL){
       ERROR("Problem writing in dir %s.\nMake sure you run 'python createfolders.py %s' before running the program.", sDir, sDir);
    }
    else
    {
        fclose(f);
        remove(fileNameLoc);
    }
    return;
}
