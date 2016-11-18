#include <libconfig.h>
#include <stdio.h>

#include "agentconfig.h"

// reading 2 dimension array, unsafe
void read2DimArray(config_t *cf, arraycontainTypedef* arrayContainer, const char *lookStr)
{
	const config_setting_t *A_mat, *A_matRow, *A_element;
	const config_setting_t *A_mat2, *A_matRow2;
	int A_col, A_row, A_int;

	A_mat = config_lookup(cf, "Adjacency");
	A_row = config_setting_length(A_mat);
	A_matRow = config_setting_get_elem(A_mat, 0);
	A_col = config_setting_length(A_matRow); 
	//printf("Adjacency mat %d row %d column\n",A_row, A_col);
	arrayContainer->A_col = A_col;
	arrayContainer->A_row = A_row;

	for(int A_rowCount = 0; A_rowCount < A_row; A_rowCount++) {
	    A_matRow = config_setting_get_elem(A_mat, A_rowCount);
	    A_col = config_setting_length(A_matRow); 
	   
		for(int A_colCount=0; A_colCount < A_col; A_colCount++) {
			A_int = config_setting_get_int_elem(A_matRow, A_colCount);
			arrayContainer->Array[A_rowCount*4 + A_colCount] = A_int;
		//	printf("%d ",A_int);
		}
	//    printf("\n");
	}
}

void printMatrix(arraycontainTypedef* arrayContainer)
{

	for(int introw=0; introw<arrayContainer->A_row; introw++) {
		for(int intcol=0; intcol<arrayContainer->A_col; intcol++) {
			printf("%d ",arrayContainer->Array[introw*4 + intcol]);
		}
		printf("\n");
	}

}
