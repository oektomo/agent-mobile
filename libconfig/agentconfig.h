#ifndef AGENTCONFIG_H
#define AGENTCONFIG_H
#include <stdint.h>

typedef struct{
int8_t *Array;
int A_col;
int A_row;
} arraycontainTypedef;

void read2DimArray(config_t *cf, int8_t *A_array, const char *lookStr);

#endif
