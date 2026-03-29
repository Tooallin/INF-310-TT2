/* Rutinas para comprobar la dominancia entre soluciones */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Rutina para la comprobación de dominancia habitual (basada en el método de Deb).
 * Retornará los siguientes valores:
 * 1 si 'a' domina a 'b'
 * -1 si 'b' domina a 'a'
 * 0 si ambos 'a' y 'b' son no dominados entre sí (empate en el frente de Pareto)
 */
int check_dominance(individual *a, individual *b) {
	int i;
	int flag1; // Bandera para indicar si 'a' es estrictamente mejor que 'b' en algún objetivo
	int flag2; // Bandera para indicar si 'b' es estrictamente mejor que 'a' en algún objetivo
	flag1 = 0;
	flag2 = 0;
	
	// CASO 1: AMBOS INDIVIDUOS SON INFACTIBLES
	// (Tienen castigos por violar ventanas de tiempo, cuotas de categorías, etc.) [cite: 354]
	// NOTA: En esta implementación, las violaciones se guardan como números negativos. 
	// Un valor de -5 es peor que un valor de -2.
	if (a->constr_violation < 0 && b->constr_violation < 0) {
		
		// Si 'a' tiene una violación mayor (es decir, más cercana a 0, menos negativa), 'a' domina.
		if (a->constr_violation > b->constr_violation) {
			return (1);
		} else {
			// Si 'a' tiene una violación peor (más negativa), 'b' domina.
			if (a->constr_violation < b->constr_violation) {
				return (-1);
			} else {
				// Si tienen exactamente el mismo nivel de castigo, empatan.
				return (0);
			}
		}
	} else {
		
		// CASO 2: UN INDIVIDUO ES FACTIBLE Y EL OTRO NO
		// La regla de oro: Cualquier solución factible siempre domina a una infactible.
		
		// Si 'a' es infactible (< 0) y 'b' es perfectamente factible (== 0), 'b' domina.
		if (a->constr_violation < 0 && b->constr_violation == 0) {
			return (-1);
		} else {
			
			// Si 'a' es perfectamente factible (== 0) y 'b' es infactible (< 0), 'a' domina.
			if (a->constr_violation == 0 && b->constr_violation < 0) {
				return (1);
			} else {
				
				// CASO 3: AMBOS INDIVIDUOS SON FACTIBLES (constr_violation == 0)
				// Aquí entra a jugar la verdadera evaluación multiobjetivo de Pareto[cite: 225].
				// Se comparan los 3 objetivos de tu modelo del MCTOPTW.
				
				for (i = 0; i < n_objectives; i++) {
					// Asume que NSGA-II minimiza por defecto. 
					// (Si tu objetivo 1 y 2 son maximizar, sus valores deben estar multiplicados por -1).
					
					// Si 'a' tiene un valor menor (mejor) que 'b' en este objetivo:
					if (a->obj[i] < b->obj[i]) {
						flag1 = 1;
					} else {
						// Si 'a' tiene un valor mayor (peor) que 'b' en este objetivo:
						if (a->obj[i] > b->obj[i]) {
							flag2 = 1;
						}
					}
				} 
				
				// RESULTADO DEL FRENTE DE PARETO
				
				// Si 'a' es mejor en al menos un objetivo y no es peor en ninguno, 'a' domina.
				if (flag1 == 1 && flag2 == 0) {
					return (1);
				} else {
					
					// Si 'b' es mejor en al menos un objetivo y no es peor en ninguno, 'b' domina.
					if (flag1 == 0 && flag2 == 1) {
						return (-1);
					} else {
						
						// Si 'a' gana en unos objetivos y 'b' gana en otros, o son idénticos, 
						// ninguno domina al otro. Ambos pertenecen al mismo nivel de dominancia.
						return (0);
					}
				}
			}
		}
	}
}