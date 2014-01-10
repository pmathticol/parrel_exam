/*
 * serial.c
 *
 *  Created on: 2013Äê12ÔÂ26ÈÕ
 *      Author: panzhizhou
 */

#include<stdio.h>
#include<time.h>
#define N 1000

int matrixA[N][N],matrixB[N][N],resultMatrix[N][N];
int tempB[N][N];

inline void generate(int A[][1000],int B[][1000])
{
	int i,j,k;
    srand(time(NULL));
    for(i=0;i<N;i++)
    	for(j=0;j<N;j++)
    	{
    		A[i][j]=rand()%100;
    		tempB[i][j]=rand()%100;
                B[j][i]=tempB[i][j];
    	}
}
int main()
{
	int i,j,k;
     memset(resultMatrix,0,sizeof(resultMatrix));
     generate(matrixA,matrixB);
     for(i=0;i<N;i++)
    	 for(j=0;j<N;j++)
    		 for(k=0;k<N;k++)
    			 resultMatrix[i][j]=resultMatrix[i][j]+matrixA[i][k]*matrixB[j][k];
    printf("the serial mutiply ,it costs %lf seconds\n",(double)clock()/CLOCKS_PER_SEC);
    return 0;
}

