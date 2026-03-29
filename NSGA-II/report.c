/* Rutinas para almacenar los datos de la población en archivos de texto (Logs y Resultados) */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Función para imprimir la información completa de una población en un archivo.
 * Imprime las columnas en este orden estricto:
 * 1. Valores de las Funciones Objetivo (Obj1, Obj2, Obj3)
 * 2. Valores de las Restricciones (Castigos individuales)
 * 3. El Cromosoma completo (identificadores de POIs y -1)
 * 4. Violación total acumulada (Factibilidad global)
 * 5. Nivel de Rank (1 es el frente de Pareto óptimo)
 * 6. Crowding Distance (Diversidad)
 */
void report_pop(population *pop, FILE *fpt) {
	int i, j;
	for (i = 0; i < popsize; i++) {
		
		// Imprime objetivos
		for (j = 0; j < n_objectives; j++) {
			fprintf(fpt,"%e\t", pop->ind[i].obj[j]);
		}
		
		// Imprime castigos individuales de restricciones
		for (j = 0; j < n_constraints; j++) {
			fprintf(fpt,"%e\t", pop->ind[i].constr[j]);
		}
		
		// Imprime el genotipo (rutas)
		for (j = 0; j < gene_length; j++) {
			fprintf(fpt,"%d\t", pop->ind[i].gene[j]);
		}
		
		// Imprime métricas resumen
		fprintf(fpt,"%e\t", pop->ind[i].constr_violation);
		fprintf(fpt,"%d\t", pop->ind[i].rank);
		fprintf(fpt,"%e\n", pop->ind[i].crowd_dist);
	}
	return;
}

/* * Función para imprimir EXCLUSIVAMENTE la información de la población factible 
 * y no dominada. Solo rescata a los individuos perfectos (Frente 1 y Castigo 0).
 */
void report_feasible(population *pop, FILE *fpt) {
	int i, j;
	for (i = 0; i < popsize; i++) {
		
		// FILTRO: Solo individuos sin penalizaciones (0.0) y que pertenezcan al mejor frente de Pareto (rank 1)
		if (pop->ind[i].constr_violation == 0.0 && pop->ind[i].rank == 1) {
			
			for (j = 0; j < n_objectives; j++) {
				fprintf(fpt,"%e\t", pop->ind[i].obj[j]);
			}
			for (j = 0; j < n_constraints; j++) {
				fprintf(fpt,"%e\t", pop->ind[i].constr[j]);
			}
			for (j = 0; j < gene_length; j++) {
				fprintf(fpt,"%d\t", pop->ind[i].gene[j]);
			}
			fprintf(fpt,"%e\t", pop->ind[i].constr_violation);
			fprintf(fpt,"%d\t", pop->ind[i].rank);
			fprintf(fpt,"%e\n", pop->ind[i].crowd_dist);
		}
	}
	return;
}