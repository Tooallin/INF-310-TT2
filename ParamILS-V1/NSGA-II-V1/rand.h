/* Declaration for random number related variables and routines */

# ifndef _RAND_H_
# define _RAND_H_

/* Variable declarations for the random number generator */
extern double seed;
extern double oldrand[55];
extern int jrand;

/* Function declarations for the random number generator */
void randomize(void);
void warmup_random (double seed);
void advance_random (void);
void random_sequence(int N, int *sequence);
void split_sequence(int N, int M, int *sequence);
void split_sequence_by_duration(problem_instance *pi, int *sequence);
double randomperc(void);
int rnd (int low, int high);
double rndreal (double low, double high);

# endif
