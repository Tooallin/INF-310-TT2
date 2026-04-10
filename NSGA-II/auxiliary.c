/* Some utility functions (not part of the algorithm) */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "global.h"
# include "rand.h"

/* Function to return the maximum of two variables */
double maximum(double a, double b) {
	if (a > b) {
		return(a);
	}
	return (b);
}

/* Function to return the minimum of two variables */
double minimum(double a, double b) {
	if (a < b) {
		return (a);
	}
	return (b);
}

/* Function to find the start index and end index of a route */
void find_route_bounds(individual *ind, int route, int *start_index, int *end_index) {
	int i;
	int current = 0;
	int inside_route = 0;
	int start = -1, end = -1;

	for (i = 0; i < gene_length; i++) {
		if (ind->gene[i] == -1) {
			current++;
			inside_route = 0;
			continue;
		}
		if (current == route - 1) {
			if (!inside_route) {
				start = i;
				inside_route = 1;
			}
			end = i;
		}
		if (current > route - 1) break;
	}

	*start_index = start;
	*end_index = end;
	return;
}

/* Function to find the start index and end index of the last route */
void find_last_route_bounds(individual *ind, int *start_index, int *end_index) {
	int i;

	for (i = gene_length - 1; i >= 0; i--) {
		if (ind->gene[i] == -1) {
			
			if (i == gene_length - 1) {
				*start_index = -1;
				*end_index = -1;
			} else {
				*start_index = i + 1;
				*end_index = gene_length - 1;
			}
			return;
		}
	}

	if (gene_length == 0) {
		*start_index = -1;
		*end_index = -1;
	} else {
		*start_index = 0;
		*end_index = gene_length - 1;
	}

	return;
}