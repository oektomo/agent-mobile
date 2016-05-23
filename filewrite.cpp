#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
	FILE *f = fopen("file.txt", "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}

	/* print some text */
	const char *text = "Write this to the file";
	fprintf(f, "Some text: %s\n", text);

	/* print integers and floats */
	int i = 1;
	float py = 3.1415927;
	fprintf(f, "Integer: %d, float: %f\n", i, py);

	/* printing single chatacters */
	char c = 'A';
	fprintf(f, "A character: %c\n", c);
	fclose(f);

	f = fopen("file.txt", "a");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    exit(1);
	}
	fprintf(f,"\nhahaha\n");
	while(1);
	fclose(f);


return 0;
}
