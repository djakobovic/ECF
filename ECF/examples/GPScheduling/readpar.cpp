//////////////////////////////////////////////////////////////////////////////
//
// Class for reading parameters from text files
// 2001/02/15
//
// Domagoj Jakobovic
// FEEC, Zagreb
// http://www.zemris.fer.hr/~yeti
//
//////////////////////////////////////////////////////////////////////////////

#include"readpar.h"
#include<string.h>

ReadPar::ReadPar(void)
{
}

ReadPar::~ReadPar(void)
{
}
		
int ReadPar::OpenFile(const char *fname)
{	if((fp=fopen(fname,"r"))==NULL)
		return 0;
	return 1;
} 

int ReadPar::CloseFile(void)
{	fclose(fp);
	return 1;
}

// povratne vrijednosti:
// 0  - nije nadjen parametar
// -1 - nadjen ali bez vrijednosti
// 1  - nadjen i procitana vrijednost
int ReadPar::ReadParameter(const char *par, int type, void *val, int vector, int item)
{	int pos=0, *ip=(int*)val, len = (int) strlen(par);
	double *dp=(double*)val;
	char *cp=(char*)val;
	rewind(fp);
	while(fgets(buf,MAX_LINE-1,fp)!=NULL && item>0)
	{	if(!strncmp(par, buf, len) && (buf[len]==TAB || buf[len]==SPACE || buf[len]==NEWLINE))
		//if(!strncmp(par, buf, len))
		{	if(item==1)
				for(int i=0;i<vector;i++)
				{	while(buf[pos]==SPACE || buf[pos]==TAB)		// postavljanje na slijedecu prazninu
						pos++;		
					while(buf[pos]!=SPACE && buf[pos]!=TAB)
						pos++;
					switch(type)
					{	case 0:
							if(sscanf(buf+pos,"%s",cp)==EOF) return -1;	// nije procitana vrijednost
							cp+=strlen(cp); break;
						case 1:
							if(sscanf(buf+pos,"%d",ip)==EOF) return -1;
							ip++; break;
						case 2:
							if(sscanf(buf+pos,"%lf",dp)==EOF) return -1;
							dp++; break;
					}
				}
			item--;
		}
	}
	if(pos==0) return 0;
	else return 1;
}