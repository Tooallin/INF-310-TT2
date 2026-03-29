/* Rutinas de selección basadas en ordenamiento de no-dominancia (Non-dominated Sorting) */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Rutina principal para realizar el ordenamiento por frentes de Pareto.
 * Toma una población combinada (mixed_pop) de tamaño 2N (padres + hijos) 
 * y la filtra para construir la nueva población (new_pop) de tamaño N.
 */
void fill_nondominated_sort(population *mixed_pop, population *new_pop) {
	int flag;
	int i, j;
	int end;
	int front_size;
	int archieve_size;
	int rank = 1; // Nivel de dominancia (Frente 1 es el mejor)
	
	// Listas enlazadas temporales para clasificar a los individuos
	list *pool;  // Piscina con todos los individuos que aún no han sido asignados a un frente
	list *elite; // Individuos que están conformando el frente actual que se está evaluando
	list *temp1, *temp2;
	
	pool = (list *)malloc(sizeof(list));
	elite = (list *)malloc(sizeof(list));
	front_size = 0;
	archieve_size=0;
	
	pool->index = -1;
	pool->parent = NULL;
	pool->child = NULL;
	elite->index = -1;
	elite->parent = NULL;
	elite->child = NULL;
	temp1 = pool;
	
	// 1. Inicializa la piscina 'pool' con los 2N individuos de la población combinada
	for (i = 0; i < 2 * popsize; i++) {
		insert(temp1, i);
		temp1 = temp1->child;
	}
	
	i = 0;
	// 2. Ciclo principal para extraer los frentes de dominancia uno por uno [cite: 239, 240, 241]
	do {
		temp1 = pool->child;
		insert(elite, temp1->index); // Toma el primer individuo de la piscina y lo asume "élite" (Frente actual)
		front_size = 1;
		temp2 = elite->child;
		temp1 = del(temp1); // Lo saca de la piscina
		temp1 = temp1->child;
		
		// 3. Compara al resto de la piscina con los individuos del frente 'elite' actual [cite: 236]
		do {
			temp2 = elite->child;
			if (temp1 == NULL) {
				break;
			}
			do {
				end = 0;
				// Llama a la función de dominance.c (1: domina, -1: dominado, 0: empate)
				flag = check_dominance(&(mixed_pop->ind[temp1->index]), &(mixed_pop->ind[temp2->index]));
				
				// Si el individuo de la piscina domina al del frente 'elite', 
				// el del frente 'elite' es degradado y devuelto a la piscina
				if (flag == 1) {
					insert(pool, temp2->index);
					temp2 = del(temp2);
					front_size--;
					temp2 = temp2->child;
				}
				// Si hay empate (no dominancia mutua), se mantiene evaluando
				if (flag == 0) {
					temp2 = temp2->child;
				}
				// Si el de la piscina es dominado por CUALQUIERA del frente 'elite', pierde.
				if (flag == -1) {
					end = 1;
				}
			} while (end != 1 && temp2 != NULL);
			
			// Si el individuo de la piscina dominó a alguien o empató con todos en 'elite',
			// se une a este frente de dominancia actual.
			if (flag == 0 || flag == 1) {
				insert(elite, temp1->index);
				front_size++;
				temp1 = del(temp1);
			}
			temp1 = temp1->child;
		} while (temp1 != NULL);
		
		temp2 = elite->child;
		j = i;
		
		// 4. Verificación de capacidad para la nueva población (Líneas 9-13 de tu Algoritmo 1) [cite: 275-284]
		// Si agregar el frente ENTERO actual no excede el tamaño N de la población (popsize)...
		if ((archieve_size + front_size) <= popsize) {
			do {
				copy_ind(&mixed_pop->ind[temp2->index], &new_pop->ind[i]); // Copia al individuo
				new_pop->ind[i].rank = rank; // Le asigna su nivel de frente (1, 2, 3...)
				archieve_size += 1;
				temp2 = temp2->child;
				i += 1;
			} while (temp2 != NULL);
			
			// Calcula las crowding distances dentro de este frente copiado [cite: 277]
			assign_crowding_distance_indices(new_pop, j, i-1);
			rank += 1; // Prepara el nivel para el siguiente ciclo
			
		} else {
			// 5. El frente actual NO cabe completo (Líneas 14-17 de tu Algoritmo 1) [cite: 285-292]
			// Llama a crowding_fill para ordenar este último frente por Crowding Distance 
			// y extraer solo los mejores hasta llenar exactamente la población de tamaño N[cite: 247, 290, 291].
			crowding_fill(mixed_pop, new_pop, i, front_size, elite);
			archieve_size = popsize; // Llena la cuota artificialmente para forzar la salida del while
			
			// Asigna el ranking a los individuos de este último frente particionado
			for (j = i; j < popsize; j++) {
				new_pop->ind[j].rank = rank;
			}
		}
		
		// Limpia la lista 'elite' para empezar a construir el siguiente frente en la próxima iteración
		temp2 = elite->child;
		do {
			temp2 = del(temp2);
			temp2 = temp2->child;
		} while (elite->child != NULL);
		
	} while (archieve_size < popsize); // Repite hasta que hayamos seleccionado exactamente a N individuos
	
	// Limpieza de memoria general
	while (pool != NULL) {
		temp1 = pool;
		pool = pool->child;
		free(temp1);
	}
	while (elite != NULL) {
		temp1 = elite;
		elite = elite->child;
		free(temp1);
	}
	return;
}

/* * Rutina para rellenar la población con individuos ordenados de mayor a menor Crowding Distance.
 * Esta función se ejecuta ÚNICAMENTE cuando un frente de dominancia no cabe completo en la 
 * nueva población, decidiendo quién entra y quién se queda fuera basándose en la diversidad[cite: 247].
 */
void crowding_fill(population *mixed_pop, population *new_pop, int count, int front_size, list *elite) {
	int *dist;
	list *temp;
	int i, j;
	
	// 1. Calcula las distancias de apilamiento matemáticas (el cuboide) para este frente específico [cite: 242, 285]
	assign_crowding_distance_list(mixed_pop, elite->child, front_size);
	
	// 2. Extrae los índices de los individuos a un arreglo normal
	dist = (int *)malloc(front_size*sizeof(int));
	temp = elite->child;
	for (j = 0; j < front_size; j++) {
		dist[j] = temp->index;
		temp = temp->child;
	}
	
	// 3. Ordena el arreglo de mayor a menor Crowding Distance [cite: 290]
	// (Quienes tienen mayor distancia están en zonas menos pobladas, aportando más diversidad) [cite: 243, 244, 247]
	quicksort_dist(mixed_pop, dist, front_size);
	
	// 4. Copia los mejores individuos a la nueva población hasta que se llene (tamaño N) [cite: 291]
	for (i = count, j = front_size - 1; i < popsize; i++, j--) {
		copy_ind(&mixed_pop->ind[dist[j]], &new_pop->ind[i]);
	}
	
	free(dist);
	return;
}