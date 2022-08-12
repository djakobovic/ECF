#include <ecf/ECF.h>
#include "../../cartesian/Cartesian.h"
#include "CGPEvalOp.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <fstream>

void anf (int *, int);							//samo anf
int anf_ai (int *, int);						//vraca ai
int anf_ai_deg (int *, int);					//vraca ai+deg
void wt (int *, int);							//samo walsh
int wt_nl (int *, int);							//vraca nl
int wt_nl_ci (int *, int);						//vraca nl+ci
int wt_spektar (int *, int);					//vraca Walshov spektar
int wt_nl_ci_spektar (int *, int);				//vraca nl+ci+spektar
void autokorelacija (int *, int);				//prikaz spektra autokorelacije
int autokorelacija_rf (int *, int);				//izracun sume kvadrata indikatora
int autokorelacija_max (int *, int);			//izracun max vrijednosti autokorelacijskog spektra
int autokorelacija_rf_pc_max (int *, int );		//izracun sume kvadrata indikatora + max vrijednsoti spekta + karakteristike propagacije

int karakteristikaPropagacije (int *, int);
int sumaKvadrataIndikator (int *, int);
int AC (int *, int);
int walshSpectrum2 (int *, int);
int algebarskiStupanj (int *, int);
int balans (int *, int);
int nelinearnost (int, int);
int predrasudaNelinearnost (int, int);
int korelacijskiImunitet (int *, int);
int hammingWeight (int);
int choose(int, int);
int preceq(int, int);
int* sort_increasing_deg(int*, int);
typedef struct
{
	int _n, _m, **_v;
} MAT;
MAT* initialize_mat(MAT*, int, int);
MAT* deallocate_mat(MAT*);
MAT* swap_columns(MAT*, int, int);
MAT* add_line(MAT*, int, int);
int* get_monomials(int, int, int*, int*);
int* get_support(const int *, int, int*, int*, int);
MAT* get_matrix(const int *, int, MAT*, int*, int, int, int);
int solve_matrix(MAT*, int*, int);
int algebarskiImunitet(int *, int);

void formatiraniPrikaz (int *, int);
void polarniPrikaz (int *, int);

int evaluate_box(int *, int, int, int);
int inner_product(int, int);
float transparency (int *, int);

int walshSpectrum (int *, int);
int autocorrelationSpectrum (int *, int);
int anf_deg (int *, int);

//int main()
float eval(int tt[], int nVariables, int varijanta, int *stop)
{


	/*SVOJSTVA

	Trenutno se gleda pet svojstava Boolean funkcija
	1. balansiranost - da je u tablici istinitosti jednak broj jedinica i nula na izlazu (2^n-1)
	2. korelacijski imunitet - korelacijski imunitet je stupnja t ako se distribucija vjerojatnosti izlaza ne mijenja ako je max t bitova ulaza fiksirano
	3. algebarski stupanj - najveci broj varijabli u funkciji
	4. algebarski imunitet - najmanja vrijednost d za koju tablica istinitosti ima ne-nula anhilator stupnja d
	5. nelinearnost - najmanja Hammingova udaljenost izmedu funkcije i najbolje afine aproksimacije funkcije

	za sva gore navedena svojstva, što veci broj to je i svojstvo bolje, jedino balansiranost je 1 ako je f balansirana, inace je kazna obrnuto proporcionalna broju varijabli

	6. autokorelacija spektar - što manja, to bolje
	7. karakteristika propagacije - funkcija zadovoljava karakteristiku propagacije stupnja t ako su sve derivacije vektora koje imaju Hammingovu tezinu manju od t balansirane, što vece, to bolje
	8. suma kvadrata indikator (služi za GAC) - što manji to bolje, da promjena jednog bita na ulazu utjece na što više bita na izlazu
	9. AC -  udaljenost izmedu Booleove funkcije i skupa funkcija sa linearnom strukturom (služi za GAC) - što manji, to bolje
	10. Walshov spektar - što su vrijednosti uniformnije, to je nelinearnost veca, najveca je kada je svaki clan jednak 2^(n/2) - Parsevalova jednadžba

	*/


	//treba ovdje u tt polje smjestiti prave vrijednosti, te  nVariables varijablu postaviti na pravu vrijednost

	//int tt[16] = {1,1,0,1,1,0,1,0,0,1,1,0,0,0,0,1};
	//int nVariables = 4;

	//int tt[256] = {1,0,0,0,0,1,1,1,1,1,0,1,0,0,1,0,0,1,1,1,1,0,0,0,0,0,1,0,1,1,0,1,1,0,1,0,0,1,0,1,0,1,1,1,0,0,0,0,1,0,1,0,0,1,0,1,1,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,1,1,0,1,1,1,0,1,0,1,1,1,0,1,1,1,0,0,1,0,0,0,1,0,1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,1,1,0,1,0,1,0,1,0,1,0,0,0,1,1,1,1,0,0,1,1,0,0,1,1,1,0,0,1,1,0,0,1,0,0,1,1,0,0,1,1,1,0,0,1,1,0,0,1,1,1,0,1,1,1,0,1,0,1,1,0,0,1,1,1,0,0,1,0,0,0,1,0,1,0,0,0,1,0,0,0,1,1,0,0,0,0,1,1,0,1,1,0,1,0,0,1,1,1,0,0,0,0,1,1,0,1,1,0,1,0,0,1,0,0,1,0,1,1,0,1,1,0,0,1,0,1,1,1,1,1,0,1,0,0,1,0,0,1,1,1,1,0,0,0};
	//int nVariables = 8;

	float fitness = 0;

	int fitness1 = 0, fitness2 = 0, fitness3 = 0;


	switch(varijanta) {

		case 1:
	//prvi set eksperimenata s najjednostavnijom fitness funkcijom - balans i nelinearnost
	fitness = balans(tt,nVariables) + wt_nl (tt, nVariables);
	break;

		case 4:
	//za GECCO2014
	fitness = balans(tt,nVariables) + (1 - transparency(tt, nVariables));
	break;

		case 2:
	//drugi set eksperimenata gdje je u fitness funkciju postavljen balans, nelinearnost, algebarski imunitet, korelacijski imunitet i algebarski stupanj
	fitness =   balans(tt,nVariables) + anf_ai_deg (tt, nVariables) + wt_nl_ci (tt, nVariables);
	break;

		case 3:
	//treci set eksperimenata gdje su svi parametri koji se mogu testirati
	fitness =   balans(tt,nVariables) + anf_ai_deg (tt, nVariables) + wt_nl_ci_spektar (tt, nVariables) + autokorelacija_rf_pc_max(tt, nVariables); //ovdje sam dodao promjenu, jer se prije nije gledao i Walshov spektar
	//pragovi:  1                       4      7                               100                                       32    72
	break;

	case 5:
	//za PPSN2014
	fitness = wt_nl (tt, nVariables);
	break;

	case 6:
	//za PPSN2014 2, minimizacijski problem
	fitness1 = wt_nl (tt, nVariables);
	fitness2 = anf_deg (tt, nVariables);
	fitness3 = balans(tt,nVariables);
	if (fitness3 == 0 && fitness1 == 55 && fitness2 == 7)
		*stop = 1;
	fitness = (fitness1 - 55) + (fitness2 - 7) + abs(fitness3);
	break;


	case 7:
	//za PPSN2014, samo GP
	fitness = balans(tt,nVariables) + wt_nl (tt, nVariables);
	break;
	}


	//cetvrti set eksperimenata - nešto kao meta-EA

	//fitness1 = balans(tt,nVariables) + wt_spektar(tt, nVariables) + autokorelacija_rf(tt, nVariables);

	//fitness2 = anf_ai_deg (tt, nVariables) + wt_nl_ci (tt, nVariables) + autokorelacija_pc_max(tt, nVariables); //drugi podskup

	//peti set eksperimenata - neka verzija suradnicke koevolucije - jedna podpopulacija radi po jednoj fitness funkciji, druga podpopulacija o drugoj, najbolje jedinke iz obje podpopulacije po trecoj

	//fitness1 = balans(tt,nVariables) + wt_spektar(tt, nVariables);
	//fitness2 = balans(tt,nVariables) + autokorelacija_rf(tt, nVariables);

	//fitness =  balans(tt,nVariables) + anf_ai_deg (tt, nVariables) + wt_nl_ci_spektar (tt, nVariables) + autokorelacija_rf_pc_max(tt, nVariables);	// sve zivo, sada i sa Walshovim spektrom


	//da li imati primjer gdje je neka težinska funkcija?

	//autokorelacija(tt, nVariables);

	//fitness = autokorelacija_max(tt, nVariables);

//	printf ("Fitness je %d\n",fitness);
	//anf (tt, nVariables);
	//wt (tt, nVariables);
	//printf("Balans je %d\n", balans(tt,nVariables)); //ovo

	return fitness;
}


int balans (int *tt, int n)
{
	int i, rez=0;

   for(i=0; i < (1<<n); i++)
   {
	   if (tt[i]==0) //brojim nule
		   rez=rez+1;
   }
   if (rez == ((1<<n)/2))
	   return 1; //za balansiranu TT dajemo malu nagradu jer se podrazumijeva da Booleova funkcija mora biti balansirana
   else if (rez < ((1<<n)/2)) // nula je manje od pola, što znaci da je jedinica više od pola
	{	if(rez == 0)
			rez = 1;
		//return (((1<<n)-rez)/rez)*(-5);
		// za fit3:
		return (((1<<n)-rez)/(1.*rez))*(-50);
	}
   else if (rez > ((1<<n)/2)) // nula je više od pola, što znaci da je jedinica manje od pola
	{	if(rez == (1 << n))
			rez = (1 << n) - 1;
	   //return (rez/((1<<n)-rez))*(-5);
	   // za fit3:
	   return ((1.*rez)/((1<<n)-rez))*(-50);
   }
}

void anf (int *tt, int n)
{
	int i, j, k, rezultat = 0;

	static int *t=0, *u=0, *rez=0;

	if(t == 0) {
		rez=(int *) malloc((1<<n)*sizeof(int));
		u=(int *) malloc(((1<<n)>>1)*sizeof(int));
		t=(int *) malloc(((1<<n)>>1)*sizeof(int));
	}

	for(i=0; i < (1<<n); ++i)
        rez[i] = tt[i];
	for( i=0; i < ((1<<n)>> 1); ++i )
        u[i]=t[i]=0;

	for( i=0; i < n; ++i )
	{
        for( j=0; j < ( (1<<n)>>1 ); ++j )
		{
            t[j] = rez[2*j];
            u[j] = (rez[2*j]==rez[2*j+1])? 0 : 1;
		}
        for( k=0; k<((1<<n) >> 1 ); ++k )
		{
            rez[k] = t[k];
            rez[((1<<n) >> 1 ) + k] = u[k];
		}
    }
	printf("ANF je\n");
	for (i=0; i < (1<<n); i++)
		printf("%d", rez[i]);
	printf("\n");

	formatiraniPrikaz (rez, n);

//	free(rez);
//	free(u);
//	free(t);

}

int anf_ai (int *tt, int n) //ovdje dobijam algebarski imunitet
{
	int i, j, k;

	static int *t=0, *u=0, *rez=0;

	if(t == 0) {
		rez=(int *) malloc((1<<n)*sizeof(int));
		u=(int *) malloc(((1<<n)>>1)*sizeof(int));
		t=(int *) malloc(((1<<n)>>1)*sizeof(int));
	}

	for(i=0; i < (1<<n); ++i)
        rez[i] = tt[i];
   for( i=0; i < ((1<<n)>> 1); ++i )
        u[i]=t[i]=0;

	for( i=0; i < n; ++i )
	{
        for( j=0; j < ( (1<<n)>>1 ); ++j )
		{
            t[j] = rez[2*j];
            u[j] = (rez[2*j]==rez[2*j+1])? 0 : 1;
		}
        for( k=0; k<((1<<n) >> 1 ); ++k )
		{
            rez[k] = t[k];
            rez[((1<<n) >> 1 ) + k] = u[k];
		}
    }


	//printf ("Algebarski imunitet je %d\n", algebarskiImunitet(tt, n)); //ovo


//	free(rez);
//	free(u);
//	free(t);

	return algebarskiImunitet(tt, n);

}

int anf_ai_deg (int *tt, int n) //ovdje dobijam algebarski stupanj i algebarski imunitet
{
	int i, j, k, rezultat = 0;

	static int *t=0, *u=0, *rez=0;

	if(t == 0) {
		rez=(int *) malloc((1<<n)*sizeof(int));
		u=(int *) malloc(((1<<n)>>1)*sizeof(int));
		t=(int *) malloc(((1<<n)>>1)*sizeof(int));
	}

	for(i=0; i < (1<<n); ++i)
        rez[i] = tt[i];
   for( i=0; i < ((1<<n)>> 1); ++i )
        u[i]=t[i]=0;

	for( i=0; i < n; ++i )
	{
        for( j=0; j < ( (1<<n)>>1 ); ++j )
		{
            t[j] = rez[2*j];
            u[j] = (rez[2*j]==rez[2*j+1])? 0 : 1;
		}
        for( k=0; k<((1<<n) >> 1 ); ++k )
		{
            rez[k] = t[k];
            rez[((1<<n) >> 1 ) + k] = u[k];
		}
    }

	//printf("Algebarski stupanj je %d\n", algebarskiStupanj(rez, n)); //ovo
	//printf ("Algebarski imunitet je %d\n", algebarskiImunitet(tt, n)); //ovo

	rezultat = algebarskiStupanj(rez, n) + algebarskiImunitet(tt, n);


//	free(rez);
//	free(u);
//	free(t);

	return rezultat;

}

int anf_deg (int *tt, int n) //ovdje dobijam algebarski stupanj
{
	int i, j, k, rezultat = 0;

	static int *t=0, *u=0, *rez=0;

	if(t == 0) {
		rez=(int *) malloc((1<<n)*sizeof(int));
		u=(int *) malloc(((1<<n)>>1)*sizeof(int));
		t=(int *) malloc(((1<<n)>>1)*sizeof(int));
	}

	for(i=0; i < (1<<n); ++i)
        rez[i] = tt[i];
   for( i=0; i < ((1<<n)>> 1); ++i )
        u[i]=t[i]=0;

	for( i=0; i < n; ++i )
	{
        for( j=0; j < ( (1<<n)>>1 ); ++j )
		{
            t[j] = rez[2*j];
            u[j] = (rez[2*j]==rez[2*j+1])? 0 : 1;
		}
        for( k=0; k<((1<<n) >> 1 ); ++k )
		{
            rez[k] = t[k];
            rez[((1<<n) >> 1 ) + k] = u[k];
		}
    }

	//printf("Algebarski stupanj je %d\n", algebarskiStupanj(rez, n)); //ovo
	//printf ("Algebarski imunitet je %d\n", algebarskiImunitet(tt, n)); //ovo

	rezultat = algebarskiStupanj(rez, n);


//	free(rez);
//	free(u);
//	free(t);

	return rezultat;

}

void wt (int *tt, int n)
{
    int i, j, m, halfm, t1, t2, r, a, b, max = 0;

	int *rez=0;

	rez=(int *) malloc((1<<n)*sizeof(int));



    for( i=0; i < (1<<n); ++i )
        rez[i] = (tt[i]==0)? 1 : -1;

    for( i = 1; i <= n; ++i ) {
        m  = (1 << i);
        halfm = m/2;
        for( r=0; r < (1<<n); r += m ) {
            t1 = r;
            t2 = r + halfm;
            for( j=0; j < halfm; ++j, ++t1, ++t2 ) {
                a = rez[t1];
                b = rez[t2];
                rez[t1] = a + b;
                rez[t2] = a - b;
				//za racunanje nelinearnosti da ne moram poslije opet tražiti max vrijednost
				if (abs(rez[t1]) > max)
					max = abs(rez[t1]);
			    if (abs(rez[t2]) > max)
					max = abs(rez[t2]);
            }
        }
    }
	printf("Walsh transformacija je\n");
	for (i=0; i < (1<<n); i++)
		printf("%d", rez[i]);
	printf("\n");


	//printf ("Stupanj linearnosti je %d\n", max);

	//printf ("Stupanj predrasude nelinearnosti je %d\n", predrasudaNelinearnost (max, n));

	free(rez);

}

int wt_nl (int *tt, int n) //ovdje dobijam nelinearnost
{
    int i, j, m, halfm, t1, t2, r, a, b, max = 0, rezultat = 0;

	int *rez=0;
	int *ac=0;

	rez=(int *) malloc((1<<n)*sizeof(int));

    for( i=0; i < (1<<n); ++i )
        rez[i] = (tt[i]==0)? 1 : -1;

    for( i = 1; i <= n; ++i ) {
        m  = (1 << i);
        halfm = m/2;
        for( r=0; r < (1<<n); r += m ) {
            t1 = r;
            t2 = r + halfm;
            for( j=0; j < halfm; ++j, ++t1, ++t2 ) {
                a = rez[t1];
                b = rez[t2];
                rez[t1] = a + b;
                rez[t2] = a - b;
				//za racunanje nelinearnosti da ne moram poslije opet tražiti max vrijednost
				if (abs(rez[t1]) > max)
					max = abs(rez[t1]);
			    if (abs(rez[t2]) > max)
					max = abs(rez[t2]);
            }
        }
    }

	//printf ("Stupanj nelinearnosti je %d\n", nelinearnost (max, n)); //ovo


	free(rez);

	return nelinearnost (max, n);
}

int wt_nl_ci (int *tt, int n) //ovdje dobijam nelinearnost i korelacijski imunitet
{
    int i, j, m, halfm, t1, t2, r, a, b, max = 0, rezultat = 0;

	int *rez=0;

	rez=(int *) malloc((1<<n)*sizeof(int));

    for( i=0; i < (1<<n); ++i )
        rez[i] = (tt[i]==0)? 1 : -1;

    for( i = 1; i <= n; ++i ) {
        m  = (1 << i);
        halfm = m/2;
        for( r=0; r < (1<<n); r += m ) {
            t1 = r;
            t2 = r + halfm;
            for( j=0; j < halfm; ++j, ++t1, ++t2 ) {
                a = rez[t1];
                b = rez[t2];
                rez[t1] = a + b;
                rez[t2] = a - b;
				//za racunanje nelinearnosti da ne moram poslije opet tražiti max vrijednost
				if (abs(rez[t1]) > max)
					max = abs(rez[t1]);
			    if (abs(rez[t2]) > max)
					max = abs(rez[t2]);
            }
        }
    }

	//printf ("Stupanj nelinearnosti je %d\n", nelinearnost (max, n)); //ovo
	//	printf ("Korelacijski imunitet je %d\n", korelacijskiImunitet (rez, n)); //ovo

	rezultat = nelinearnost (max, n) + korelacijskiImunitet (rez, n);

	free(rez);

	return rezultat;
}

int wt_spektar (int *tt, int n) //ovdje dobijam Walsh spektar
{
    int i, j, m, halfm, t1, t2, r, a, b, max = 0, rezultat = 0;

	int *rez=0;

	int walshSpektar = 0;

	rez=(int *) malloc((1<<n)*sizeof(int));

    for( i=0; i < (1<<n); ++i )
        rez[i] = (tt[i]==0)? 1 : -1;

    for( i = 1; i <= n; ++i ) {
        m  = (1 << i);
        halfm = m/2;
        for( r=0; r < (1<<n); r += m ) {
            t1 = r;
            t2 = r + halfm;
            for( j=0; j < halfm; ++j, ++t1, ++t2 ) {
                a = rez[t1];
                b = rez[t2];
                rez[t1] = a + b;
                rez[t2] = a - b;
				//za racunanje nelinearnosti da ne moram poslije opet tražiti max vrijednost
				if (abs(rez[t1]) > max)
					max = abs(rez[t1]);
			    if (abs(rez[t2]) > max)
					max = abs(rez[t2]);
            }
        }
    }

	walshSpektar = walshSpectrum2 (rez, n);

	free(rez);

	return walshSpektar;
}

int wt_nl_ci_spektar (int *tt, int n) //ovdje dobijam nelinearnost, korelacijski imunitet i Walshov spektar
{
    int i, j, m, halfm, t1, t2, r, a, b, max = 0, rezultat = 0;

	int *rez=0;
	int *ac=0;

	float walshSpektar = 0;

	rez=(int *) malloc((1<<n)*sizeof(int));

	ac=(int *) malloc((1<<n)*sizeof(int));

    for( i=0; i < (1<<n); ++i )
        rez[i] = (tt[i]==0)? 1 : -1;

    for( i = 1; i <= n; ++i ) {
        m  = (1 << i);
        halfm = m/2;
        for( r=0; r < (1<<n); r += m ) {
            t1 = r;
            t2 = r + halfm;
            for( j=0; j < halfm; ++j, ++t1, ++t2 ) {
                a = rez[t1];
                b = rez[t2];
                rez[t1] = a + b;
                rez[t2] = a - b;
				//za racunanje nelinearnosti da ne moram poslije opet tražiti max vrijednost
				if (abs(rez[t1]) > max)
					max = abs(rez[t1]);
			    if (abs(rez[t2]) > max)
					max = abs(rez[t2]);
            }
        }
    }

//	printf ("Stupanj nelinearnosti je %d\n", nelinearnost (max, n)); //ovo
//	printf ("Korelacijski imunitet je %d\n", korelacijskiImunitet (rez, n)); //ovo

	rezultat = nelinearnost (max, n) + korelacijskiImunitet (rez, n) + walshSpectrum2 (rez, n);

	free(rez);
	free(ac);

	return rezultat;
}

int algebarskiStupanj (int * anf, int n)
{

    int i, tmp, weight, deg;

    if(anf[ (1<<n)-1 ] != 0)
        deg = n;
    else
        for(deg=0, i=1; i < ((1<<n)-1); ++i)
            if( anf[i]!=0 ) {
                for(weight=0, tmp = i; tmp>0; tmp>>=1)
                    weight = weight + tmp%2;
                if(weight > deg)
                    deg = weight;
            }

	return deg;
}

int nelinearnost (int max, int n)
{
	return (((1<<n) - max)/2);
}

int predrasudaNelinearnost (int max, int n)
{
	return max/(1<<(n+1));
}

int korelacijskiImunitet (int *wh, int n)
{
	int i, red = 1;
	do //vrti po redu (stupnju) CI-a
	{
		for (i = 1; i < (1<<n); i++)
		{
			if (red == hammingWeight(i))
			{
				if (wh[i] != 0)
					return red-1;
			}
		}
		red++;
	} while (red <= n);
	return red-2;//ne znam zašto
}

int hammingWeight(int x)
{
    int res;
    for( res=0; x>0; x = x>>1 )
        res = res + x%2;
    return res;
}

void autokorelacija (int *tt, int n) //autokorelacija ne odreduje jedinstveno funkciju, ovdje prikazujem autokorelaciju
{
	int i, j, tmp = 0, res = 0;

	int *ac = 0;

	float autokorelacijskiSpektar = 0;

	ac =(int *) malloc((1<<n)*sizeof(int));

	for (i=0; i < (1<<n); i++)	//prolazak po svim elementima autokorelacije
	{
		for(j=0; j < (1<<n); j++)	//prolazak kroz sumaciju/sve elemente WT-a
		{
			tmp = tt[j]^tt[i^j];
			if (tmp == 1)
				tmp= -1;
			else if ( tmp == 0)
				tmp = 1;
			res= res + tmp;
		}
		ac[i] = res;
		res = 0;
	}
	printf("Autokorelacija je\n");
	for (i=0; i < (1<<n); i++)
		printf("%d", ac[i]);
	printf("\n");


	free(ac);

}

int autokorelacija_rf (int *tt, int n) //autokorelacija ne odreduje jedinstveno funkciju, ovdje vracam vrijednost indikator sume kvadrata
{
	int i, j, tmp = 0, res = 0, sumaKvadrata = 0;

	int *ac = 0;


	ac =(int *) malloc((1<<n)*sizeof(int));

	for (i=0; i < (1<<n); i++)	//prolazak po svim elementima autokorelacije
	{
		for(j=0; j < (1<<n); j++)	//prolazak kroz sumaciju/sve elemente WT-a
		{
			tmp = tt[j]^tt[i^j];
			if (tmp == 1)
				tmp= -1;
			else if ( tmp == 0)
				tmp = 1;
			res= res + tmp;
		}
		ac[i] = res;
		res = 0;
	}

	sumaKvadrata = sumaKvadrataIndikator(ac, n);


	free(ac);

	return sumaKvadrata;
}

int autokorelacija_max (int *tt, int n) //autokorelacija ne odreduje jedinstveno funkciju, ovdje vracam vrijednost autokorelacije (max)
{
	int i, j, tmp = 0, res = 0, autokor = 0;

	int *ac = 0;

	ac =(int *) malloc((1<<n)*sizeof(int));

	for (i=0; i < (1<<n); i++)	//prolazak po svim elementima autokorelacije
	{
		for(j=0; j < (1<<n); j++)	//prolazak kroz sumaciju/sve elemente WT-a
		{
			tmp = tt[j]^tt[i^j];
			if (tmp == 1)
				tmp= -1;
			else if ( tmp == 0)
				tmp = 1;
			res= res + tmp;
		}
		ac[i] = res;
		res = 0;
	}


	autokor = AC (ac, n); //maksimalna vrijednost autokorelacije

	free(ac);

	return autokor;
}

int autokorelacija_rf_pc_max (int *tt, int n) //autokorelacija ne odreduje jedinstveno funkciju, ovdje vracam vrijednost autokorelacije (max), karakteristiku propagacije i indikator sume kvadrata
{
	int i, j, tmp = 0, res = 0, PC = 0, autokor = 0, sumaKvadrata = 0;

	int *ac = 0;

	ac =(int *) malloc((1<<n)*sizeof(int));

	for (i=0; i < (1<<n); i++)	//prolazak po svim elementima autokorelacije
	{
		for(j=0; j < (1<<n); j++)	//prolazak kroz sumaciju/sve elemente WT-a
		{
			tmp = tt[j]^tt[i^j];
			if (tmp == 1)
				tmp= -1;
			else if ( tmp == 0)
				tmp = 1;
			res= res + tmp;
		}
		ac[i] = res;
		res = 0;
	}

	PC = karakteristikaPropagacije(ac, n);

	/*printf ("Karakteristika propagacije je %d\n", PC);

	if (PC == 1)
		printf("Funkcija zadovoljava SAC - strict avalanche criterion\n");
		*/
	autokor = AC (ac, n); //maksimalna vrijednost autokorelacije

	//printf("Max korelacija je %d\n", autokor);

	sumaKvadrata = sumaKvadrataIndikator(ac, n);

	//printf("Suma kvadrata je je %d\n", sumaKvadrata);


	free(ac);

	return PC+autokor+sumaKvadrata;
}

int karakteristikaPropagacije (int *ac, int n) //što vece to bolje
{
	int i, red = 1;
	do //vrti po redu (stupnju) autokorelacije
	{
		for (i = 1; i < (1<<n); i++)
		{
			if (red == hammingWeight(i))
			{
				if (ac[i] != 0)
					return red-1;
			}
		}
		red++;
	} while (red <= n);
	return red-2;//ne znam zašto
}

int AC (int *ac, int n) //što manje to bolje, pa na kraju dijelimo s -n/2 jer u biti ovo djeluje kao kazna (faktor je 4 jer inace ima previše utjecaja), min moguca vrijednost je 16 a ide lako na preko 100, vidjeti cemo da li cemo morati još ovo modificirati
{
	int i, max = 0;
	int tmp = 0;
	tmp =n/2;
	for (i=1; i < (1<<n); i++)	//zanemarujemo prvu vrijednost jer je ona uvijek 2^n
	{
		if (abs(ac[i]) > max)
			max = abs(ac[i]);
	}
	return max/(-tmp);
}

int sumaKvadrataIndikator (int *ac, int n) //što manje to bolje, pa na kraju množimo s -1 jer u biti ovo djeluje kao kazna, takoder smanjujem vrijednost dane bude predominantna
{
	int i, suma = 0;
	for (i=0; i < (1<<n); i++)
	{
		suma = suma + (ac[i]*ac[i]);
	}
	suma = (sqrt((long double)suma/n))/2;
	return suma*(-1);
}

int walshSpectrum2(int *walsh, int n)  //funkcija racuna float ali vracam int jer ne znam što bih radio s decimalnim dijelom, što manje to bolje
{
	int i;
	double rez = 0;
	float pot = 0.0, x = 0;
	pot = (float)n/2;
	x = pow((float)2.0,pot);	//2^n/2
	for (i=0; i < (1<<n);i++)
	{
		pot = fabs((float)walsh[i])-x;
		rez = rez + pow(pot,2);
	}
	rez = sqrt((long double)rez);
	return rez/(-2); //vracam minus vrijednost jer je ovo kazna, dijelims dva da ne bude predominantna
}

void polarniPrikaz (int *tt, int n)
{
	int i, *rez=0;
	rez=(int *) malloc((1<<n)*sizeof(int));

	for (i=0; i < (1<<n); i++)
	{
        rez[i] = (tt[i]==0)? 1 : -1;
	}
	printf("Polarni prikaz je\n");
	for (i=0; i < (1<<n); i++)
		printf("%d", rez[i]);
	printf("\n");
}

void formatiraniPrikaz (int *anf, int n)
{
	int i, tmp = 0, j, a = 0;
	char prikaz[1000]="";
	char *xor = " ^ ";
	char *jedan = "1 ^ ";
	char *x = "X";
	char broj[5];

	if (anf [0] == 1)
	{
		strcat(prikaz, jedan);
	}

	for (i=1; i < (1<<n);i++)
	{
		if (anf[i]==1)
		{
			tmp=i;
			for (j=1; tmp > 0; tmp=tmp>>1,j++)
			{
				if (tmp%2==1)
				{
					strcat (prikaz, x);
					sprintf(broj, "%d", j);
					strcat(prikaz,broj);
				}
			}
				strcat(prikaz, xor);
			}

	}
	puts (prikaz);
}



//sve ispod treba za AI


int choose(int n, int k)
{
    int i, num = 1, den = 1;
    if( k<0 || k>n ) return 0;
    for( i=0; i<k; ++i ) {
        num *= n--;
        den *= (k-i); }
    return (num/den);
}

int preceq( int a, int b )
{
    int res = 1;
    while( (a>0 || b>0) && (res==1) ) {
        if( a%2 > b%2 ) res = 0;
        a >>= 1; b >>= 1; }
    return res;
}

int* sort_increasing_deg( int* v, int len )
{
    int i,j,tmp;
    for( i=0; i < len-1; ++i )
        for( j=i+1; j<len; ++j )
            if(hammingWeight(v[j]) < hammingWeight(v[i]) ) {
                tmp = v[j];
                v[j] = v[i];
                v[i] = tmp;
            }
    return v;
}

MAT* initialize_mat( MAT* mat, int nrow, int ncol)
{
    int i,j;
    mat = (MAT*) malloc (sizeof(MAT));
    mat->_n = nrow;
    mat->_m = ncol;
    mat->_v = (int**) malloc (nrow * sizeof(int*));
    for(i=0; i < nrow; ++i) {
        mat->_v[i] = (int*) malloc (ncol * sizeof(int));
        for(j=0; j < ncol; ++j)
            mat->_v[i][j] = 0;
    }
    return mat;
}

MAT* deallocate_mat( MAT* mat)
{
    int i;
    if( mat != NULL ) {
        if(mat->_v != NULL) {
            for( i=0; i < mat->_n; ++i )
                free(mat->_v[i]);
            free(mat->_v);
        }
        free(mat);
    }
    return NULL;
}

MAT* swap_columns( MAT* mat, int a, int b )
{
    int* tmp,i;
    tmp = (int*) malloc ( mat->_n * sizeof(int) );
    for( i=0; i<mat->_n; ++i ) tmp[i] = mat->_v[i][a];
    for( i=0; i<mat->_n; ++i ) mat->_v[i][a] = mat->_v[i][b];
    for( i=0; i<mat->_n; ++i ) mat->_v[i][b] = tmp[i];
    free(tmp);
    return mat;
}

MAT* add_line( MAT* mat, int dst, int src )
{
    int j;
    for( j=0; j < mat->_m; ++j )
        mat->_v[dst][j] = (mat->_v[dst][j] + mat->_v[src][j] )%2;
    return mat;
}

int* get_monomials( int n, int d, int* res, int* N )
{
    int i,k;
    for( (*N)=0, k=0; k<=d; ++k )
        (*N) = (*N) + choose(n,k);
    res = (int*) malloc ((*N) * sizeof(int));
    for( k=0, i=0; i<(1<<n); ++i )
        if( hammingWeight(i) <= d )
           res[k++] = i;
    return res;
}

int* get_support( const int * tt, int n, int* res, int* N, int b )
{
    int i,k,len;
    len = 1<<n;
    for( (*N)=0, i=0; i<len; ++i )
        (*N) = (*N) + (tt[i] != b);
    res = (int*) malloc ((*N) * sizeof(int));
    for( k=0, i=0; i<len; ++i )
        if( tt[i] != b )
            res[k++] = i;
    return res;
}

MAT* get_matrix( const int * tt, int n, MAT* m, int* monomials, int Nm, int ai, int b )
{
    int Ns, i, j, len, *support;
    len = 1<<n;
    support = NULL;
    support = get_support( tt, n, support, &Ns, b );
    if (Ns == 0 || Ns == len)
        m = NULL;
    else {
        m = (Nm>Ns)? initialize_mat(m,Nm,Nm): initialize_mat(m,Nm,Ns);
        for( i=0; i<Nm; ++i )
            for( j=0; j<Ns; ++j )
                m->_v[i][j] = preceq( monomials[i], support[j] );
    }
    free(support);
    return m;
}

int solve_matrix( MAT* m, int* monomials, int b )
{
    int i, j, l, res, *deg, processed_lines, zero_lines;
    deg = (int*) malloc (m->_n * sizeof(int));
    for( res = 0, i = 0; i < m->_n; ++i ) {
        deg[i] = hammingWeight(monomials[i]);
        if( deg[i] > res ) res = deg[i];
    }
    processed_lines = zero_lines = 0;
    for( i = 0; i < m->_n; ++i ) {
        for( j = 0; j < m->_m && m->_v[i][j] == 0; ++j );
        if( j == m->_m ) {
            ++ zero_lines;
            if( deg[i] < res && deg[i]!=0 )
                res = deg[i];
        } else {
            ++ processed_lines;
            if( i!=j && i<m->_m && j<m->_m )
                m = swap_columns( m, i, j );
            for( l=i+1; l < m->_n && i < m->_m; ++l ) {
                if( i < m->_m && m->_v[l][i] != 0 ) {
                    m = add_line( m, l, i );
                    deg[l] = (deg[i] > deg[l])? deg[i] : deg[l];
                }
            }
        }
    }
    free (deg);
    return res;
}

int algebarskiImunitet(int *tt, int n)
{
    MAT *m0 = NULL, *m1 = NULL;
    int deg, *monomials = NULL, Nm;
    int a, b, rez=0;

    deg = (n >> 1) + (n % 2);
    monomials = get_monomials(n, deg, monomials, &Nm);
    monomials = sort_increasing_deg(monomials, Nm);
    m0 = get_matrix(tt, n, m0, monomials, Nm, deg, 0);
    if(m0 == NULL)
        rez = 0;
    else {
        m1 = get_matrix(tt, n, m1, monomials, Nm, deg, 1);
        a = solve_matrix(m0, monomials, 0);
        b = solve_matrix(m1, monomials, 1);
        rez = (a<b)? a : b;
    }

    free(monomials);
    deallocate_mat(m0);
    deallocate_mat(m1);

    return rez;
}

//ove se funkcije ne koriste
int autocorrelationSpectrum (int *ac, int n) //ova se mjera cini previše eksperimentalna pa ju necemo koristiti, koristimo sumaKvadrataIndikator
{
	int i, rez = 0, x, r;
	x = -2;
	r = 4;
	for (i=1; i < (1<<n);i++)//ne uzimam prvu vrijednost jer je uvijek 2^n
	{
		rez = rez + pow((long double)abs((abs(ac[i])-x)),r);
	}

	return rez;
}

int walshSpectrum(int *walsh, int n)  //ova se mjera cini previše eksperimentalna pa ju necemo koristiti, koristimo walshSpectrum2
{
	int i, rez = 0, x, r;
	x = 8;
	r = 4;
	for (i=0; i < (1<<n);i++)
	{
		rez = rez + pow((long double)abs((abs(walsh[i])-x)),r);
	}

	return rez;
}

float transparency (int *tt, int nVariables)
{
	int N = nVariables;
	int M = 1;
	float res = 0.0;
	float C = (float) ((1 << (2*N)) - (1 << N));
	int b, a, v, x, tmp1, tmp2;
	float sigma1, sigma2, sigma3 = 0;
	int tmp = 0;

	tmp1 = (1 << M);
	tmp2 = (1 << N);

	for (b = 0; b < tmp1; b++)
	{
		sigma1 = 0;
		for (a = 1; a < tmp2; a++)
		{
			sigma2 = 0;
			for (v = 1; v < tmp1; v = v << 1) //can go from 1 since we need that Hamming weight be 1
			{
				sigma3 = 0;
				for (x = 0; x < tmp2; x++)
				{
					sigma3 = sigma3 + (float) (1 - 2*inner_product(v, (evaluate_box(tt, x, a, nVariables))));
				}
				sigma2 = sigma2 + (float) (1 - 2*inner_product(v, b)) * sigma3;
			}

			if (sigma2 < 0)
				sigma2 = sigma2 *(-1);

			sigma1 = sigma2 + sigma1;
		}
		tmp = 2 * hammingWeight(b);
		if (res < abs(M - tmp)-sigma1/C)
			res = abs(M - tmp)-sigma1/C;
	}
	return res;
}

int inner_product(int a, int b)
{
	int M = 1;
	int i, res = 0;
	for (i = 0; i < M; i++)
	{
		res = res ^ (((a >> i) & 0x01) & ((b >> i ) & 0x01));
	}
	return res;
}

int evaluate_box(int *tt, int x, int a, int nVariables)
{
 	int N = nVariables;
	int M = 1;
	int i, res1 = 0, res2 = 0, tmp, help;

	tmp = 1 << M;

	for (i = 0; i < M; i++)
	{
		help = *(tt + tmp*i + x);
		res1 |= ((*(tt + tmp*i + x)) << i);
	}

	x = x^a;
	for (i = 0; i < M; i++)
		res2 |= ((*(tt + tmp*i + x)) << i);

	return res1 ^ res2;
}

//my inplementacion so it works with this proboly wont work if function set is changed
uint getResult(uint firstOperator, uint secendOperator, std::string functionName)
{
	//change name into integer so we can switch
	int functionIndex;
	if( strcmp(functionName.c_str(), "NOT") == 0) functionIndex = 0;
	else if( strcmp(functionName.c_str(), "AND") == 0) functionIndex = 1;
	else if( strcmp(functionName.c_str(), "OR") == 0) functionIndex = 2;
	else if( strcmp(functionName.c_str(), "XOR") == 0) functionIndex = 3;
	else if( strcmp(functionName.c_str(), "XNOR") == 0) functionIndex = 4;
	else if( strcmp(functionName.c_str(), "IF") == 0) functionIndex = 5;
	else functionIndex = 0;


	uint output;
	switch (functionIndex)
	{
	case 0:
		output = !firstOperator;
		break;
	case 1:
		output = firstOperator & secendOperator;
		break;
	case 2:
		output = firstOperator | secendOperator;
		break;
	case 3:
		output = firstOperator ^ secendOperator;
		break;
	case 4:
		output = !(firstOperator ^ secendOperator);
		break;
	case 5:
		if(firstOperator)
			output = secendOperator;
		else
			output = !secendOperator;
		break;
	default:
		output = 0;
		break;
	}
	return output;
}

StateP state_;
uint freq, maximal;

void FunctionMinEvalOp::registerParameters(StateP state)
{
	state->getRegistry()->registerEntry("function", (voidP) (new uint(1)), ECF::UINT);
}
bool FunctionMinEvalOp::initialize(StateP state)
{
	voidP sptr = state->getRegistry()->getEntry("function"); // get parameter value
	iFunction_ = *((uint*) sptr.get()); // convert from voidP to user defined type
	state_ = state;
	
	sptr = state_->getRegistry()->getEntry("term.stagnation");
	freq = *((uint*)sptr.get());
	sptr = state_->getRegistry()->getEntry("term.maxgen");
	maximal = *((uint*)sptr.get());
		
	showTruth = false;

	return true;
}


FitnessP FunctionMinEvalOp::evaluate(IndividualP individual)
{

	FitnessP fitness (new FitnessMax);

	cart::Cartesian* gen = (cart::Cartesian*) individual->getGenotype().get();

	double value = 0;
	
	int Nrows = gen->getNumOfRows();
	int Ncolumns = gen->getNumOfCols();
	int Ninputs = gen->getNumOfInputs();
	int Noutputs = gen->getNumOfOutputs();

	
	//field rez has 8 integers for every input, it is truth table
	//field inputs: first 8 elements are inputs to sbox, after are sub-results of every logic gate
	

	int* rez = (int*)malloc(sizeof(int)*(Noutputs * (int)pow(2.,Ninputs)));
	uint* inputs = (uint*)malloc(sizeof(uint)*(Ninputs + (Nrows*Ncolumns)));

	//generat all 256 posible input data
	
	for(int i7 = 0; i7 < 2; i7++)
	{
		for(int i6 = 0; i6 < 2; i6++)
		{
			for(int i5 = 0; i5 < 2; i5++)
			{
				for(int i4 = 0; i4 < 2; i4++)
				{
					for(int i3 = 0; i3 < 2; i3++)
					{
						for(int i2 = 0; i2 < 2; i2++)
						{
							for(int i1 = 0; i1 < 2; i1++)
							{
								for(int i0 = 0; i0 < 2; i0++)
								{
									//set first 8 elements of inputs on curent input

									inputs[0] = i0;
									inputs[1] = i1;
									inputs[2] = i2;
									inputs[3] = i3;
									inputs[4] = i4;
									inputs[5] = i5;
									inputs[6] = i6;
									inputs[7] = i7;

									//for each logic gate compute result and save it in inputs

									for(int curentNode = 0; curentNode < (Nrows * Ncolumns); curentNode++)
									{
										inputs[curentNode + Ninputs] = getResult( inputs[gen->at(3 * curentNode)],  inputs[gen->at((3 * curentNode) + 1)],    gen->funcSet->at(gen->at((3 * curentNode) + 2))->getName());
									}

									//compute offset to know where in rez we need to write result
									//last 8 elements of genetype gen represent outputs, find them and copy to rez

									int inputOffset = Noutputs * (128*i7 + 64*i6 + 32*i5 + 16*i4 + 8*i3 + 4*i2 + 2*i1 + i0);
									for(int j = 0; j < Noutputs; j++)
									{
										rez[inputOffset + j] = inputs[gen->at(3*Nrows*Ncolumns + j)];
									}
								}
							}
						}
					}
				}
			}
		}
	}
		
	value = eval(rez,Ninputs,iFunction_,NULL);

	fitness->setValue(value);

	if(showTruth) {
		cout << "Usao\n\n\n";
		stringstream ss;
		uint size = sizeof(rez) / sizeof(int);	// velicina
		for(uint i = 0; i < (1 << Ninputs); i++)
			ss << rez[i];// << " ";
		ss << endl;
		ECF_LOG(state_, 1, ss.str());
	}

	free(rez);
	free(inputs);

	return fitness;
}

