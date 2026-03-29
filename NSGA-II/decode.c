/* Rutinas para decodificar la población (Mapeo Genotipo -> Fenotipo) */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Función para decodificar todos los individuos de una población.
 * Itera a través del arreglo de la población y llama a la rutina de decodificación individual.
 */
void decode_pop(population *pop) {
	int i;
	for (i = 0; i < popsize; i++) {
		decode_ind(&(pop->ind[i]));
	}
	return;
}

/* * Función para decodificar el patrón de bits de un individuo.
 * * NOTA IMPORTANTE PARA ESTE MODELO: 
 * En implementaciones clásicas, aquí se traducen variables binarias a valores reales.
 * Sin embargo, en esta implementación del MCTOPTW, la representación elegida 
 * consta de un arreglo de enteros con identificadores de POIs y delimitadores -1[cite: 300, 302].
 * Como la estructura del cromosoma ya representa directamente las M rutas válidas, 
 * la decodificación no es necesaria (Genotipo = Fenotipo). 
 * La función se mantiene vacía para conservar la arquitectura de NSGA-II.
 */
void decode_ind(individual *ind) {
	return;
}