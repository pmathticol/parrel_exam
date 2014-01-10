/*
 * matrix_pallel_algo1.c
 *
 *  Created on: 2013年12月26日
 *      Author: panzhizhou
 *
 *      简单行分块算法，主从模式。
 *      假设有p个子节点对应p个进程，把矩阵A分成rowA/p 块，这些块同时和B进行运算
 *      用 MPI_Scatter或MPI_Bcast分发数据
 *      用 MPI_Gather收集结果,同时注意同步传输数据
 *      这是最简单的矩阵行分块算法
 *
 */
#include<stdio.h>
#include<time.h>
#include "mpi.h"
#define N 1000
int matrixA[N][N],matrixB[N][N],resultMatrix[N][N];
//int tempB[N][N];
inline void generate(int A[][1000],int B[][1000])
{
	int i,j,k;
    srand(time(NULL));
    for(i=0;i<N;i++)
    	for(j=0;j<N;j++)
    	{
    		A[i][j]=rand()%100;
    		B[i][j]=rand()%100;
    		//tempB[j][i]=B[i][j];
    	}
    for(i=0;i<N;i++)
      for(j=0;j<N;j++)
       {
         int temp=B[i][j];
         B[i][j]=B[j][i];
         B[j][i]=temp;   
       }
 //   memset(resultMatrix,0,sizeof(resultMatrix));
}
int main(int argc,char *argv[])
{
	    int rank_id;
	    int proc_num;
	    MPI_Init(&argc,&argv);
	    int proc_name_length;
	    char processor_name[MPI_MAX_PROCESSOR_NAME];
	    MPI_Comm_size(MPI_COMM_WORLD,&proc_num);
	    MPI_Comm_rank(MPI_COMM_WORLD,&rank_id);
	    MPI_Get_processor_name(processor_name,&proc_name_length);
	    double start_time,end_time;
	    int block_num;
	    int row;

	    if(rank_id==0){  //分发A
	    	generate(matrixA,matrixB);
	    	start_time=MPI_Wtime();
	    }
	    //开始分配数据
	    block_num=N/proc_num;
	    int matrix_tempA[block_num][N];
	    int matrix_tempC[block_num][N];
	    if(rank_id!=proc_num-1) //不是最后一个进程，则发送block_num*N个数据
	    {
	    	 row=block_num;
	    	 MPI_Scatter(&matrixA[rank_id*block_num][0],block_num*N,MPI_INT,&matrix_tempA[0][0],block_num*N,
	    			MPI_INT,0,MPI_COMM_WORLD );
	    }
	    else   //最后一个进程，分得A的行数比较特殊
	    {
	    	 row=N-((proc_num-1)*block_num);
	    	 MPI_Scatter(&matrixA[rank_id*block_num][0],row*N,MPI_INT,&matrix_tempA[0][0],row*N,
	    		    			MPI_INT,0,MPI_COMM_WORLD );
	    }
	    //开始分发B，所有行都发
          int ncount=N*N;
          MPI_Bcast(&matrixB[0][0],ncount,MPI_INT,0,MPI_COMM_WORLD);
	  //  MPI_Scatter(&matrixB[0][0],N*N,MPI_INT,&matrix_tempB[0][0],N*N,
	   //	    		    			MPI_INT,0,MPI_COMM_WORLD );
	    //开始同时计算局部，相当于一个  row*N 和N*N的矩阵相乘，这里因为转置了B,所以速度更快
	    int  i,j,k;
	    for(i=0;i<row;i++)
	    	for(j=0;j<N;j++)
                  {
                        matrix_tempC[i][j]=0;
	    		for(k=0;k<N;k++)
	    			matrix_tempC[i][j]+=(matrix_tempA[i][j]*matrixB[j][k]);   //注意这里循环的顺序
                  }
	    //开始收集数据，注意同步数据
	    MPI_Barrier(MPI_COMM_WORLD);//同步
	    MPI_Gather(&matrix_tempC[0][0],row*N,MPI_INT,&resultMatrix[rank_id*block_num][0],row*N,
	    		MPI_INT,0,MPI_COMM_WORLD);
				
	    if(rank_id==0)   //结束了，开始计算时间
	    {
	    	end_time=MPI_Wtime();
	        printf("matrix parrel mutply,it costs %fs\n", end_time-start_time);
	    }
}

