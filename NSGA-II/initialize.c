/* Rutinas de inicialización de datos para la población de NSGA-II */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Función para inicializar toda una población de manera aleatoria.
 * Recorre el arreglo de la población desde 0 hasta el tamaño definido (popsize) 
 * y delega la construcción a la rutina individual.
 */
void initialize_pop(population *pop, problem_instance *pi) {
	int i;
	for (i = 0; i < popsize; i++) {
		// Llama a la inicialización para cada individuo de la población
		initialize_ind(&(pop->ind[i]), pi);
	}
	return;
}

/* * Función para inicializar a un individuo específico.
 * Aquí se aplica directamente la lógica descrita en la Sección 3.5 de la memoria.
 */
void initialize_ind(individual *ind, problem_instance *pi) {
	int i;
	
	// 1. Genera una secuencia inicial aleatoria.
	// Crea un arreglo con los N identificadores de los POIs en un orden completamente al azar[cite: 369, 370].
	// En este punto, el genotipo es solo una lista gigante de nodos sin separar.
	random_sequence(pi->nPOI, ind->gene);
	
	// 2. Segmentación inteligente basada en la duración.
	// Divide la secuencia anterior en M+1 segmentos insertando M separadores con el valor -1[cite: 371].
	// El corte se realiza calculando la duración de la ruta y deteniéndose justo antes 
	// de superar el tiempo máximo permitido (T_max)[cite: 372, 373]. 
	// Esto asegura que la población inicial nazca respetando la Restricción 11 (Eq. 11).
	split_sequence_by_duration(pi, ind->gene);
	
	// 3. Inicialización de los valores de factibilidad.
	// Se asume que el individuo es perfectamente factible (castigo 0.0) hasta que pase 
	// por la función evaluate_ind en eval.c, donde se calcularán las verdaderas violaciones.
	for (i = 0; i < n_constraints; i++) {
		ind->constr[i] = 0.0;
	}
	
	return;
}