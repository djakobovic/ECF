///////////////////////////////////////////////////////////////////////
// Deklaracija razreda Matrica, SLAJ, LUdek, Vektor3D
///////////////////////////////////////////////////////////////////////

#ifndef matrica_h
#define matrica_h

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <memory.h>

class Matrica
{
	protected:
		int row,col;
		//double **data;
		void Take(void);
		void Free(void);
	public:
		double **data;	// zbog ocajnickog ubrzavanja
		Matrica Transpose(int);
		Matrica();
		Matrica(int,int);
		Matrica(Matrica&);
		~Matrica();
		double Get(int,int) const;
		double Get(int) const;
		double Set(int,int,double);
		double Set(int,double);
		double Norm(void);
		int GetRow(void) const;
		int GetCol(void) const;
		int CopyColumn(int,Matrica&);
		int SetColumn(int,Matrica&);
		Matrica& operator= (Matrica&);
		Matrica operator+ (Matrica&);
		Matrica operator+ (double);
		void operator+= (Matrica&);
		Matrica operator- (const Matrica&);
		void operator-= (Matrica&);
		Matrica operator* (const Matrica&);
		Matrica operator* (const double&);
		Matrica operator/ (const double&);
		Matrica operator~ ();	// transponiranje
		double* operator[](const int redak);
		void Reset(int,int=1);
		void Init(int,int=1);
		int Load(const char*);
		void Save(const char*);
		void SaveLong(const char* fname);
		void Show(void);
};

inline int Matrica::GetRow(void) const
{	return row;	}

inline int Matrica::GetCol(void) const
{	return col;	}

inline double* Matrica::operator[](const int redak)
{	assert(redak >= 0 && redak < row);
	return data[redak];	}

class SLAJ		// sustav lin. alg. jednadzbi
{
	protected:
		Matrica A;
		Matrica b;		
	public:
		void Load_A(char*);
		void Load_b(char*);
		void Save_A(char*);
		void Save_b(char*);
		void Set_A(Matrica&);
		void Set_b(Matrica&);
		void GetSolution(Matrica&);
};

// LU i LUP dekompozicija
class LUdek : public SLAJ
{
	protected:
		int *P;
		int n;
	public:
		LUdek(void);
		int Solve(Matrica&,Matrica&,Matrica&);
		int Decompose(void);
		int ForwardSubst(void);
		int BackwardSubst(void);
		Matrica Invert(Matrica&);
		int pivot;	// 0 - bez pivotiranja
					// 1 - sa pivotiranjem
};

#endif	// matrica_h