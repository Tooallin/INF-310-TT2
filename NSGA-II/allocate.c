/* Rutinas de asignación y liberación de memoria para NSGA-II */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Función para reservar memoria para una población completa.
 * Recibe un puntero a la población y el tamaño (cantidad de individuos).
 */
void allocate_memory_pop(population *pop, int size) {
	int i;

	// 1. Reserva un bloque de memoria contiguo para el arreglo de individuos.
    // El tamaño total es la cantidad de individuos multiplicada por el peso en memoria de la estructura 'individual'.
	pop->ind = (individual *)malloc(size*sizeof(individual));

	// 2. Itera sobre cada individuo recién creado para inicializar sus arreglos internos.
	for (i=0; i<size; i++) {
		allocate_memory_ind(&(pop->ind[i]));
	}
	return;
}

/* * Función para reservar memoria para los atributos de un solo individuo.
 * Construye el cromosoma y los espacios para evaluar la solución.
 */
void allocate_memory_ind(individual *ind) {
	// 1. Reserva memoria para el cromosoma (gene). 
    // Aquí se guarda el arreglo de enteros de largo N+M (POIs + delimitadores de las rutas).
	ind->gene = (int *)malloc(gene_length * sizeof(int));

	// 2. Reserva memoria para las funciones objetivo.
    // En tu caso, almacenará 3 valores: max score, max categorías y min diferencia de puntaje.
	ind->obj = (double *)malloc(n_objectives * sizeof(double));

	// 3. Reserva memoria para los valores de las restricciones (factibilidad).
    // Aquí se guardan los castigos por violar ventanas de tiempo, cuotas de categorías o duración de rutas.
	ind->constr = (double *)malloc(n_constraints * sizeof(double));
	return;
}

/* * Función para liberar la memoria de una población completa.
 * Es crucial ejecutarla al final de cada generación o al terminar el algoritmo
 * para evitar fugas de memoria (memory leaks).
 */
void deallocate_memory_pop(population *pop, int size) {
	int i;

	// 1. Primero, debe destruir la memoria interna de cada individuo de adentro hacia afuera.
	for (i = 0; i < size; i++) {
		deallocate_memory_ind(&(pop->ind[i]));
	}

	// 2. Una vez que los individuos están vacíos, libera el arreglo contenedor de la población.
	free(pop->ind);
	return;
}

/* * Función para liberar la memoria interna de un individuo.
 * Devuelve al sistema operativo los bloques reservados por 'malloc' en la fase de asignación.
 */
void deallocate_memory_ind(individual *ind) {
	// Libera el arreglo del cromosoma
	free(ind->gene);

	// Libera el arreglo de las evaluaciones objetivo
	free(ind->obj);

	// Libera el arreglo de las penalizaciones de restricciones
	free(ind->constr);
	return;
}
