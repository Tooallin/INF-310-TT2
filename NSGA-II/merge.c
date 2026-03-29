/* Rutinas para combinar (fusionar) dos poblaciones y copiar individuos */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Rutina para fusionar dos poblaciones (pop1 y pop2) en una sola población combinada (pop3).
 * En el contexto de NSGA-II (Algoritmo 1), pop1 son los padres (P_g), pop2 son los hijos (Q_g) 
 * y pop3 es la población mixta (R_g) de tamaño 2N.
 */
void merge(population *pop1, population *pop2, population *pop3) {
	int i, k;
	
	// 1. Copia todos los individuos de la población 1 (Padres) en la primera mitad de la población 3
	for (i = 0; i < popsize; i++) {
		copy_ind(&(pop1->ind[i]), &(pop3->ind[i]));
	}
	
	// 2. Copia todos los individuos de la población 2 (Hijos) en la segunda mitad de la población 3
	// Se usa 'k' para empezar a escribir desde el índice 'popsize' hasta '2 * popsize - 1'
	for (i = 0, k = popsize; i < popsize; i++, k++) {
		copy_ind(&(pop2->ind[i]), &(pop3->ind[k]));
	}
	return;
}

/* * Rutina para realizar una copia profunda (Deep Copy) de un individuo 'ind1' a otro 'ind2'.
 * En C, no se puede hacer simplemente "ind2 = ind1" porque ambos contienen punteros 
 * a arreglos dinámicos (gene, obj, constr). Si se hace una asignación simple, ambos 
 * apuntarían a la misma dirección de memoria, causando errores graves.
 */
void copy_ind(individual *ind1, individual *ind2) {
	int i;
	
	// Copia de los atributos escalares básicos (Nivel de Pareto, Factibilidad y Diversidad)
	ind2->rank = ind1->rank;
	ind2->constr_violation = ind1->constr_violation;
	ind2->crowd_dist = ind1->crowd_dist;
	
	// Copia profunda del cromosoma (arreglo genotípico que contiene las M rutas y delimitadores)
	for (i = 0; i < gene_length; i++) {
		ind2->gene[i] = ind1->gene[i];
	}
	
	// Copia profunda de las evaluaciones de las 3 funciones objetivo
	for (i = 0; i < n_objectives; i++) {
		ind2->obj[i] = ind1->obj[i];
	}
	
	// Copia profunda de las penalizaciones de las 3 restricciones calculadas
	for (i = 0; i < n_constraints; i++) {
		ind2->constr[i] = ind1->constr[i];
	}
	return;
}