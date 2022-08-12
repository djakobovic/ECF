////////////////////////////////////////////////////////////////////////////
// Implementacija razreda Matrica, SLAJ, LUDek
////////////////////////////////////////////////////////////////////////////

#include "matrice.h"
#include "check.h"

#define ABS(X)	(X>0 ? X : -X)
//#include <axh.h>	// za assert

////////////////////////////////////////////////////////////////////////////
// razred Matrica

void Matrica::Free(void)
{
	static int i;
	if(data==NULL)
		return;
	for(i=0;i<row;i++)
		delete[] data[i];
	delete[] data;
}

void Matrica::Take(void)
{
	static int i;
	CHECK(row>0 && col>0);
	data=new double*[row];
	for(i=0;i<row;i++)
		data[i]=new double[col];
}

Matrica::Matrica(void)
{
	col=row=0;
	data=NULL;
}

Matrica::Matrica(int _row,int _col=1)
{
	col=_col;
	row=_row;
	Take();
}

void Matrica::Init(int _row, int _col)
{
	Free();
	col=_col;
	row=_row;
	Take();
}

Matrica::Matrica(Matrica &source)
{
	static int i;
	row=source.row;
	col=source.col;
	Take();
	for(i=0;i<row;i++)
		memcpy(data[i],source.data[i],col*sizeof(double));
		/*for(int j=0;j<col;j++)
			data[i][j]=source.data[i][j];*/
}

Matrica& Matrica::operator =(Matrica& source)
{
	static int i;
	if(data!=NULL) Free();
	row=source.row;
	col=source.col;
	Take();
	for(i=0;i<row;i++)
		memcpy(data[i],source.data[i],col*sizeof(double));
		/*for(int j=0;j<col;j++)
			data[i][j]=source.data[i][j];*/
	return *this;
}

Matrica::~Matrica()
{
	Free();
}

Matrica Matrica::operator +(Matrica &op2)
{
	Matrica R(row,col);
	CHECK(row==op2.row && col==op2.col && row>0);
	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++)
			R.data[i][j]=data[i][j]+op2.data[i][j];
	return R;
}

void Matrica::operator +=(Matrica &op2)
{
	CHECK(row==op2.row && col==op2.col && row>0);
	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++)
			data[i][j] += op2.data[i][j];
}

void Matrica::operator -=(Matrica &op2)
{
	CHECK(row==op2.row && col==op2.col && row>0);
	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++)
			data[i][j] -= op2.data[i][j];
}

Matrica Matrica::operator -(const Matrica &op2)
{
	Matrica R(row,col);
	CHECK(row==op2.row && col==op2.col && row>0);
	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++)
			R.data[i][j]=data[i][j]-op2.data[i][j];
	return R;
}

Matrica Matrica::operator *(const Matrica &op2)
{
	Matrica R(row,op2.col);
	CHECK(col==op2.row);
	for(int i=0;i<row;i++)
		for(int j=0;j<op2.col;j++)
		{	R.data[i][j]=0;
			for(int k=0;k<col;k++)
				R.data[i][j]+=data[i][k]*op2.data[k][j];
		}
	return R;
}

Matrica Matrica::operator *(const double &scalar)
{
	Matrica R(row,col);
	for(int i=0;i<row;i++)
		for(int j=0;j<col;j++)
			R.data[i][j]=this->data[i][j]*scalar;
	return R;
}

// translacija
Matrica Matrica::operator ~(void)
{
	CHECK(data!=NULL);
	Matrica P(col, row);
	int i,j;
	for(i=0;i<row;i++)
		for(j=0;j<col;j++)
			P.data[j][i]=data[i][j];
	return P;
}

Matrica Matrica::operator /(const double &scalar)
{
	Matrica R(row,col);
	int i,j;

	for (i=0;i<row; i++)
		for (j=0;j<col; j++)
			R.Set(i,j,data[i][j]/scalar);
	return R;
}

// transponira matricu: ako je parametar 0, transponira samo kopiju, inace sebe samu
Matrica Matrica::Transpose(int transpose)
{
	CHECK(data!=NULL);
	Matrica P(col, row);
	int i,j;
	for(i=0;i<row;i++)
		for(j=0;j<col;j++)
			P.data[j][i]=data[i][j];
	if(!transpose)
		return P;
	// ako i this treba transponirati
	i=row; row=col; col=i;
	Free(); Take();
	for(i=0;i<row;i++)
		for(j=0;j<col;j++)
			data[i][j]=P.data[i][j];
	return (*this);
}

double Matrica::Get(int _row,int _col) const
{
	CHECK(data!=NULL && _row>=0 && _row<row && _col>=0 && _col<col);
	return data[_row][_col];
}

double Matrica::Get(int _row) const
{
	CHECK(data!=NULL && _row>=0 && _row<row);
	return data[_row][0];
}

double Matrica::Set(int _row,int _col, double value)
{
	CHECK(data!=NULL && _row>=0 && _row<=row && _col>=0 && _col<=col);
	data[_row][_col]=value;
	return value;
}

double Matrica::Set(int _row,double value)
{
	CHECK(data!=NULL && _row>=0 && _row<=row);
	data[_row][0]=value;
	return value;
}

int Matrica::Load(const char* fname)
{
	double value;
	FILE *fp;
	CHECK((fp=fopen(fname,"r"))!=NULL);
	if(!(fscanf(fp,"%d %d",&row,&col)!=EOF))
		exit(1);
	if(data!=NULL)
		Free();
	Take();
	for(int i=0;i<row*col;i++)
	{	if(!(fscanf(fp," %lf ",&value)!=EOF))
			exit(1);
		Set(i/col,i%col,value);
	}
	fclose(fp);
	return 1;
}

void Matrica::Save(const char* fname)
{
	FILE *fp;
	if((fp=fopen(fname,"w+"))==NULL)
		exit(1);
	fprintf(fp,"%d %d\n",row,col);
	for(int i=0;i<row;i++)
	{	for(int j=0;j<col;j++)
			fprintf(fp," %g ",data[i][j]);
		fprintf(fp,"\n");
	}
	fclose(fp);
}

void Matrica::SaveLong(const char* fname)
{
	FILE *fp;
	if((fp=fopen(fname,"w+"))==NULL)
		exit(1);
	fprintf(fp,"%d %d\n",row,col);
	for(int i=0;i<row;i++)
	{	for(int j=0;j<col;j++)
			fprintf(fp," %lf ",data[i][j]);
		fprintf(fp,"\n");
	}
	fclose(fp);
}

void Matrica::Show(void)
{
	int i,j;
	printf("Rows: %d Cols: %d\n",row,col);
	for(i=0;i<row;i++)
	{	for(j=0;j<col;j++)
			printf("%G\t",data[i][j]);
		printf("\n");
	}
}

void Matrica::Reset(int _row,int _col)
{
	CHECK(_row>0 && _col>0);
	Free();
	row=_row;
	col=_col;
	Take();
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			this->data[i][j] = 0;
}

int Matrica::CopyColumn(int colnum,Matrica &V)
{
	int i;
	CHECK(colnum>-1 && colnum<col);
	V.Reset(row,1);
	for(i=0;i<row;i++)
		V.Set(i,data[i][colnum]);
	return 1;
}

int Matrica::SetColumn(int colnum,Matrica &v)
{
	int i;
	CHECK(v.GetRow()==row);
	CHECK(colnum>-1 && colnum<col);
	for(i=0;i<row;i++)
		data[i][colnum]=v.Get(i);
	return 1;
}

double Matrica::Norm(void)
{
	double r=0;
	for(int i=0;i<row;i++)
		r+= data[i][0]*data[i][0];
	return r;
}

Matrica Matrica::operator+ (double broj)
{	Matrica R(*this);
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			R[i][j] += broj;
	return R;
}


////////////////////////////////////////////////////////////////////////////
// razred SLAJ

void SLAJ::Load_A(char* fname)
{
	A.Load(fname);
}

void SLAJ::Load_b(char* fname)
{
	b.Load(fname);
}

void SLAJ::Save_A(char* fname)
{
	A.Save(fname);
}

void SLAJ::Save_b(char* fname)
{
	b.Save(fname);
}

void SLAJ::Set_A(Matrica &_A)
{
	A.Reset(_A.GetRow(),_A.GetCol());
	A=_A;
}

void SLAJ::Set_b(Matrica &_b)
{
	CHECK(_b.GetCol()==1);
	b=_b;
}

void SLAJ::GetSolution(Matrica &X)
{
	static int row,i;
	row=b.GetRow();
	X.Reset(row,1);
	for(i=0;i<row;i++)
		X.data[i][0]=b.data[i][0];
	//X=b;
}

////////////////////////////////////////////////////////////////////////////
// razred LUdek

// za potrebe DASGUPTA6 promijenio sam sve oznaceno sa !!!


LUdek::LUdek(void)
{
//	P=NULL;	!!!
	P=new int[9];
	for(int i=0;i<9;i++)
		P[i]=i;
}

int LUdek::Solve(Matrica &A,Matrica &X,Matrica &b)
{
	Set_A(A);
	Set_b(b);
	Decompose();
	ForwardSubst();
	BackwardSubst();
	//GetSolution(X);
	for(int i=0;i<n;i++)
		X.data[i][0]=b.data[P[i]][0];
	return 1;
}

Matrica LUdek::Invert(Matrica &M)
{
	// rjesavamo sustav M*M!=I
	static int i,j,dim,poc=1;
	static Matrica R(6,6), RR(6,6), I(6,6), c(6,1);
	//static Matrica R(3,3), RR(3,3), I(3,3), c(3,1);
	CHECK((dim=M.GetCol()) == M.GetRow());
//	Matrica R(dim,dim), RR(dim,dim), I(dim,dim), c(dim,1); !!!
	// I jedinicna
	if(poc==1)
	{	for(i=0;i<dim;i++)
		{	for(j=0;j<dim;j++)
				I.Set(i,j,0);
			I.Set(i,i,1);
		}
		poc=0;
	}

	Set_A(M);
	Decompose();

	for(i=0;i<dim;i++)
	{	I.CopyColumn(i,c);
		Set_b(c);
		ForwardSubst();
		BackwardSubst();
		GetSolution(c);
		R.SetColumn(i,c);
	}

	//vratimo retke ako je bilo pivotiranja
	for(i=0;i<n;i++)
		for(j=0;j<n;j++)
			RR.data[i][j]=R.data[P[i]][j];

	return RR;
}


int LUdek::Decompose(void)
{
	static int i,j,k,p;
	n=A.GetRow();
/*	if(P!=NULL)	!!!
		delete[] P;
	P=new int[n];
	for(i=0;i<n;i++)
		P[i]=i;
*/
	
	for(i=0;i<n-1;i++)
	{	if(pivot==1)		// pivotiranje
		{	p=i;
			for(j=i+1;j<n;j++)
				if(ABS(A.Get(P[j],i))>ABS(A.Get(P[p],i)))
					p=j;
			if(A.Get(P[p],i)==0)
				CHECK(0);
			k=P[i];
			P[i]=P[p];
			P[p]=k;
		}
		for(j=i+1;j<n;j++)
		{	//A.Set(P[j],i,A.Get(P[j],i)/A.Get(P[i],i));
			A.data[P[j]][i]=A.data[P[j]][i]/A.data[P[i]][i];
			for(k=i+1;k<n;k++)
				//A.Set(P[j],k,A.Get(P[j],k)-A.Get(P[j],i)*A.Get(P[i],k));
				A.data[P[j]][k]-= A.data[P[j]][i]*A.data[P[i]][k];
		}
	}
	return 1;
}

int LUdek::ForwardSubst(void)
{
	static int i,j;
	for(i=0;i<n-1;i++)
		for(j=i+1;j<n;j++)
			//b.Set(P[j],0,b.Get(P[j],0)-A.Get(P[j],i)*b.Get(P[i],0));
			b.data[P[j]][0]-= A.data[P[j]][i]*b.data[P[i]][0];
	return 1;
}

int LUdek::BackwardSubst(void)
{
	static int i,j;
	for(i=n-1;i>=0;i--)
	{	//b.Set(P[i],0,b.Get(P[i],0)/A.Get(P[i],i));
		b.data[P[i]][0] /= A.data[P[i]][i];
		for(j=0;j<i;j++)
			//b.Set(P[j],0,b.Get(P[j],0)-A.Get(P[j],i)*b.Get(P[i],0));
			b.data[P[j]][0] -= A.data[P[j]][i]*b.data[P[i]][0];
	}
	return 1;
}