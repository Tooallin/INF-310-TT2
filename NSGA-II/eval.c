/* Rutinas para evaluar a los miembros de la población (Objetivos y Factibilidad) */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Rutina envoltura para evaluar toda una población. */
void evaluate_pop(population *pop, problem_instance *pi) {
	int i;
	for (i = 0; i < popsize; i++) {
		evaluate_ind(&(pop->ind[i]), pi);
	}
	return;
}

/* * Rutina principal para evaluar a un individuo específico.
 * Calcula sus 3 objetivos y luego sus 3 restricciones.
 */
void evaluate_ind(individual *ind, problem_instance *pi) {
	int i;

	// Evaluación de las funciones objetivo
	objective_function_one(ind, pi);
	objective_function_two(ind, pi);
	objective_function_three(ind, pi);

	// Evaluación de las restricciones (factibilidad)
	constraint_one(ind, pi);
	constraint_two(ind, pi);
	constraint_three(ind, pi);

	// Consolida todas las violaciones en un solo valor de "castigo total".
	// Como los castigos se calculan como valores negativos, se suman.
	ind->constr_violation = 0.0;
	for (i = 0; i < n_constraints; i++) {
		if (ind->constr[i] < 0.0) {
			ind->constr_violation += ind->constr[i];
		}
	}

	return;
}

/* * OBJETIVO 1: Maximizar el puntaje total (Score).
 * Se restan los puntajes logrando un valor negativo porque NSGA-II minimiza por defecto.
 */
void objective_function_one(individual *ind, problem_instance *pi) {
	int i, j;
	ind->obj[0] = 0;
	j = 0; // Índice para recorrer el cromosoma de forma lineal
	
	for (i = 0; i < n_routes; i++) { 
		// Mientras no encontremos un delimitador (-1), sumamos el score del POI
		while (ind->gene[j] != -1) {
			ind->obj[0] -= pi->set_POI[ind->gene[j]-1].SCORE;
			j++;
		}
		j++; // Salta el delimitador -1 para pasar a la siguiente ruta
	}
	return;
}

/* * OBJETIVO 2: Minimizar el tiempo total empleado (Ecuación 2 del modelo).
 * Suma los tiempos de viaje (t_ij) y los tiempos de servicio (T_i) de todas las rutas.
 *  */
void objective_function_two(individual *ind, problem_instance *pi) {
	int i, j;
	double total_time = 0.0;
	int poi_actual, poi_anterior;

	j = 0;
	for (i = 0; i < n_routes; i++) {
		// Si la ruta está vacía (solo contiene el delimitador -1)
		if (ind->gene[j] == -1) {
			j++;
			continue;
		}

		poi_actual = ind->gene[j];
		// Suma el viaje desde el nodo de origen (o) hasta el primer POI + su tiempo de servicio
		total_time += pi->param_t[pi->param_o.id][poi_actual] + pi->set_POI[poi_actual - 1].TT;
		poi_anterior = poi_actual;
		j++;

		// Recorre los nodos intermedios de la ruta
		while (ind->gene[j] != -1 && j < gene_length) {
			poi_actual = ind->gene[j];
			// Suma el viaje desde el POI anterior al actual + el tiempo de servicio del actual
			total_time += pi->param_t[poi_anterior][poi_actual] + pi->set_POI[poi_actual - 1].TT;
			poi_anterior = poi_actual;
			j++;
		}

		// Suma el viaje de retorno desde el último POI de la ruta hacia el nodo final (s)
		if (j > 0 && ind->gene[j - 1] != -1) {
			total_time += pi->param_t[poi_anterior][pi->param_s.id];
		}

		j++; // Salta el delimitador -1
	}

	// Como NSGA-II minimiza por defecto, y el objetivo es minimizar, asignamos el valor directo.
	ind->obj[1] = total_time;
	
	return;
}

/* * OBJETIVO 3: Minimizar la brecha (diferencia) entre la ruta con mayor y menor puntaje.
 */
void objective_function_three(individual *ind, problem_instance *pi) {
	int i, j;
	int min, max;
	int temp;
	min = 10000;
	max = -10000;
	j = 0;
	
	for (i = 0; i < n_routes; i++) {
		temp = 0;
		while (ind->gene[j] != -1) {
			temp += pi->set_POI[ind->gene[j]-1].SCORE;
			j++;
		}
		j++;
		
		// Actualiza los valores de la ruta con mayor y menor puntaje
		if (temp >= max) {
			max = temp;
		}
		if (temp <= min) {
			min = temp;
		}
	}
	
	// El objetivo es la diferencia absoluta, que NSGA-II buscará llevar a 0
	ind->obj[2] = abs(max - min);
	return;
}

/* * RESTRICCIÓN 1 (Eq. 9 del modelo): No superar la cuota máxima por categoría E_z.
 */
void constraint_one(individual *ind, problem_instance *pi) {
	int i, j, k;
	int start, end;
	double temp, visited;
	ind->constr[0] = 0.0;

	for (i = 0; i < pi->set_Z; i++) {
		temp = 0;

		// find_route_bounds requiere índices de ruta base 1
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
			ind->constr[0] += pi->param_E[i] - temp; // Castigo negativo
		}
	}

	return;
}

/* * RESTRICCIÓN 2 (Eq. 11 del modelo): Respetar el tiempo máximo T_max por ruta.
 */
void constraint_two(individual *ind, problem_instance *pi) {
	int i, j;
	double temp;
	int poi_actual, poi_anterior;

	ind->constr[1] = 0.0;
	j = 0;

	for (i = 0; i < n_routes; i++) {
		temp = 0.0;

		if (ind->gene[j] == -1) {
			j++;
			continue;
		}

		poi_actual = ind->gene[j];
		temp += pi->param_t[pi->param_o.id][poi_actual] + pi->set_POI[poi_actual - 1].TT;
		poi_anterior = poi_actual;
		j++;

		while (ind->gene[j] != -1 && j < gene_length) {
			poi_actual = ind->gene[j];
			temp += pi->param_t[poi_anterior][poi_actual] + pi->set_POI[poi_actual - 1].TT;
			poi_anterior = poi_actual;
			j++;
		}

		if (j > 0 && ind->gene[j - 1] != -1) {
			temp += pi->param_t[poi_anterior][pi->param_s.id];
		}

		if (temp > pi->param_TM) {
			ind->constr[1] += pi->param_TM - temp; // Castigo negativo
		}

		j++;
	}
	return;
}

/* * RESTRICCIÓN 3 (Eq. 10 del modelo): Ventanas de Tiempo (Time Windows).
 */
void constraint_three(individual *ind, problem_instance *pi) {
	int i, j;
	int start, end;
	double temp;
	ind->constr[2] = 0.0;

	// find_route_bounds usa índice base 1
	for (i = 1; i <= n_routes; i++) {
		find_route_bounds(ind, i, &start, &end);
		if (start == -1 || end == -1) continue;
		temp = 0;

		// Verifica la ventana de tiempo del primer POI
		if (pi->param_t[pi->param_o.name][ind->gene[start]] < pi->set_POI[ind->gene[start]-1].OT) {
			temp = pi->set_POI[ind->gene[start]-1].OT;
		}
		if (pi->param_t[pi->param_o.name][ind->gene[start]] > pi->set_POI[ind->gene[start]-1].CT) {
			temp = pi->param_t[pi->param_o.name][ind->gene[start]];
		}

		// Recorre el resto verificando penalizaciones por llegar temprano o tarde
		for (j = start; j < end; j++) {
			if (temp < pi->set_POI[ind->gene[start]-1].OT) {
				ind->constr[2] += temp - pi->set_POI[ind->gene[start]-1].OT;
			}
			if (temp > pi->set_POI[ind->gene[start]-1].CT) {
				ind->constr[2] += pi->set_POI[ind->gene[start]-1].CT - temp;
			}
			temp = temp + pi->set_POI[ind->gene[start]-1].TT + pi->param_t[ind->gene[start]][ind->gene[start+1]];
		}

		// Verificaciones finales para el último POI y el nodo de destino
		if (temp < pi->set_POI[ind->gene[end]-1].OT) {
			ind->constr[2] += temp - pi->set_POI[ind->gene[end]-1].OT;
		}
		if (temp > pi->set_POI[ind->gene[end]-1].CT) {
			ind->constr[2] += pi->set_POI[ind->gene[end]-1].CT - temp;
		}
		temp = temp + pi->set_POI[ind->gene[end]-1].TT + pi->param_t[ind->gene[end]][pi->param_s.name];

		if (temp < pi->param_s.OT) {
			ind->constr[2] += temp - pi->param_s.OT;
		}
		if (temp > pi->param_s.CT) {
			ind->constr[2] += pi->param_s.CT - temp;
		}
	}

	return;
}