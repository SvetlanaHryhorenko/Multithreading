// testMPI.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include "mpi.h"
#include <vector>
#include <windows.h>
#include <stdio.h>
#include <algorithm>
#include <iomanip>

using namespace std;

float h=0.1;
int hmin=0;
int hmax=1;
float t=0.001;
int tmin=0;
int tmax=1;
const int H=10/*(hmax-hmin)/h*/;
const int T=1000/*(tmax-tmin)/t*/;
float result[T][H];
float e=0.001;
   
    
float w(float tt,float x)
{
	return -2*x/(3*tt+30);
}
float f(float w,float w0,float w1,float w2)
{
	return w-w1+1.5*t*(((w2-w0)*(w2-w0)/(4*h*h))+w1*(w0-2*w1+w2)/(h*h));
}

float df0(float w0,float w1,float w2)
{
	return 1.5*t*(-0.5*(w2-w0)+w1)/(h*h);
}
float df1(float w0,float w1,float w2)
{
	return -1+1.5*t*(w0+w2-4*w1)/(h*h);
}
float df2(float w0,float w1,float w2)
{
	return 1.5*t*(0.5*(w2-w0)+w1)/(h*h);
}

void solve(int num)
{
int N=H;
vector<float> x(N);
vector<vector<float>> matrix(N,N+1);
bool cont=true;

while(cont==true){
	cont=false;

	for(int i=0;i<N;i++)
	{
		for(int j=0;j<=N;j++)
		{
			matrix[i][j]=0;
		}
	}	

	matrix[0][0]=1;
	matrix[0][N]=result[num][0];
	for(int i=1;i<N-1;i++)
	{
		matrix[i][i-1]=df0(result[num][i-1],result[num][i],result[num][i+1]);
		matrix[i][i]=df1(result[num][i-1],result[num][i],result[num][i+1]);
		matrix[i][i+1]=df2(result[num][i-1],result[num][i],result[num][i+1]);
		matrix[i][N]=-f(result[num-1][i],result[num][i-1],result[num][i],result[num][i+1]);
	}
	matrix[N-1][N-1]=1;
	matrix[N-1][N]=result[num][N-1];
	
vector<int> i_indexes;
vector<int> indexes;
vector<vector<int>> back(N,2);


for(int i=0;i<N;i++)
		{
			indexes.push_back(i);
			i_indexes.push_back(i);
		}

 int nn;
  int myid;
  int numprocs;
  int i;
  int rc;
  
  nn = 3;

  float max;
	int max_i,max_j;
	int n=N;
	for(int m=0;m<N-1;m++)
	{
		n--;
		max=0;
		int k;
		

		for(int i=0;i<=n;i++)
		{
			k=indexes[i];
			for(int j=0;j<N;j++)
			{    
				if(abs(matrix[k][j])>max)
				{
					max=abs(matrix[k][j]);
					max_i=k;
					max_j=j;
				}
			}
		}
		back[m][0]=max_i;
		back[m][1]=max_j;
		indexes.erase(remove(indexes.begin(),indexes.end(),max_i),indexes.end());
		i_indexes.erase(remove(i_indexes.begin(),i_indexes.end(),max_j),i_indexes.end());
		
		float K;

 MPI_Status st;
		
MPI_Comm_size(MPI_COMM_WORLD,&numprocs); /*DETERMINE TOTAL NUMBER OF PROCESSORS*/
MPI_Comm_rank(MPI_COMM_WORLD,&myid); /*DETERMINE RANK OF THIS PROCESSOR*/

if(myid == 0)
     { 
		 int n3=n/3;
		 int ind[2];
		 ind[0]=0;
		 ind[1]=n3;
       MPI_Send(ind,2,MPI_INT,1,15,MPI_COMM_WORLD);
	    ind[0]=n3+1;
		 ind[1]=2*n3;
       MPI_Send(&ind,2,MPI_INT,2,15,MPI_COMM_WORLD);
	    ind[0]=2*n3+1;
		 ind[1]=n;
       MPI_Send(&ind,2,MPI_INT,3,15,MPI_COMM_WORLD);
     }
    else
     { 
        
			int b[2];
			MPI_Recv(b,2,MPI_INT,0,15,MPI_COMM_WORLD,&st);
			for(int i=b[0];i<b[1];i++)
		{
			k=indexes[i];
			//cout<<"  k  "<<k<<"  ";
			K=matrix[k][max_j]/matrix[max_i][max_j];
			
			for(int j=0;j<=N;j++)
			{
				matrix[k][j]-=K*matrix[max_i][j];
			}
			}   
     }
}
back[N-1][0]=indexes[0];
back[N-1][1]=i_indexes[0];
for(int i=N-1;i>=0;i--)
	{
		int k=back[i][0];
		int m=back[i][1];
		x[m]=matrix[k][N];
		for(int j=0;j<m;j++)
		{
			x[m]-=x[j]*matrix[k][j];
		}
		for(int j=m+1;j<N;j++)
		{
			x[m]-=x[j]*matrix[k][j];
		}
		x[m]=x[m]/matrix[k][m];
	}
for(int i=0;i<H-2;i++)
{
	result[num][i+1]+=x[i+1];
	if(abs(x[i])>e)
	{
		cont=true;
	}
	}
}
}



int main(int argc, char *argv[])
{
	for(int i=0;i<T;i++)
	{
		result[i][0]=w(tmin+i*t,hmin);
		result[i][H-1]=w(tmin+i*t,hmax);
	}

	for(int i=1;i<H;i++)
	{
		result[0][i]=w(tmin,hmin+i*h);
	}
	
   int rc;
 	 if (rc= MPI_Init(&argc, &argv)) 
  { 
    cout << "Ошибка запуска, выполнение остановлено " << endl;
    MPI_Abort(MPI_COMM_WORLD, rc);
  } 

	 for(int i=1;i<T/10;i++)
	{
		solve(i);
	}
	MPI_Finalize();
for(int i=1;i<20;i++)
	{
		for(int j=0;j<H;j++)
	{
		cout<<result[i][j]<<"  "<<w(tmin+i*t,hmin+j*h)<<"\n";
	}
		}
 system("pause");
return 0;
}

