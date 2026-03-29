/* Rutina de asignación de rangos (Frentes de Pareto) */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Función para clasificar a toda una población de tamaño N en frentes de dominancia
 * y calcular sus distancias de diversidad (Crowding distance).
 */
void assign_rank_and_crowding_distance(population *new_pop) {
	int flag;
	int i;
	int end;
	int front_size;
	int rank = 1; // Nivel 1 = Frente de Pareto Óptimo
	list *orig;   // Lista de individuos por evaluar (Piscina)
	list *cur;    // Lista de individuos del frente actual
	list *temp1, *temp2;
	
	orig = (list *)malloc(sizeof(list));
	cur = (list *)malloc(sizeof(list));
	front_size = 0;
	orig->index = -1;
	orig->parent = NULL;
	orig->child = NULL;
	cur->index = -1;
	cur->parent = NULL;
	cur->child = NULL;
	temp1 = orig;
	
	// 1. Mete a todos los individuos de la población a la lista de origen (orig)
	for (i = 0; i < popsize; i++) {
		insert(temp1, i);
		temp1 = temp1->child;
	}
	
	// 2. Ciclo principal para extraer los frentes uno por uno
	do {
		// Caso base: Si solo queda un individuo en la piscina, es el último frente
		if (orig->child->child == NULL) {
			new_pop->ind[orig->child->index].rank = rank;
			new_pop->ind[orig->child->index].crowd_dist = INF;
			break;
		}
		
		temp1 = orig->child;
		insert(cur, temp1->index); // Toma el primer individuo y lo pone en el frente actual (cur)
		front_size = 1;
		temp2 = cur->child;
		temp1 = del(temp1); // Lo saca de la piscina
		temp1 = temp1->child;
		
		// 3. Compara al resto de la piscina con el frente actual
		do {
			temp2 = cur->child;
			do {
				end = 0;
				// Verifica dominancia usando las reglas estrictas de check_dominance (factibilidad + objetivos)
				flag = check_dominance(&(new_pop->ind[temp1->index]), &(new_pop->ind[temp2->index]));
				
				// Si el individuo evaluado domina a uno que ya estaba en el frente, 
				// el antiguo es degradado y devuelto a la piscina 'orig'
				if (flag == 1) {
					insert(orig, temp2->index);
					temp2 = del(temp2);
					front_size--;
					temp2 = temp2->child;
				}
				if (flag == 0) { // Empate, ambos siguen en carrera
					temp2 = temp2->child;
				}
				if (flag == -1) { // El individuo evaluado fue dominado, no entra a este frente
					end = 1;
				}
			} while (end != 1 && temp2 != NULL);
			
			// Si no fue dominado por nadie, se incorpora al frente actual 'cur'
			if (flag == 0 || flag == 1) {
				insert(cur, temp1->index);
				front_size++;
				temp1 = del(temp1);
			}
			temp1 = temp1->child;
		} while (temp1 != NULL);
		
		// 4. Se cierra el frente actual. Se le asigna el rango (nivel) a todos sus miembros
		temp2 = cur->child;
		do {
			new_pop->ind[temp2->index].rank = rank;
			temp2 = temp2->child;
		} while (temp2 != NULL);
		
		// 5. Calcula la "Crowding Distance" (diversidad espacial) exclusivamente para los miembros de este frente
		assign_crowding_distance_list(new_pop, cur->child, front_size);
		
		// 6. Limpia la lista del frente actual para empezar a calcular el siguiente nivel
		temp2 = cur->child;
		do {
			temp2 = del(temp2);
			temp2 = temp2->child;
		} while (cur->child != NULL);
		
		rank += 1; // Incrementa el nivel (Frente 2, Frente 3, etc.)
		
	} while (orig->child != NULL); // Repite hasta que la piscina esté vacía
	
	free(orig);
	free(cur);
	return;
}