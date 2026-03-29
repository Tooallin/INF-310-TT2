/* Rutinas de Selección por Torneo para NSGA-II */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Rutina de selección principal.
 * Crea una población hija (new_pop) a partir de la población de padres (old_pop).
 * Realiza torneos binarios para elegir a los mejores padres y luego aplica el cruzamiento.
 */
void selection(population *old_pop, population *new_pop, problem_instance *pi) {
	int *a1, *a2;
	int temp;
	int i;
	int rand;
	individual *parent1, *parent2;
    
	// 1. Prepara dos arreglos de índices para barajar la población de forma aleatoria
	a1 = (int *)malloc(popsize*sizeof(int));
	a2 = (int *)malloc(popsize*sizeof(int));
	for (i = 0; i < popsize; i++) {
		a1[i] = a2[i] = i;
	}
    
	// 2. Baraja aleatoriamente los índices en a1 y a2 (Shuffle)
	// Esto asegura que los torneos no siempre enfrenten a los mismos individuos.
	for (i = 0; i < popsize; i++) {
		rand = rnd (i, popsize-1);
		temp = a1[rand];
		a1[rand] = a1[i];
		a1[i] = temp;
		rand = rnd (i, popsize-1);
		temp = a2[rand];
		a2[rand] = a2[i];
		a2[i] = temp;
	}
    
	// 3. Ciclo de formación de la nueva generación
	// En cada iteración se procesan 4 individuos para generar 4 hijos mediante 4 torneos y 2 cruces.
	for (i = 0; i < popsize; i+=4) {
		// Torneos usando el primer arreglo barajado (a1)
		parent1 = tournament (&old_pop->ind[a1[i]], &old_pop->ind[a1[i+1]]);
		parent2 = tournament (&old_pop->ind[a1[i+2]], &old_pop->ind[a1[i+3]]);
		
		// Genera los hijos 1 y 2 usando Route-Based Crossover (crossover.c) [cite: 390-391]
		crossover (parent1, parent2, &new_pop->ind[i], &new_pop->ind[i+1], pi);
		
		// Torneos usando el segundo arreglo barajado (a2)
		parent1 = tournament (&old_pop->ind[a2[i]], &old_pop->ind[a2[i+1]]);
		parent2 = tournament (&old_pop->ind[a2[i+2]], &old_pop->ind[a2[i+3]]);
		
		// Genera los hijos 3 y 4
		crossover (parent1, parent2, &new_pop->ind[i+2], &new_pop->ind[i+3], pi);
	}
    
	free (a1);
	free (a2);
	return;
}

/* * Rutina para el Torneo Binario.
 * Compara dos individuos y devuelve el ganador basándose en el Elitismo de NSGA-II.
 */
individual* tournament (individual *ind1, individual *ind2)
{
    int flag;
    
    // CRITERIO 1: Dominancia (Calidad de la solución y Factibilidad)
    // Se utiliza la función de dominance.c que ya considera castigos por restricciones [cite: 235-237].
    flag = check_dominance (ind1, ind2);
    
    if (flag==1)      // ind1 domina a ind2
    {
        return (ind1);
    }
    if (flag==-1)     // ind2 domina a ind1
    {
        return (ind2);
    }
    
    // CRITERIO 2: Crowding Distance (Diversidad)
    // Si ninguno domina al otro (pertenecen al mismo frente), se prefiere al que está 
    // en una zona menos poblada para mantener la diversidad[cite: 247].
    if (ind1->crowd_dist > ind2->crowd_dist)
    {
        return(ind1);
    }
    if (ind2->crowd_dist > ind1->crowd_dist)
    {
        return(ind2);
    }
    
    // CRITERIO 3: Azar
    // Si son idénticos en frente y diversidad, se elige uno al azar.
    if ((randomperc()) <= 0.5)
    {
        return(ind1);
    }
    else
    {
        return(ind2);
    }
}