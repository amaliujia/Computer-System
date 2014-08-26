/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    int i, j, k, q, temp1, temp2, temp3, temp4,temp5, temp6, temp7, temp8;
	if(M == 32 && N == 32){
		for(i = 0; i < M; i+=8){
			for(j = 0; j < N; j+=8){
				for(k = i; k <i+8; k++){
					for(q = j; q <j+8; q+=8){
						temp1 = A[k][q];
						temp2 = A[k][q+1];
						temp3 = A[k][q+2];
						temp4 = A[k][q+3];
                        temp5 = A[k][q+4];
                        temp6 = A[k][q+5];
                        temp7 = A[k][q+6];
                        temp8 = A[k][q+7];
						B[q][k] = temp1;
						B[q+1][k] = temp2;
						B[q+2][k] = temp3;
						B[q+3][k] = temp4;
                        B[q+4][k] = temp5;
                        B[q+5][k] = temp6;
                        B[q+6][k] = temp7;
                        B[q+7][k] = temp8;	
					}
				}
			}
		}	
	}else if(M == 64 && N == 64){
        for(i = 0; i < M; i+=4){
            for(j = 0; j < N; j+=4){
                for(k = i; k < i+4; k++){
						temp1 = A[k][j];
                        temp2 = A[k][j+1];
                        temp3 = A[k][j+2];
                        temp4 = A[k][j+3];
                        B[j][k] = temp1;
                        B[j+1][k] = temp2;
                        B[j+2][k] = temp3;
                        B[j+3][k] = temp4;
				}
            }
        }	
	}else if(M == 61 && N == 67){
        for(i = 0; i < M; i += 8){
            for(j = 0; j < N; j += 8){
                for(q = j; q < j + 8 && q < N; q++){
                   if(i + 8 < M){
						temp1 = A[q][i];
						temp2 = A[q][i+1];
						temp3 = A[q][i+2];
						temp4 = A[q][i+3];
						temp5 = A[q][i+4];
						temp6 = A[q][i+5];
						temp7 = A[q][i+6];
						temp8 = A[q][i+7];
						B[i][q] = temp1;
						B[i+1][q] = temp2;
						B[i+2][q] = temp3;
						B[i+3][q] = temp4;
						B[i+4][q] = temp5;
						B[i+5][q] = temp6;
						B[i+6][q] = temp7;
						B[i+7][q] = temp8;	
					}else{
						for(k = i; k < M; k++){
                     		temp1 = A[q][k];
                     		B[k][q] = temp1;
						}
					}
                }
            }
        }
    }	
	
    ENSURES(is_transpose(M, N, A, B));
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

