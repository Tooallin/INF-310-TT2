/* Rutinas para el algoritmo de ordenamiento recursivo y aleatorizado Quick-Sort */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Función envoltura (wrapper) para iniciar el Quicksort basado en un objetivo particular.
 * Recibe el arreglo de índices 'obj_array' y llama a la función recursiva.
 * Es utilizada intensivamente por crowddist.c antes de calcular el cuboide.
 */
void quicksort_front_obj(population *pop, int objcount, int obj_array[], int obj_array_size) {
	// Llama a la rutina real indicando el límite izquierdo (0) y derecho (tamaño - 1)
	q_sort_front_obj(pop, objcount, obj_array, 0, obj_array_size - 1);
	return;
}

/* * Implementación real de Quicksort para ordenar la población según una de las 3 funciones objetivo.
 * Ordena de menor a mayor (ascendente).
 */
void q_sort_front_obj(population *pop, int objcount, int obj_array[], int left, int right) {
	int index;
	int temp;
	int i, j;
	double pivot;
	
	if (left < right) {
		// 1. Elige un pivote aleatorio para evitar el peor caso de rendimiento (O(n^2))
		index = rnd(left, right);
		
		// 2. Mueve el pivote al final del arreglo temporalmente
		temp = obj_array[right];
		obj_array[right] = obj_array[index];
		obj_array[index] = temp;
		
		// El valor a comparar es la evaluación del objetivo (objcount) del individuo pivote
		pivot = pop->ind[obj_array[right]].obj[objcount];
		i = left - 1;
		
		// 3. Proceso de partición: Mueve a la izquierda todos los elementos menores o iguales al pivote
		for (j = left; j < right; j++) {
			if (pop->ind[obj_array[j]].obj[objcount] <= pivot) {
				i += 1;
				// Intercambia posiciones en el arreglo de índices
				temp = obj_array[j];
				obj_array[j] = obj_array[i];
				obj_array[i] = temp;
			}
		}
		
		// 4. Coloca el pivote en su posición final definitiva
		index = i + 1;
		temp = obj_array[index];
		obj_array[index] = obj_array[right];
		obj_array[right] = temp;
		
		// 5. Llamadas recursivas para ordenar la sub-lista izquierda y la sub-lista derecha
		q_sort_front_obj(pop, objcount, obj_array, left, index - 1);
		q_sort_front_obj(pop, objcount, obj_array, index + 1, right);
	}
	return;
}

/* * Función envoltura (wrapper) para iniciar el Quicksort basado en la Crowding Distance.
 * Utilizada por fillnds.c para decidir qué individuos de un frente empatado pasan a la siguiente generación.
 */
void quicksort_dist(population *pop, int *dist, int front_size) {
	q_sort_dist(pop, dist, 0, front_size - 1);
	return;
}

/* * Implementación real de Quicksort para ordenar la población según su distancia de apilamiento (diversidad).
 * NOTA: Ordena en forma ASCENDENTE (de menor a mayor distancia). 
 * Es por esto que en `fillnds.c` el ciclo recorre el arreglo de atrás hacia adelante (j--), 
 * para extraer primero a los que tienen MAYOR Crowding Distance.
 */
void q_sort_dist(population *pop, int *dist, int left, int right) {
	int index;
	int temp;
	int i, j;
	double pivot;
	
	if (left < right) {
		// 1. Elección de pivote aleatorio
		index = rnd(left, right);
		temp = dist[right];
		dist[right] = dist[index];
		dist[index] = temp;
		
		// El valor a comparar esta vez es la crowding_dist calculada para el individuo
		pivot = pop->ind[dist[right]].crowd_dist;
		i = left - 1;
		
		// 2. Proceso de partición según la distancia
		for (j = left; j < right; j++) {
			if (pop->ind[dist[j]].crowd_dist <= pivot) {
				i += 1;
				temp = dist[j];
				dist[j] = dist[i];
				dist[i] = temp;
			}
		}
		
		// 3. Ubicación del pivote en su lugar final
		index = i + 1;
		temp = dist[index];
		dist[index] = dist[right];
		dist[right] = temp;
		
		// 4. Recursión para ordenar mitades
		q_sort_dist(pop, dist, left, index - 1);
		q_sort_dist(pop, dist, index + 1, right);
	}
	return;
}