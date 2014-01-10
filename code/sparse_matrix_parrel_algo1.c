#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"mpi.h"

#define N 1000
#define NUM 4
#define NON_ZERO_NUM 100*100

int matrix_A[N][N],matrix_B[N][N],matrix_C[N][N];
int sparse_matrix_A[N*N];
int block_C[N/NUM][N];
int processArray[NUM];

void sparse_matrix_generate(int matrix_x[][N],int num)
{
	//	生成稀疏矩阵
	//	num为非零元素的个数
    int i,m,n;
	srand((unsigned int)time(NULL));
    for(i=0;i<num;i++)
	{
		m = rand()%N;
		n = rand()%N;
		if(matrix_x[m][n])
		//如果当前元素不为0，说明这次生成的（m，n）和之前某一次的(m,n)相同,则需要重新生成（m，n）
			i -= 1;
		else
			matrix_x[m][n] = rand()%N +1; //随机数区间为[1,N]				       
	}	
}

void matrix_generate(int matrix_x[][N])
{
	//	生成矩阵
    int i,j;
	srand((unsigned int)time(NULL));
    for(i=0;i<N;i++)
		for(j=0;j<N;j++)
			matrix_x[i][j] = rand()%N +1; //随机数区间为[1,N]				       
}

void printf_matrix(int matrix_x[][N])
{
	int i,j;
	printf("\n");
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
			printf("%4d",matrix_x[i][j]);
		printf("\n");
	}
}

void matrix_restore()
{
	// 重新存储稀疏矩阵
	int i,j,count=2;
	sparse_matrix_A[0] = N;
	for(i=0;i<N;i++)
	{
		for(j=0;j<N;j++)
		{
			if(matrix_A[i][j] != 0)
			{
				sparse_matrix_A[count++] = matrix_A[i][j];
				sparse_matrix_A[count++] = i;
				sparse_matrix_A[count++] = j;
				processArray[i/(N/NUM)]++;
			}
		}
	}
	sparse_matrix_A[1] = count;
}

int main(int argc,char *argv[])
{
	int process_size,process_rank;
	double starttime,endtime;

	MPI_Init(&argc,&argv);
	MPI_Comm_size(MPI_COMM_WORLD,&process_size);
    MPI_Comm_rank(MPI_COMM_WORLD,&process_rank);

    if(process_rank ==0)
	{
		sparse_matrix_generate(matrix_A,N);
      	matrix_generate(matrix_B);
		matrix_restore(); // 重新存储矩阵
		starttime = MPI_Wtime();
	}

    MPI_Bcast(&processArray,NUM,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&sparse_matrix_A[1],1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&sparse_matrix_A,sparse_matrix_A[1],MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&matrix_B,N*N,MPI_INT,0,MPI_COMM_WORLD);

    int head =2,tail,i,j,row,column;
	for(i=0;i<process_rank;i++)
		head += 3*processArray[i];
	tail = head + 3*processArray[process_rank];

	for(i=head;i<=tail;i+=3)
	{
		for(j=0;j<N;j++)
		{
			row = sparse_matrix_A[i+1]%(N/NUM);
			column = sparse_matrix_A[i+2];
			block_C[row][j] += sparse_matrix_A[i] * matrix_B[column][j];
		}
	}

	MPI_Barrier(MPI_COMM_WORLD);//同步 
    MPI_Gather(&block_C,N*NUM,MPI_INT,&matrix_C,N*NUM,MPI_INT,0,MPI_COMM_WORLD);
    if(process_rank == 0)
	{
         endtime=MPI_Wtime();
         printf("稀疏矩阵A[%d][%d]*B[%d][%d]（非零元素分别为%d）\n%d个process并行耗时:%f:\n",N,N,N,N,NON_ZERO_NUM,NUM,endtime - starttime);
    }
	MPI_Finalize();

	return 0;
}
