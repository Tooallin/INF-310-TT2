/* Rutinas para el cálculo de la Crowding Distance (Distancia de Apilamiento) en NSGA-II */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Rutina para calcular la crowding distance cuando el frente de dominancia viene en forma de lista enlazada (list).
 * Prepara los arreglos necesarios y maneja los casos base antes de llamar a la función de cálculo principal.
 */
void assign_crowding_distance_list(population *pop, list *lst, int front_size) {
	int **obj_array;
	int *dist;
	int i, j;
	list *temp;
	temp = lst;
    
	// Caso base 1: Si el frente tiene solo 1 individuo, se le asigna distancia infinita (INF)
	// para asegurar que siempre sea seleccionado.
	if (front_size == 1) {
		pop->ind[lst->index].crowd_dist = INF;
		return;
	}
	// Caso base 2: Si el frente tiene 2 individuos, ambos son los extremos del frente.
	// Se les asigna distancia infinita para mantener la máxima amplitud del conjunto de soluciones.
	if (front_size == 2) {
		pop->ind[lst->index].crowd_dist = INF;
		pop->ind[lst->child->index].crowd_dist = INF;
		return;
	}
    
	// Reserva de memoria para matrices auxiliares
	obj_array = (int **)malloc(n_objectives*sizeof(int*));
	dist = (int *)malloc(front_size*sizeof(int));
	for (i = 0; i < n_objectives; i++) {
		obj_array[i] = (int *)malloc(front_size*sizeof(int));
	}
    
	// Extrae los índices de los individuos desde la lista enlazada al arreglo 'dist'
	for (j = 0; j < front_size; j++) {
		dist[j] = temp->index;
		temp = temp->child;
	}
    
	// Llama a la función matemática central que calcula la distancia
	assign_crowding_distance(pop, dist, obj_array, front_size);
    
	// Liberación de memoria
	free(dist);
	for (i = 0; i < n_objectives; i++) {
		free(obj_array[i]);
	}
	free(obj_array);
	return;
}

/* * Rutina para calcular la crowding distance cuando el frente viene en forma de un arreglo (por índices).
 * Funciona de manera idéntica a la anterior, pero extrae los individuos iterando entre c1 y c2.
 */
void assign_crowding_distance_indices(population *pop, int c1, int c2) {
	int **obj_array;
	int *dist;
	int i, j;
	int front_size;
	front_size = c2-c1+1;
    
	// Casos base: frentes muy pequeños obtienen distancia infinita
	if (front_size == 1) {
		pop->ind[c1].crowd_dist = INF;
		return;
	}
	if (front_size == 2) {
		pop->ind[c1].crowd_dist = INF;
		pop->ind[c2].crowd_dist = INF;
		return;
	}
    
	// Reserva de memoria
	obj_array = (int **)malloc(n_objectives*sizeof(int*));
	dist = (int *)malloc(front_size*sizeof(int));
	for (i = 0; i < n_objectives; i++) {
		obj_array[i] = (int *)malloc(front_size*sizeof(int));
	}
    
	// Llena el arreglo 'dist' con los índices consecutivos desde c1 hasta c2
	for (j = 0; j < front_size; j++) {
		dist[j] = c1++;
	}
    
	// Cálculo matemático
	assign_crowding_distance(pop, dist, obj_array, front_size);
    
	// Liberación de memoria
	free(dist);
	for (i = 0; i < n_objectives; i++) {
		free(obj_array[i]);
	}
	free(obj_array);
	return;
}

/* * Rutina central para calcular las crowding distances matemáticas.
 * Implementa la estimación del "cuboide más grande" alrededor de cada solución.
 */
void assign_crowding_distance(population *pop, int *dist, int **obj_array, int front_size) {
	int i, j;
    
	// 1. Ordenamiento independiente por cada función objetivo
	for (i = 0; i < n_objectives; i++) {
		for (j = 0; j < front_size; j++) {
			obj_array[i][j] = dist[j];
		}
		// Ordena el frente actual basándose únicamente en el objetivo 'i'
		quicksort_front_obj(pop, i, obj_array[i], front_size);
	}
    
	// 2. Inicializa las distancias de todos los individuos a 0.0
	for (j = 0; j < front_size; j++) {
		pop->ind[dist[j]].crowd_dist = 0.0;
	}
    
	// 3. A los individuos en los extremos (los mejores absolutos en cada objetivo)
	// se les asigna distancia infinita (INF) para garantizar su preservación
	for (i = 0; i < n_objectives; i++) {
		pop->ind[obj_array[i][0]].crowd_dist = INF;
	}
    
	// 4. Cálculo de la distancia del cuboide para los individuos intermedios
	for (i = 0; i < n_objectives; i++) {
		for (j = 1; j < front_size - 1; j++) {
			// Si el individuo ya tiene distancia infinita, no se sobrescribe
			if (pop->ind[obj_array[i][j]].crowd_dist != INF) {
                
				// Evita la división por cero si el peor y el mejor valor del frente son idénticos
				if (pop->ind[obj_array[i][front_size-1]].obj[i] == pop->ind[obj_array[i][0]].obj[i]) {
					pop->ind[obj_array[i][j]].crowd_dist += 0.0;
				} else {
					// Suma la diferencia normalizada entre el vecino siguiente y el vecino anterior.
					// Fórmula: (Valor[j+1] - Valor[j-1]) / (Max_Valor - Min_Valor)
					pop->ind[obj_array[i][j]].crowd_dist += (pop->ind[obj_array[i][j+1]].obj[i] - pop->ind[obj_array[i][j-1]].obj[i])/(pop->ind[obj_array[i][front_size-1]].obj[i] - pop->ind[obj_array[i][0]].obj[i]);
				}
			}
		}
	}
    
	// 5. Normalización final de la métrica
	// Promedia la distancia total dividiéndola por el número de objetivos
	for (j = 0; j < front_size; j++) {
		if (pop->ind[dist[j]].crowd_dist != INF) {
			pop->ind[dist[j]].crowd_dist = (pop->ind[dist[j]].crowd_dist) / n_objectives;
		}
	}
	return;
}