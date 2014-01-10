#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include"mpi.h"

#define N 1000
#define NON_ZERO_NUM 100*100

int matrix_A[N][N],matrix_B[N][N],matrix_C[N][N];
int sparse_matrix_A[N*N];

void sparse_matrix_generate(int matrix_x[][N],int num)
{
	//	����ϡ�����
	//	numΪ����Ԫ�صĸ���
    int i,m,n;
	srand((unsigned int)time(NULL));
    for(i=0;i<num;i++)
	{
		m = rand()%N;
		n = rand()%N;
		if(matrix_x[m][n])
		//�����ǰԪ�ز�Ϊ0��˵��������ɵģ�m��n����֮ǰĳһ�ε�(m,n)��ͬ,����Ҫ�������ɣ�m��n��
			i -= 1;
		else
			matrix_x[m][n] = rand()%N +1; //���������Ϊ[1,N]				       
	}	
}

void matrix_generate(int matrix_x[][N])
{
	//	���ɾ���
    int i,j;
	srand((unsigned int)time(NULL));
    for(i=0;i<N;i++)
		for(j=0;j<N;j++)
			matrix_x[i][j] = rand()%N +1; //���������Ϊ[1,N]				       
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
	// ���´洢ϡ�����
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
			}
		}
	}
	sparse_matrix_A[1] = count;
}

int main(int argc,char *argv[])
{
	int process_size,process_rank;
	int i,j;
	double starttime,endtime;

	MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&process_rank);

    if(process_rank ==0)
	{
		sparse_matrix_generate(matrix_A,N);
      	matrix_generate(matrix_B);
		matrix_restore(); // ���´洢����
		starttime = MPI_Wtime();
	
		for(i=2;i<sparse_matrix_A[1];i=i+3)
		{
			for(j=0;j<N;j++)
			{
				int row = sparse_matrix_A[i+1];
				int column = sparse_matrix_A[i+2];
				matrix_C[row][j] += sparse_matrix_A[i] * matrix_B[column][j];
			}
		}
    	endtime=MPI_Wtime();
		printf("sprase matrix A[%d][%d]*B[%d][%d] ,it have norzero number of %d \n,sprase serial it costs %fs\n",N,N,N,N,NON_ZERO_NUM,endtime - starttime);
	}
	MPI_Finalize();

	return 0;
}
