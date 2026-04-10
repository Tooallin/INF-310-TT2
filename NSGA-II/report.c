/* Routines for storing population data into files */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "global.h"
# include "rand.h"

/* Function to print the information of a population in a file */
void report_pop(population *pop, FILE *fpt) {
	int i, j;
	for (i = 0; i < popsize; i++) {
		for (j = 0; j < n_objectives; j++) {
			fprintf(fpt,"%e\t", pop->ind[i].obj[j]);
		}
		for (j = 0; j < n_constraints; j++) {
			fprintf(fpt,"%e\t", pop->ind[i].constr[j]);
		}
		for (j = 0; j < gene_length; j++) {
			fprintf(fpt,"%d\t", pop->ind[i].gene[j]);
		}
		fprintf(fpt,"%e\t", pop->ind[i].constr_violation);
		fprintf(fpt,"%d\t", pop->ind[i].rank);
		fprintf(fpt,"%e\n", pop->ind[i].crowd_dist);
	}
	return;
}

/* Function to print the information of feasible and non-dominated population in a file */
void report_feasible(population *pop, FILE *fpt) {
	int i, j;
	for (i = 0; i < popsize; i++) {
		if (pop->ind[i].constr_violation == 0.0 && pop->ind[i].rank == 1) {
			for (j = 0; j < n_objectives; j++) {
				fprintf(fpt,"%e\t", pop->ind[i].obj[j]);
			}
			for (j = 0; j < n_constraints; j++) {
				fprintf(fpt,"%e\t", pop->ind[i].constr[j]);
			}
			for (j = 0; j < gene_length; j++) {
				fprintf(fpt,"%d\t", pop->ind[i].gene[j]);
			}
			fprintf(fpt,"%e\t", pop->ind[i].constr_violation);
			fprintf(fpt,"%d\t", pop->ind[i].rank);
			fprintf(fpt,"%e\n", pop->ind[i].crowd_dist);
		}
	}
	return;
}
