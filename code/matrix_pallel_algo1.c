/*
 * matrix_pallel_algo1.c
 *
 *  Created on: 2013��12��26��
 *      Author: panzhizhou
 *
 *      ���зֿ��㷨������ģʽ��
 *      ������p���ӽڵ��Ӧp�����̣��Ѿ���A�ֳ�rowA/p �飬��Щ��ͬʱ��B��������
 *      �� MPI_Scatter��MPI_Bcast�ַ�����
 *      �� MPI_Gather�ռ����,ͬʱע��ͬ����������
 *      ������򵥵ľ����зֿ��㷨
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

	    if(rank_id==0){  //�ַ�A
	    	generate(matrixA,matrixB);
	    	start_time=MPI_Wtime();
	    }
	    //��ʼ��������
	    block_num=N/proc_num;
	    int matrix_tempA[block_num][N];
	    int matrix_tempC[block_num][N];
	    if(rank_id!=proc_num-1) //�������һ�����̣�����block_num*N������
	    {
	    	 row=block_num;
	    	 MPI_Scatter(&matrixA[rank_id*block_num][0],block_num*N,MPI_INT,&matrix_tempA[0][0],block_num*N,
	    			MPI_INT,0,MPI_COMM_WORLD );
	    }
	    else   //���һ�����̣��ֵ�A�������Ƚ�����
	    {
	    	 row=N-((proc_num-1)*block_num);
	    	 MPI_Scatter(&matrixA[rank_id*block_num][0],row*N,MPI_INT,&matrix_tempA[0][0],row*N,
	    		    			MPI_INT,0,MPI_COMM_WORLD );
	    }
	    //��ʼ�ַ�B�������ж���
          int ncount=N*N;
          MPI_Bcast(&matrixB[0][0],ncount,MPI_INT,0,MPI_COMM_WORLD);
	  //  MPI_Scatter(&matrixB[0][0],N*N,MPI_INT,&matrix_tempB[0][0],N*N,
	   //	    		    			MPI_INT,0,MPI_COMM_WORLD );
	    //��ʼͬʱ����ֲ����൱��һ��  row*N ��N*N�ľ�����ˣ�������Ϊת����B,�����ٶȸ���
	    int  i,j,k;
	    for(i=0;i<row;i++)
	    	for(j=0;j<N;j++)
                  {
                        matrix_tempC[i][j]=0;
	    		for(k=0;k<N;k++)
	    			matrix_tempC[i][j]+=(matrix_tempA[i][j]*matrixB[j][k]);   //ע������ѭ����˳��
                  }
	    //��ʼ�ռ����ݣ�ע��ͬ������
	    MPI_Barrier(MPI_COMM_WORLD);//ͬ��
	    MPI_Gather(&matrix_tempC[0][0],row*N,MPI_INT,&resultMatrix[rank_id*block_num][0],row*N,
	    		MPI_INT,0,MPI_COMM_WORLD);
				
	    if(rank_id==0)   //�����ˣ���ʼ����ʱ��
	    {
	    	end_time=MPI_Wtime();
	        printf("matrix parrel mutply,it costs %fs\n", end_time-start_time);
	    }
}

