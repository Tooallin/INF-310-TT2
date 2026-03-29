/* Definición de las rutinas de generación de números aleatorios y secuencias iniciales */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"
# include <string.h>

double seed;
double oldrand[55];
int jrand;

/* * Inicializa el generador de números aleatorios usando la semilla global */
void randomize()
{
    int j1;
    for(j1=0; j1<=54; j1++)
    {
        oldrand[j1] = 0.0;
    }
    jrand=0;
    warmup_random (seed);
    return;
}

/* * "Calienta" el generador. 
 * Aplica un algoritmo matemático para llenar el arreglo de estado inicial 
 * evitando que los primeros números generados tengan patrones predecibles.
 */
void warmup_random (double seed)
{
    int j1, ii;
    double new_random, prev_random;
    oldrand[54] = seed;
    new_random = 0.000000001;
    prev_random = seed;
    for(j1=1; j1<=54; j1++)
    {
        ii = (21*j1)%54;
        oldrand[ii] = new_random;
        new_random = prev_random-new_random;
        if(new_random<0.0)
        {
            new_random += 1.0;
        }
        prev_random = oldrand[ii];
    }
    advance_random ();
    advance_random ();
    advance_random ();
    jrand = 0;
    return;
}

/* * Genera el siguiente lote de 55 números pseudo-aleatorios */
void advance_random ()
{
    int j1;
    double new_random;
    for(j1=0; j1<24; j1++)
    {
        new_random = oldrand[j1]-oldrand[j1+31];
        if(new_random<0.0)
        {
            new_random = new_random+1.0;
        }
        oldrand[j1] = new_random;
    }
    for(j1=24; j1<55; j1++)
    {
        new_random = oldrand[j1]-oldrand[j1-24];
        if(new_random<0.0)
        {
            new_random = new_random+1.0;
        }
        oldrand[j1] = new_random;
    }
}

/* * Segmenta una secuencia en M partes iguales (No parece usarse en la versión final 
 * porque no respeta T_max, pero sirve como función auxiliar base).
 */
void split_sequence(int N, int M, int *sequence) {
	int total_parts = M + 1;
	int base = N / total_parts;
	int extra = N % total_parts;
	int write_idx = N + M - 1;
	int read_idx = N - 1;
	int part;
	int i;

	for (part = M; part >= 0; part--) {
		int size = base + (part < extra ? 1 : 0);

		for (i = 0; i < size; i++) {
			sequence[write_idx--] = sequence[read_idx--];
		}

		if (part > 0) {
			sequence[write_idx--] = -1;
		}
	}
}

/* * RUTINA CLAVE DE INICIALIZACIÓN (Sección 3.5 del documento)
 * Toma una secuencia aleatoria de POIs y la divide en M rutas asegurándose de que 
 * CADA ruta respete el tiempo máximo permitido (Restricción 11: T_max).
 */
void split_sequence_by_duration(problem_instance *pi, int *sequence) {
	int *temp = (int *)malloc(pi->nPOI * sizeof(int));
	int *used = (int *)calloc(pi->nPOI, sizeof(int));
	int write_idx = 0;
	int route_count = 0;
	int origin = pi->param_o.id;
	int destination = pi->param_s.id;
	int i;
	int poi;
	double travel_time;
	double service_time;
	double return_time;
	double projected_time;

	// Guarda una copia de la secuencia original aleatoria
	memcpy(temp, sequence, pi->nPOI * sizeof(int));

	// Construye exactamente M rutas
	while (route_count < pi->set_M) {
		double total_time = 0.0;
		int last_node = origin;

		// Intenta agregar POIs de la secuencia a la ruta actual
		for (i = 0; i < pi->nPOI; i++) {
			if (used[i]) continue; // Si ya se usó, lo salta

			poi = temp[i];
			// Calcula el tiempo de viaje desde el nodo anterior, el servicio, y el retorno al fin
			travel_time = pi->param_t[last_node][poi];
			service_time = pi->set_POI[poi - 1].TT;
			return_time = pi->param_t[poi][destination];
			
			// Proyecta si agregar este nodo violaría T_max
			projected_time = total_time + travel_time + service_time + return_time;

			// Si es factible en tiempo, lo inserta en la ruta [cite: 372-373]
			if (projected_time <= pi->param_TM) {
				sequence[write_idx++] = poi;
				used[i] = 1;
				total_time += travel_time + service_time;
				last_node = poi;
			}
		}

		// Cierra la ruta con el delimitador
		sequence[write_idx++] = -1;
		route_count++;
	}

	// Envía todos los POIs que no cupieron por tiempo al segmento final de "no visitados" [cite: 305]
	for (i = 0; i < pi->nPOI; i++) {
		if (!used[i]) {
			sequence[write_idx++] = temp[i];
		}
	}

	free(temp);
	free(used);
}

/* * Crea una secuencia aleatoria (permutación) de tamaño N. 
 * Representa el primer paso de la inicialización [cite: 369-370].
 */
void random_sequence(int N, int *sequence) {
	int i, j, temp;

	// Llena el arreglo con números del 1 al N
	for (i = 0; i < N; i++) {
		sequence[i] = i + 1;
	}

	// Algoritmo Fisher-Yates shuffle para desordenar el arreglo aleatoriamente
	for (i = N - 1; i > 0; i--) {
		j = rnd(0, i);
		temp = sequence[i];
		sequence[i] = sequence[j];
		sequence[j] = temp;
	}
}

/* * Obtiene un número aleatorio real entre 0.0 y 1.0 */
double randomperc() {
	jrand++;
	if (jrand >= 55) {
			jrand = 1;
			advance_random();
	}
	return((double)oldrand[jrand]);
}

/* * Obtiene un número entero aleatorio entre 'low' y 'high' (inclusive) */
int rnd (int low, int high)
{
    int res;
    if (low >= high)
    {
        res = low;
    }
    else
    {
        res = low + (randomperc()*(high-low+1));
        if (res > high)
        {
            res = high;
        }
    }
    return (res);
}

/* * Obtiene un número real aleatorio entre 'low' y 'high' (inclusive) */
double rndreal (double low, double high)
{
    return (low + (high-low)*randomperc());
}