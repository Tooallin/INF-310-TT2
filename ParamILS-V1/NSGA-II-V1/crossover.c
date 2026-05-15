/* Crossover routines */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "global.h"
# include "rand.h"

/* Function to cross two individuals */
void crossover(individual *parent1, individual *parent2, individual *child1, individual *child2, problem_instance *pi) {
	rbx_crossover(parent1, parent2, child1);
	rbx_crossover(parent2, parent1, child2);
	return;
}

/* Function to cross two individuals using the RBX operator */
void rbx_crossover(individual *parent1, individual *parent2, individual *child) {
	int *used = (int *)calloc(gene_length, sizeof(int));
	int *gene = child->gene;
	int selected_route;
	int start, end;
	int max_attempts;
	int attempts;
	int i, j, k;
	max_attempts = 10;
	attempts = 0;

	do {
		selected_route = rnd(1, n_routes);
		find_route_bounds(parent1, selected_route, &start, &end);
		attempts++;
	} while (start == -1 && end == -1 && attempts < max_attempts);

	if (attempts == max_attempts) {
		free(used);
		return;
	}

	i = 0;

	for (j = start; j <= end; j++) {
		gene[i++] = parent1->gene[j];
		used[parent1->gene[j]-1] = 1;
	}
	gene[i++] = -1;

	for (j = 1; j <= n_routes; j++) {
		if (j == selected_route) continue;
		find_route_bounds(parent2, j, &start, &end);
		
		int nodes_added = 0;
		
		for (k = start; k <= end; k++) {
			if (start == -1) break;
			if (used[parent2->gene[k]-1]) continue;
			
			gene[i++] = parent2->gene[k];
			used[parent2->gene[k]-1] = 1;
			nodes_added++;
		}

		if (nodes_added == 0) {
			for (int v = 0; v < gene_length - n_routes; v++) {
				if (!used[v]) {
					gene[i++] = v + 1;
					used[v] = 1;
					break;
				}
			}
		}

		gene[i++] = -1;
	}

	for (j = 0; j < gene_length; j++) {
		if (i == gene_length) break;
		if (!used[j]) {
			gene[i++] = j+1;
		}
	}

	free(used);
	return;
}