/* Memory allocation and deallocation routines */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "global.h"
# include "rand.h"

/* Function to allocate memory to a population */
void allocate_memory_pop(population *pop, int size) {
	int i;
	pop->ind = (individual *)malloc(size*sizeof(individual));
	for (i=0; i<size; i++) {
		allocate_memory_ind(&(pop->ind[i]));
	}
	return;
}

/* Function to allocate memory to an individual */
void allocate_memory_ind(individual *ind) {
	ind->gene = (int *)malloc(gene_length * sizeof(int));
	ind->obj = (double *)malloc(n_objectives * sizeof(double));
	ind->constr = (double *)malloc(n_constraints * sizeof(double));
	return;
}

/* Function to deallocate memory to a population */
void deallocate_memory_pop(population *pop, int size) {
	int i;
	for (i = 0; i < size; i++) {
		deallocate_memory_ind(&(pop->ind[i]));
	}
	free(pop->ind);
	return;
}

/* Function to deallocate memory to an individual */
void deallocate_memory_ind(individual *ind) {
	free(ind->gene);
	free(ind->obj);
	free(ind->constr);
	return;
}
