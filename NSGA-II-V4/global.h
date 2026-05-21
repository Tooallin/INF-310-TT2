/* This file contains the variable and function declarations */
# ifndef _GLOBAL_H_
# define _GLOBAL_H_
# define INF 1.0e14
# define EPS 1.0e-14
# define E  2.71828182845905
# define PI 3.14159265358979
# define GNUPLOT_COMMAND "gnuplot -persist"

typedef struct {
	int rank;
	double constr_violation;
	int *gene;
	double *obj;
	double *constr;
	double crowd_dist;
} individual;

typedef struct {
	individual *ind;
} population;

typedef struct lists {
	int index;
	struct lists *parent;
	struct lists *child;
} list;

typedef struct {
	int id;
	int name;
	int *e;
	int SCORE;
	int OT;
	int TT;
	int CT;
} POI;

typedef struct {
	POI param_o;
	POI param_s;
	int param_TM;
	int nPOI;
	POI *set_POI;
	int set_M;
	int set_Z;
	int *param_E;
	double **param_t;
} problem_instance;

extern int gene_length;
extern int n_routes;
extern int nreal;
extern int nbin;
extern int n_objectives;
extern int n_constraints;
extern int popsize;
extern double pcross_real;
extern double pcross_bin;
extern double pmut_real;
extern double pmut_bin;
extern double pcross_greedy;
extern double pmut_greedy;
extern double eta_c;
extern double eta_m;
extern int ngen;
extern int nbinmut;
extern int nrealmut;
extern int nbincross;
extern int nrealcross;
extern int *nbits;
extern double *min_realvar;
extern double *max_realvar;
extern double *min_binvar;
extern double *max_binvar;
extern int bitlength;
extern int choice;
extern int obj1;
extern int obj2;
extern int obj3;
extern int angle1;
extern int angle2;

/* Functions from auxiliary.c */
void allocate_memory_pop(population *pop, int size);
void allocate_memory_ind(individual *ind);
void deallocate_memory_pop(population *pop, int size);
void deallocate_memory_ind(individual *ind);

/* Functions from auxiliary.c */
double maximum(double a, double b);
double minimum(double a, double b);
void find_route_bounds(individual *ind, int route, int *start_index, int *end_index);
void find_last_route_bounds(individual *ind, int *start_index, int *end_index);

/* Functions from crossover.c */
void crossover(individual *parent1, individual *parent2, individual *child1, individual *child2, problem_instance *pi);
void rbx_crossover(individual *parent1, individual *parent2, individual *child);
void erc_crossover(individual *parent1, individual *parent2, individual *child, problem_instance *pi);
void subtour_crossover(individual *parent1, individual *parent2, individual *child);

/* Functions from mutation.c */
void mutation_pop(population *pop, problem_instance *pi);
void mutation_ind(individual *ind, problem_instance *pi);
void ars_mutation(individual *ind);
void ers_mutation(individual *ind);
void insert_mutation(individual *ind);
void remove_mutation(individual *ind);
void greedy_insert_mutation(individual *ind, problem_instance *pi);
void replace_mutation(individual *ind, problem_instance *pi);
void min_team_mutation(individual *ind, problem_instance *pi);

/* Functions from reader.c */
int readInputFile(char* filePath, problem_instance *pi);

/* Functions from crowddist.c */
void assign_crowding_distance_list(population *pop, list *lst, int front_size);
void assign_crowding_distance_indices(population *pop, int c1, int c2);
void assign_crowding_distance(population *pop, int *dist, int **obj_array, int front_size);

/* Functions from decode.c */
void decode_pop(population *pop);
void decode_ind(individual *ind);

/* Functions from display.c */
void onthefly_display(population *pop, FILE *gp, int ii);

/* Functions from dominance.c */
int check_dominance(individual *a, individual *b);

/* Functions from eval.c */
void evaluate_pop(population *pop, problem_instance *pi);
void evaluate_ind(individual *ind, problem_instance *pi);
void objective_function_one(individual *ind, problem_instance *pi);
void objective_function_two(individual *ind, problem_instance *pi);
void objective_function_three(individual *ind, problem_instance *pi);
void constraint_one(individual *ind, problem_instance *pi);
void constraint_two(individual *ind, problem_instance *pi);
void constraint_three(individual *ind, problem_instance *pi);

/* Functions from fillnds.c */
void fill_nondominated_sort(population *mixed_pop, population *new_pop);
void crowding_fill(population *mixed_pop, population *new_pop, int count, int front_size, list *cur);

/* Functions from initialize.c */
void initialize_pop(population *pop, problem_instance *pi);
void initialize_ind(individual *ind, problem_instance *pi);

/* Functions from list.c */
void insert(list *node, int x);
list* del(list *node);

/* Functions from merge.c */
void merge(population *pop1, population *pop2, population *pop3);
void copy_ind(individual *ind1, individual *ind2);

/* Functions from rank.c */
void assign_rank_and_crowding_distance(population *new_pop);

/* Functions from report.c */
void report_pop(population *pop, FILE *fpt);
void report_feasible(population *pop, FILE *fpt);
void report_ind(individual *ind, FILE *fpt);

/* Functions from sort.c */
void quicksort_front_obj(population *pop, int objcount, int obj_array[], int obj_array_size);
void q_sort_front_obj(population *pop, int objcount, int obj_array[], int left, int right);
void quicksort_dist(population *pop, int *dist, int front_size);
void q_sort_dist(population *pop, int *dist, int left, int right);

/* Functions from tourselect.c */
void selection(population *old_pop, population *new_pop, problem_instance *pi);
individual* tournament (individual *ind1, individual *ind2);

/* Functions from problem_instance.c */
int isAPOI(int name, problem_instance pi);
int getNodeId(int name, problem_instance pi);
void printProblemInstance(problem_instance *pi);

# endif
