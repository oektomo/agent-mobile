#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <libconfig.h>
#define EXIT_FAILURE -1

int main(int argc, char **argv)
{
	if(argc != 2){
		printf("please specify the file.cfg name\n");
		return 0;
	}

	config_t cfg, *cf;
	
	cf = &cfg;
	config_init(cf);

	if(!config_read_file(cf, argv[1])) {
		fprintf(stderr, "%s:%d - %s\n",
		 config_error_file(cf),
		 config_error_line(cf),
		 config_error_text(cf));
		config_destroy(cf);
		return(EXIT_FAILURE);
	}

	const char *appWindowTitle = NULL;
	if(config_lookup_string(cf, "application.window.title", &appWindowTitle))
		printf("Window Title: %s\n", appWindowTitle);

	const char *ipAgent1 = NULL;
	if(config_lookup_string(cf, "ipAgent1", &ipAgent1))
		printf("IP Agent 1 = %s\n", ipAgent1);

	const config_setting_t *A_mat, *A_matRow, *A_element;
	const config_setting_t *A_mat2, *A_matRow2;
	int A_col, A_row, A_int;

	A_mat = config_lookup(cf, "Adjacency");
	A_row = config_setting_length(A_mat);
	A_matRow = config_setting_get_elem(A_mat, 0);
	A_col = config_setting_length(A_matRow); 
	printf("Adjacency mat %d row %d column\n",A_row, A_col);
	int8_t A_array[A_col][A_row];

	for(int A_rowCount = 0; A_rowCount < A_row; A_rowCount++) {
	    A_matRow = config_setting_get_elem(A_mat, A_rowCount);
	    A_col = config_setting_length(A_matRow); 
	   
		for(int A_colCount=0; A_colCount < A_col; A_colCount++) {
			A_int = config_setting_get_int_elem(A_matRow, A_colCount);
			A_array[A_rowCount][A_colCount] = A_int;
			printf("%d ",A_int);
		}
	    printf("\n");
	}
	printf("closing configuration file\n");
	config_destroy(cf);
	printf("printing matrix\n");
	for(int introw=0; introw<A_row; introw++) {
		for(int intcol=0; intcol<A_col; intcol++) {
		    printf("%d ",A_array[introw][intcol]);
		}
	    printf("\n");
	}
	return 0;
}
