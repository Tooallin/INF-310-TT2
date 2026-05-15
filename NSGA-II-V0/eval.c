/* Routine for evaluating population members  */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "global.h"
# include "rand.h"

/* Routine to evaluate objective function values and constraints for a population */
void evaluate_pop(population *pop, problem_instance *pi) {
	int i;
	for (i = 0; i < popsize; i++) {
		evaluate_ind(&(pop->ind[i]), pi);
	}
	return;
}

/* Routine to evaluate objective function values and constraints for an individual */
void evaluate_ind(individual *ind, problem_instance *pi) {
	int i;

	objective_function_one(ind, pi);
	objective_function_two(ind, pi);
	objective_function_three(ind, pi);

	constraint_one(ind, pi);
	constraint_two(ind, pi);
	constraint_three(ind, pi);

	ind->constr_violation = 0.0;
	for (i = 0; i < n_constraints; i++) {
		if (ind->constr[i] < 0.0) {
			ind->constr_violation += ind->constr[i];
		}
	}

	return;
}

/* Routine to evaluate objective function 1: Maximize Total Score */
void objective_function_one(individual *ind, problem_instance *pi) {
	int i, j;

	ind->obj[0] = 0;
	j = 0;

	for (i = 1; i <= n_routes; i++) {
		while (ind->gene[j] != -1) {
			ind->obj[0] -= pi->set_POI[ind->gene[j]-1].SCORE;
			j++;
		}
		j++;
	}
	return;
}

/* Routine to evaluate objective function 2: Minimize Total Travel Time */
void objective_function_two(individual *ind, problem_instance *pi) {
	int i, j;
	double temp;
	int poi_actual, poi_anterior;

	ind->obj[1] = 0;
	j = 0;

	for (i = 1; i <= n_routes; i++) {
		temp = 0.0;

		if (j >= gene_length) break;

		if (ind->gene[j] == -1) {
			j++;
			continue;
		}

		poi_actual = ind->gene[j];
		temp += pi->param_t[pi->param_o.id][poi_actual] + pi->set_POI[poi_actual - 1].TT;
		poi_anterior = poi_actual;
		j++;

		while (j < gene_length && ind->gene[j] != -1) {
			poi_actual = ind->gene[j];
			temp += pi->param_t[poi_anterior][poi_actual] + pi->set_POI[poi_actual - 1].TT;
			poi_anterior = poi_actual;
			j++;
		}

		if (j > 0 && ind->gene[j - 1] != -1) {
			temp += pi->param_t[poi_anterior][pi->param_s.id];
		}

		ind->obj[1] += temp;

		if (j < gene_length) {
			j++;
		}
	}
	return;
}

/* Routine to evaluate objective function 3: Minimize Variance between Maximum and Minimum Total Scores in Routes */
void objective_function_three(individual *ind, problem_instance *pi) {
	int i, j;
	int min, max;
	int temp;

	ind->obj[2] = 0;
	min = 99999999; 
	max = -1;
	j = 0;

	for (i = 1; i <= n_routes; i++) {
		temp = 0;

		if (j >= gene_length) {
			break; 
		}

		while (j < gene_length && ind->gene[j] != -1) {
			temp += pi->set_POI[ind->gene[j]-1].SCORE;
			j++;
		}

		if (j < gene_length) {
			j++;
		}

		if (temp >= max) {
			max = temp;
		}

		if (temp <= min) {
			min = temp;
		}
	}

	ind->obj[2] += abs(max - min);
	return;
}

/* Routine to evaluate constraint 1: Ensure maximum coverage for each category */
void constraint_one(individual *ind, problem_instance *pi) {
	int i, j, k;
	int start, end;
	double temp, visited;

	ind->constr[0] = 0.0;

	for (i = 0; i < pi->set_Z; i++) {
		temp = 0;

		for (j = 1; j <= n_routes; j++) {
			find_route_bounds(ind, j, &start, &end);
			if (start == -1 || end == -1) continue;
			visited = 0;

			for (k = start; k <= end; k++) {
				if (pi->set_POI[ind->gene[k]-1].e[i] == 1) visited += 1;
			}
			temp += visited;
		}

		if (temp > pi->param_E[i]) {
			ind->constr[0] += pi->param_E[i] - temp;
		}
	}

	return;
}

/* Routine to evaluate constraint 2: Time Windows Compliance */
void constraint_two(individual *ind, problem_instance *pi) {
	int i, j;
	int start, end;
	double temp;
	int poi_actual;

	ind->constr[1] = 0.0;

	for (i = 1; i <= n_routes; i++) {
		find_route_bounds(ind, i, &start, &end);
		if (start == -1 || end == -1) continue;

		poi_actual = ind->gene[start];
		temp = pi->param_t[pi->param_o.id][poi_actual];

		for (j = start; j <= end; j++) {
			poi_actual = ind->gene[j];

			if (temp < pi->set_POI[poi_actual-1].OT) {
				ind->constr[1] += temp - pi->set_POI[poi_actual-1].OT;
				
				temp = pi->set_POI[poi_actual-1].OT; 
			}
			else if (temp > pi->set_POI[poi_actual-1].CT) {
				ind->constr[1] += pi->set_POI[poi_actual-1].CT - temp;
			}

			temp += pi->set_POI[poi_actual-1].TT;

			if (j < end) {
				temp += pi->param_t[poi_actual][ind->gene[j+1]];
			} else {
				temp += pi->param_t[poi_actual][pi->param_s.id];
			}
		}

		if (temp < pi->param_s.OT) {
			ind->constr[1] += temp - pi->param_s.OT;
		} else if (temp > pi->param_s.CT) {
			ind->constr[1] += pi->param_s.CT - temp;
		}
	}

	return;
}

/* Routine to evaluate constraint 3: Ensure maximum travel time is not exceeded */
void constraint_three(individual *ind, problem_instance *pi) {
	int i, j;
	double temp;
	int poi_actual, poi_anterior;

	ind->constr[2] = 0.0;
	j = 0;

	for (i = 1; i <= n_routes; i++) {
		temp = 0.0;

		if (j >= gene_length) break;

		if (ind->gene[j] == -1) {
			j++;
			continue;
		}

		poi_actual = ind->gene[j];
		temp += pi->param_t[pi->param_o.id][poi_actual] + pi->set_POI[poi_actual - 1].TT;
		poi_anterior = poi_actual;
		j++;

		while (j < gene_length && ind->gene[j] != -1) {
			poi_actual = ind->gene[j];
			temp += pi->param_t[poi_anterior][poi_actual] + pi->set_POI[poi_actual - 1].TT;
			poi_anterior = poi_actual;
			j++;
		}

		if (j > 0 && ind->gene[j - 1] != -1) {
			temp += pi->param_t[poi_anterior][pi->param_s.id];
		}

		if (temp > pi->param_TM) {
			ind->constr[2] += pi->param_TM - temp;
		}

		if (j < gene_length) {
            j++;
        }
	}
	return;
}