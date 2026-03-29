/* Rutinas de cruzamiento (Crossover) para NSGA-II */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Función envoltura (wrapper) para cruzar dos individuos padres.
 * Para generar el segundo hijo se realiza el mismo procedimiento solo que invirtiendo el orden de los padres[cite: 395].
 */
void crossover(individual *parent1, individual *parent2, individual *child1, individual *child2, problem_instance *pi) {
	// Genera el primer hijo dando prioridad a las rutas del padre 1
	rbx_crossover(parent1, parent2, child1);
	
	// Genera el segundo hijo dando prioridad a las rutas del padre 2
	rbx_crossover(parent2, parent1, child2);
	return;
}

/* * Implementación del Route-Based Crossover (Cruce Basado en Rutas).
 * Este operador elige aleatoriamente una de las rutas completas del primer padre y la copia directamente en el hijo (el orden y la estructura se respeta)[cite: 391].
 * Luego, se agregan las rutas restantes del segundo padre al hijo, ignorando la ruta elegida del primer padre[cite: 393].
 */
void rbx_crossover(individual *parent1, individual *parent2, individual *child) {
	
	// Arreglo auxiliar para registrar qué POIs ya fueron insertados en el hijo.
	// Esto es vital para asegurar que no se visiten nodos duplicados entre las distintas rutas de la solución (Restricción 8).
	int *used = (int *)calloc(gene_length, sizeof(int));
	
	int *gene = child->gene;
	int selected_route;
	int start, end;
	int max_attempts = 10;
	int attempts = 0;
	int i, j, k;

	// 1. Selección de la ruta del Padre 1.
	// Se intenta seleccionar una ruta aleatoria que no esté vacía.
	do {
		selected_route = rnd(1, n_routes); // Elige una ruta entre 1 y M
		find_route_bounds(parent1, selected_route, &start, &end); // Busca sus límites usando la función auxiliar
		attempts++;
	} while (start == -1 && end == -1 && attempts < max_attempts); // Evita quedarse atrapado si el padre tiene rutas vacías

	// Mecanismo de seguridad: si después de 10 intentos no halla una ruta válida, aborta el cruce.
	if (attempts == max_attempts) {
	    free(used);
	    return;
	}

	i = 0; // Índice para ir construyendo el cromosoma del hijo
	
	// 2. Copia exacta de la ruta seleccionada del Padre 1 al hijo.
	for (j = start; j <= end; j++) {
		gene[i++] = parent1->gene[j]; // Copia el identificador del POI
		used[parent1->gene[j]-1] = 1; // Lo marca como utilizado en nuestro registro auxiliar
	}
	gene[i++] = -1; // Cierra la ruta insertando el delimitador

	// 3. Relleno con las rutas del Padre 2.
	for (j = 1; j <= n_routes; j++) {
		// Ignoramos la ruta que ya sacamos del Padre 1 para mantener el límite de M rutas[cite: 393].
		if (j == selected_route) continue;
		
		find_route_bounds(parent2, j, &start, &end);
		
		for (k = start; k <= end; k++) {
			if (start == -1) break; // Si la ruta en el Padre 2 está vacía, no hace nada
			
			// Todos los POIs ya utilizados son eliminados en el segundo padre[cite: 392].
			// Si el POI ya está en el registro 'used', lo saltamos.
			if (used[parent2->gene[k]-1]) continue; 
			
			gene[i++] = parent2->gene[k]; // Inserta el POI en el hijo
			used[parent2->gene[k]-1] = 1; // Lo marca como utilizado
		}
		gene[i++] = -1; // Cierra la ruta insertando el delimitador
	}

	// 4. Construcción del segmento final (Nodos no visitados).
	// Los POIs eliminados o no visitados se añaden al segmento final[cite: 394].
	for (j = 0; j < gene_length; j++) {
		if (i == gene_length) break; // Seguridad para no exceder el tamaño del cromosoma
		
		// Revisa el registro auxiliar. Si un POI (j+1) no fue usado en ninguna de las M rutas, se va al fondo.
		if (!used[j]) {
			gene[i++] = j+1; 
		}
	}

	// Liberación de la memoria del registro auxiliar para evitar memory leaks.
	free(used);
	return;
}