/* Rutinas de Mutación para alterar genotipos y mantener la diversidad */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>

# include "global.h"
# include "rand.h"

/* * Función envoltura para aplicar mutación a toda la población. */
void mutation_pop(population *pop) {
	int i;
	for (i = 0; i < popsize; i++) {
		mutation_ind(&(pop->ind[i]));
	}
	return;
}

/* * Función para mutar a un individuo específico.
 * Selecciona aleatoriamente 1 de los 4 operadores de mutación disponibles.
 */
void mutation_ind(individual *ind) {
	int choice;
	choice = rnd(1, 4); // Genera un número aleatorio entre 1 y 4
	
	if (choice == 1) {
		ars_mutation(ind);       // Intra-route Swap
	} else if (choice == 2) {
		ers_mutation(ind);       // Inter-route Swap
	} else if (choice == 3) {
		insert_mutation(ind);    // Insert
	} else {
		remove_mutation(ind);    // Remove
	}
	return;
}

/* * 1. OPERADOR: INTRA-ROUTE SWAP MUTATION (ars_mutation)
 * Selecciona dos POIs dentro de una MISMA ruta y los intercambia. 
 * Mejora la eficiencia local de una ruta sin alterar qué nodos la componen.
 */
void ars_mutation(individual *ind) {
	int start, end;
	int poi1, poi2, temp;
	int route;
	
	// 1. Selecciona una ruta al azar
	route = rnd(1, n_routes);

	// 2. Encuentra los límites de la ruta. Si está vacía o tiene solo 1 nodo, aborta.
	find_route_bounds(ind, route, &start, &end);
	if (start == -1 || end == -1 || end - start < 1) return;

	// 3. Selecciona dos posiciones distintas dentro de la ruta
	do {
		poi1 = rnd(start, end);
		poi2 = rnd(start, end);
	} while (poi1 == poi2);

	// 4. Intercambia los POIs (Swap)
	temp = ind->gene[poi1];
	ind->gene[poi1] = ind->gene[poi2];
	ind->gene[poi2] = temp;

	return;
}

/* * 2. OPERADOR: INTER-ROUTE SWAP MUTATION (ers_mutation)
 * Selecciona dos POIs de RUTAS DIFERENTES y los intercambia. 
 * Excelente para diversificar intensamente y escapar de óptimos locales.
 */
void ers_mutation(individual *ind) {
	int start1, end1, start2, end2;
	int poi1, poi2, temp;
	int route1, route2;
	int max_attempts = 10;
	int attempts = 0;

	// 1. Selecciona dos rutas diferentes (con un máximo de 10 intentos)
	do {
		route1 = rnd(1, n_routes);
		route2 = rnd(1, n_routes);
		attempts++;
	} while (route1 == route2 && attempts < max_attempts);

	if (route1 == route2) return;

	// 2. Busca los límites de ambas rutas
	find_route_bounds(ind, route1, &start1, &end1);
	find_route_bounds(ind, route2, &start2, &end2);

	// Si alguna de las rutas está vacía, aborta la mutación
	if (start1 == -1 || end1 == -1 || end1 - start1 < 0) return;
	if (start2 == -1 || end2 == -1 || end2 - start2 < 0) return;

	// 3. Elige un nodo aleatorio de cada ruta
	poi1 = rnd(start1, end1);
	poi2 = rnd(start2, end2);

	// 4. Intercambia los POIs entre las rutas
	temp = ind->gene[poi1];
	ind->gene[poi1] = ind->gene[poi2];
	ind->gene[poi2] = temp;

	return;
}

/* * 3. OPERADOR: INSERT MUTATION (insert_mutation)
 * Selecciona un POI aleatorio de los nodos NO VISITADOS y lo inserta 
 * en una posición aleatoria dentro de una de las M rutas.
 * Fomenta la intensificación local y aumenta la exploración.
 */
void insert_mutation(individual *ind) {
	int start_route, end_route, start_unvisited, end_unvisited;
	int insert_pos, selected_pos, poi_to_insert;
	int i, k;
	int *new_gene;
	int route;

	// 1. Ubica el segmento de los nodos no visitados (al final del genotipo)
	find_last_route_bounds(ind, &start_unvisited, &end_unvisited);
	if (start_unvisited > end_unvisited) return; // Si no hay nodos sin visitar, aborta

	// 2. Selecciona un POI al azar del banco de "no visitados"
	selected_pos = rnd(start_unvisited, end_unvisited);
	poi_to_insert = ind->gene[selected_pos];

	// 3. Elige una ruta de destino aleatoria
	route = rnd(1, n_routes);
	find_route_bounds(ind, route, &start_route, &end_route);
	if (start_route == -1 || end_route == -1) return;

	// 4. Elige en qué posición de la ruta se insertará el nuevo nodo
	insert_pos = rnd(start_route, end_route + 1);

	new_gene = (int *)malloc(gene_length * sizeof(int));
	k = 0;

	// 5. Reconstruye el genotipo insertando el nuevo POI y desplazando el resto
	for (i = 0; i < gene_length; i++) {
		// Inserta el nuevo nodo en la posición elegida
		if (i == insert_pos) {
			new_gene[k++] = poi_to_insert;
		}
		// Copia el resto de nodos, omitiendo el nodo extraído de la lista de 'no visitados'
		if (i != selected_pos) {
			new_gene[k++] = ind->gene[i];
		}
	}

	// Seguridad: Rellena con separadores en caso de desfase
	while (k < gene_length) {
		new_gene[k++] = -1;
	}

	memcpy(ind->gene, new_gene, gene_length * sizeof(int));
	free(new_gene);
}

/* * 4. OPERADOR: REMOVE MUTATION (remove_mutation)
 * Elimina un POI al azar de alguna de las rutas activas y lo arroja al final 
 * del cromosoma, a la zona de nodos "no visitados".
 * Útil para aliviar rutas que superaron el tiempo máximo (T_max).
 */
void remove_mutation(individual *ind) {
	int start, end, start_unvisited, end_unvisited;
	int route = rnd(1, n_routes);
	int poi_to_remove, remove_index;
	int i, k;
	int *new_gene;

	// 1. Encuentra los límites de la ruta seleccionada
	find_route_bounds(ind, route, &start, &end);
	if (start == -1 || end == -1 || end - start < 0) return; // Aborta si la ruta está vacía

	// 2. Elige un nodo de la ruta para eliminar
	remove_index = rnd(start, end);
	poi_to_remove = ind->gene[remove_index];

	// 3. Valida que exista un bloque final de no visitados
	find_last_route_bounds(ind, &start_unvisited, &end_unvisited);
	if (start_unvisited == -1 || end_unvisited == -1) return;

	new_gene = (int *)malloc(gene_length * sizeof(int));
	k = 0;

	// 4. Reconstruye el genotipo saltándose el nodo eliminado
	for (i = 0; i < gene_length; i++) {
		if (i != remove_index) {
			new_gene[k++] = ind->gene[i];
		}
	}

	// 5. Agrega el nodo eliminado al final del genotipo (lista de no visitados)
	new_gene[k++] = poi_to_remove;

	while (k < gene_length) {
		new_gene[k++] = -1;
	}

	memcpy(ind->gene, new_gene, gene_length * sizeof(int));
	free(new_gene);
}