/* Declaración de variables y rutinas relacionadas con la generación de números aleatorios */

# ifndef _RAND_H_
# define _RAND_H_

/* Declaración de variables globales para el generador de números aleatorios */
extern double seed;         // Semilla inicial
extern double oldrand[55];  // Arreglo de estado para el generador pseudo-aleatorio
extern int jrand;           // Índice actual

/* Declaración de funciones */
void randomize(void);
void warmup_random (double seed);
void advance_random (void);
void random_sequence(int N, int *sequence);
void split_sequence(int N, int M, int *sequence);
void split_sequence_by_duration(problem_instance *pi, int *sequence); // Lógica de inicialización
double randomperc(void);
int rnd (int low, int high);
double rndreal (double low, double high);

# endif