/* Crossover routines */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "global.h"
# include "rand.h"

/* Function to cross two individuals */
void crossover(individual *parent1, individual *parent2, individual *child1, individual *child2, problem_instance *pi) {
	double prob_type = randomperc();

	if (prob_type <= pcross_greedy) {
		erc_crossover(parent1, parent2, child1, pi);
		erc_crossover(parent2, parent1, child2, pi);
	} else {
		double prob_rand = randomperc();
		if (prob_rand <= 0.50) {
			subtour_crossover(parent1, parent2, child1);
			subtour_crossover(parent2, parent1, child2);
		}
		else {
			rbx_crossover(parent1, parent2, child1);
			rbx_crossover(parent2, parent1, child2);
		}
	}
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

/* Function to cross two individuals using the Sub-tour Exchange operator */
void subtour_crossover(individual *parent1, individual *parent2, individual *child) {
	int *used = (int *)calloc(gene_length, sizeof(int));
	int *gene = child->gene;
	int r, start, end, i, j, k;
	int selected_route;
	int attempts = 0;

	do {
		selected_route = rnd(1, n_routes);
		find_route_bounds(parent1, selected_route, &start, &end);
		attempts++;
	} while (start == -1 && end == -1 && attempts < 10);

	i = 0;
	
	for (r = 1; r <= n_routes; r++) {
		int nodes_added = 0;

		if (r == selected_route && start != -1 && end != -1) {
			int route_len = end - start + 1;
			int sub_len = rnd(1, route_len > 3 ? 3 : route_len); 
			int sub_start = start + rnd(0, route_len - sub_len);

			for (j = sub_start; j < sub_start + sub_len; j++) {
				gene[i++] = parent1->gene[j];
				used[parent1->gene[j]-1] = 1;
				nodes_added++;
			}
		}

		int p2_start, p2_end;
		find_route_bounds(parent2, r, &p2_start, &p2_end);
		
		if (p2_start != -1 && p2_end != -1) {
			for (k = p2_start; k <= p2_end; k++) {
				int poi = parent2->gene[k];
				if (!used[poi - 1]) {
					gene[i++] = poi;
					used[poi - 1] = 1;
					nodes_added++;
				}
			}
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

	for (j = 0; j < gene_length - n_routes; j++) { 
		if (!used[j] && i < gene_length) {
			gene[i++] = j + 1;
		}
	}

	free(used);
	return;
}

/* Function to cross two individuals using the Elitist Route Crossover (ERC) operator with RCL */
void erc_crossover(individual *parent1, individual *parent2, individual *child, problem_instance *pi) {
	int *used = (int *)calloc(gene_length, sizeof(int));
	int *gene = child->gene;
	int best_route = 1;
	int start, end;
	int i, j, k;

	int alternate_objective = (randomperc() < 0.5) ? 1 : 0;

	int rcl_size = (n_routes < 3) ? n_routes : 3; 
	int rcl_route[3] = {-1, -1, -1};
	double rcl_metric[3] = {-1.0, -1.0, -1.0};
	int rcl_count = 0;

	for (j = 1; j <= n_routes; j++) {
		find_route_bounds(parent1, j, &start, &end);
		if (start == -1 || end == -1) continue;

		int current_score = 0;
		int num_nodes = end - start + 1;

		for (k = start; k <= end; k++) {
			current_score += pi->set_POI[parent1->gene[k]-1].SCORE;
		}

		double current_metric;
		if (alternate_objective) {
			current_metric = (double)current_score;
		} else {
			current_metric = (double)current_score / (double)num_nodes; 
		}

		if (rcl_count < rcl_size) {
			rcl_route[rcl_count] = j;
			rcl_metric[rcl_count] = current_metric;
			rcl_count++;
		} else {
			int worst_rcl_idx = -1;
			double min_in_rcl = 9999999.0;
			for (int r = 0; r < rcl_size; r++) {
				if (rcl_metric[r] < min_in_rcl) {
					min_in_rcl = rcl_metric[r];
					worst_rcl_idx = r;
				}
			}
			if (current_metric > min_in_rcl) {
				rcl_route[worst_rcl_idx] = j;
				rcl_metric[worst_rcl_idx] = current_metric;
			}
		}
	}

	if (rcl_count > 0) {
		int selected = rnd(0, rcl_count - 1);
		best_route = rcl_route[selected];
	} else {
		best_route = rnd(1, n_routes);
	}

	find_route_bounds(parent1, best_route, &start, &end);
	i = 0;

	if (start != -1 && end != -1) {
		for (j = start; j <= end; j++) {
			gene[i++] = parent1->gene[j];
			used[parent1->gene[j]-1] = 1;
		}
	}
	gene[i++] = -1;

	for (j = 1; j <= n_routes; j++) {
		if (j == best_route) continue;
		
		find_route_bounds(parent2, j, &start, &end);
		int nodes_added = 0;
		
		if (start != -1 && end != -1) {
			for (k = start; k <= end; k++) {
				if (used[parent2->gene[k]-1]) continue;
				gene[i++] = parent2->gene[k];
				used[parent2->gene[k]-1] = 1;
				nodes_added++;
			}
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

	for (j = 0; j < gene_length - n_routes; j++) {
		if (!used[j] && i < gene_length) {
			gene[i++] = j+1;
		}
	}

	free(used);
	return;
}