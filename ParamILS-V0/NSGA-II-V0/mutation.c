/* Mutation routines */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include "global.h"
# include "rand.h"

/* Function to perform mutation in a population */
void mutation_pop(population *pop) {
	int i;
	for (i = 0; i < popsize; i++) {
		mutation_ind(&(pop->ind[i]));
	}
	return;
}

/* Function to perform mutation of an individual */
void mutation_ind(individual *ind) {
	int choice;
	double prob;

	prob = randomperc();

	if (prob <= pmut_bin) {
		choice = rnd(1, 4);

		if (choice == 1) {
			ars_mutation(ind);
		} else if (choice == 2) {
			ers_mutation(ind);
		} else if (choice == 3) {
			insert_mutation(ind);
		} else {
			remove_mutation(ind);
		}

		nbinmut++;
	}

	return;
}

/* Routine for intra route swap mutation */
void ars_mutation(individual *ind) {
	int start, end;
	int poi1, poi2, temp;
	int route;
	
	route = rnd(1, n_routes);

	find_route_bounds(ind, route, &start, &end);
	if (start == -1 || end == -1 || end - start < 1) return;

	do {
		poi1 = rnd(start, end);
		poi2 = rnd(start, end);
	} while (poi1 == poi2);

	temp = ind->gene[poi1];
	ind->gene[poi1] = ind->gene[poi2];
	ind->gene[poi2] = temp;

	return;
}

/* Routine for inter route swap mutation */
void ers_mutation(individual *ind) {
	int start1, end1, start2, end2;
	int poi1, poi2, temp;
	int route1, route2;
	int max_attempts;
	int attempts;
	max_attempts = 10;
	attempts = 0;

	do {
		route1 = rnd(1, n_routes);
		route2 = rnd(1, n_routes);
		attempts++;
	} while (route1 == route2 && attempts < max_attempts);

	if (route1 == route2) return;

	find_route_bounds(ind, route1, &start1, &end1);
	find_route_bounds(ind, route2, &start2, &end2);

	if (start1 == -1 || end1 == -1 || end1 - start1 < 0) return;
	if (start2 == -1 || end2 == -1 || end2 - start2 < 0) return;

	poi1 = rnd(start1, end1);
	poi2 = rnd(start2, end2);

	temp = ind->gene[poi1];
	ind->gene[poi1] = ind->gene[poi2];
	ind->gene[poi2] = temp;

	return;
}

/* Routine for insert mutation */
void insert_mutation(individual *ind) {
	int start_route, end_route, start_unvisited, end_unvisited;
	int insert_pos, selected_pos, poi_to_insert;
	int i, k;
	int *new_gene;
	int route;

	find_last_route_bounds(ind, &start_unvisited, &end_unvisited);
	if (start_unvisited > end_unvisited) return;

	selected_pos = rnd(start_unvisited, end_unvisited);
	poi_to_insert = ind->gene[selected_pos];

	route = rnd(1, n_routes);
	find_route_bounds(ind, route, &start_route, &end_route);
	if (start_route == -1 || end_route == -1) return;

	insert_pos = rnd(start_route, end_route + 1);

	new_gene = (int *)malloc(gene_length * sizeof(int));
	k = 0;

	for (i = 0; i < gene_length; i++) {
		if (i == insert_pos) {
			new_gene[k++] = poi_to_insert;
		}
		if (i != selected_pos) {
			new_gene[k++] = ind->gene[i];
		}
	}

	while (k < gene_length) {
		new_gene[k++] = -1;
	}

	memcpy(ind->gene, new_gene, gene_length * sizeof(int));
	free(new_gene);
}

/* Routine for remove mutation */
void remove_mutation(individual *ind) {
	int start, end, start_unvisited, end_unvisited;
	int route = rnd(1, n_routes);
	int poi_to_remove, remove_index;
	int i, k;
	int *new_gene;

	find_route_bounds(ind, route, &start, &end);
	if (start == -1 || end == -1 || end - start < 0) return;

	remove_index = rnd(start, end);
	poi_to_remove = ind->gene[remove_index];

	find_last_route_bounds(ind, &start_unvisited, &end_unvisited);
	if (start_unvisited == -1 || end_unvisited == -1) return;

	new_gene = (int *)malloc(gene_length * sizeof(int));
	k = 0;

	for (i = 0; i < gene_length; i++) {
		if (i != remove_index) {
			new_gene[k++] = ind->gene[i];
		}
	}

	new_gene[k++] = poi_to_remove;

	while (k < gene_length) {
		new_gene[k++] = -1;
	}

	memcpy(ind->gene, new_gene, gene_length * sizeof(int));
	free(new_gene);
}