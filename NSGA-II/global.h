/* Archivo de cabecera global: Definiciones de estructuras, constantes y prototipos */

# ifndef _GLOBAL_H_
# define _GLOBAL_H_

/* 1. CONSTANTES MATEMÁTICAS Y DE CONTROL */
# define INF 1.0e14             // Valor para representar el infinito (usado en Crowding Distance)
# define EPS 1.0e-14            // Valor pequeño para comparaciones de punto flotante
# define E  2.71828182845905    // Número de Euler
# define PI 3.14159265358979    // Constante Pi
# define GNUPLOT_COMMAND "gnuplot -persist" // Comando para lanzar el visualizador gráfico

/* 2. ESTRUCTURAS DE DATOS PRINCIPALES DEL ALGORITMO NSGA-II */

/* Estructura que representa una solución única (Individuo) */
typedef struct {
	int rank;                   // Nivel de dominancia (Frente de Pareto al que pertenece)
	double constr_violation;    // Suma total de las violaciones a las restricciones
	int *gene;                  // Cromosoma: Arreglo de largo N+M con POIs y separadores -1 [cite: 300-302]
	double *obj;                // Valores de las 3 funciones objetivo del problema [cite: 122-134]
	double *constr;             // Valores de los castigos para cada una de las 3 restricciones [cite: 354]
	double crowd_dist;          // Métrica de diversidad (Crowding Distance) [cite: 242-244]
} individual;

/* Estructura que agrupa a un conjunto de individuos */
typedef struct {
	individual *ind;            // Arreglo dinámico de individuos que forman la población
} population;

/* Estructura para listas doblemente enlazadas (usada para clasificar frentes dinámicos) */
typedef struct lists {
	int index;                  // Índice del individuo en la población
	struct lists *parent;       // Puntero al nodo anterior
	struct lists *child;        // Puntero al nodo siguiente
} list;

/* 3. ESTRUCTURAS DE DATOS DEL PROBLEMA MCTOPTW */

/* Estructura que representa un Punto de Interés (POI) */
typedef struct {
	int id;                     // Identificador interno (base 0)
	int name;                   // Nombre o etiqueta original del nodo
	int *e;                     // Vector binario de pertenencia a las Z categorías [cite: 117-118]
	int SCORE;                  // Beneficio/Puntaje por visitar el nodo (S_i) [cite: 111-112]
	int OT;                     // Tiempo de apertura de la ventana (O_i) [cite: 113]
	int TT;                     // Tiempo de servicio o duración de la visita (T_i) [cite: 114]
	int CT;                     // Tiempo de cierre de la ventana (C_i) [cite: 115]
} POI;

/* Estructura que contiene todos los datos de la instancia a resolver */
typedef struct {
	POI param_o;                // Nodo de inicio (Depósito de origen) [cite: 87]
	POI param_s;                // Nodo de término (Depósito de llegada) [cite: 87]
	int param_TM;               // Tiempo máximo permitido por cada ruta (T_max) [cite: 88, 121]
	int nPOI;                   // Cantidad total de POIs disponibles (N)
	POI *set_POI;               // Arreglo con todos los puntos de interés
	int set_M;                  // Número de rutas (tours) a construir (M) [cite: 86]
	int set_Z;                  // Cantidad de categorías existentes (Z) [cite: 116]
	int *param_E;               // Límites máximos (cuotas) por cada categoría (E_z) [cite: 119]
	double **param_t;           // Matriz de tiempos de viaje entre todos los nodos (t_ij) [cite: 120]
} problem_instance;

/* 4. DECLARACIÓN DE VARIABLES GLOBALES (Configuración del experimento) */
extern int gene_length;         // Largo total del cromosoma (N + M) 
extern int n_routes;            // Número de rutas (M)
extern int nreal;               // Variables reales (no usadas en esta representación)
extern int nbin;                // Variables binarias
extern int n_objectives;        // Cantidad de funciones objetivo (3 en tu caso) [cite: 123]
extern int n_constraints;       // Cantidad de restricciones evaluadas (3 en tu caso) [cite: 354]
extern int popsize;             // Tamaño de la población (N)
extern double pcross_real;      
extern double pcross_bin;       // Probabilidad de cruzamiento (Crossover)
extern double pmut_real;
extern double pmut_bin;         // Probabilidad de mutación
extern double eta_c;
extern double eta_m;
extern int ngen;                // Número máximo de generaciones
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
extern int obj1;                // Identificador para visualización del objetivo 1
extern int obj2;                // Identificador para visualización del objetivo 2
extern int obj3;                // Identificador para visualización del objetivo 3
extern int angle1;              // Ángulo de cámara para gráfico 3D
extern int angle2;              // Ángulo de cámara para gráfico 3D

/* 5. PROTOTIPOS DE FUNCIONES (Declaración de la interfaz entre archivos) */

/* Funciones de gestión de memoria (allocate.c) */
void allocate_memory_pop(population *pop, int size);
void allocate_memory_ind(individual *ind);
void deallocate_memory_pop(population *pop, int size);
void deallocate_memory_ind(individual *ind);

/* Funciones auxiliares y de navegación (auxiliary.c) */
double maximum(double a, double b);
double minimum(double a, double b);
void find_route_bounds(individual *ind, int route, int *start_index, int *end_index);
void find_last_route_bounds(individual *ind, int *start_index, int *end_index);

/* Operador de Cruzamiento (crossover.c) */
void crossover(individual *parent1, individual *parent2, individual *child1, individual *child2, problem_instance *pi);
void rbx_crossover(individual *parent1, individual *parent2, individual *child);

/* Operadores de Mutación (mutation.c) */
void mutation_pop(population *pop);
void mutation_ind(individual *ind);
void ars_mutation(individual *ind);
void ers_mutation(individual *ind);
void insert_mutation(individual *ind);
void remove_mutation(individual *ind);

/* Lector de archivos (reader.c) */
int readInputFile(char* filePath, problem_instance *pi);

/* Cálculos de diversidad (crowddist.c) */
void assign_crowding_distance_list(population *pop, list *lst, int front_size);
void assign_crowding_distance_indices(population *pop, int c1, int c2);
void assign_crowding_distance(population *pop, int *dist, int **obj_array, int front_size);

/* Decodificación (decode.c) */
void decode_pop(population *pop);
void decode_ind(individual *ind);

/* Visualización (display.c) */
void onthefly_display(population *pop, FILE *gp, int ii);

/* Evaluación de dominancia (dominance.c) */
int check_dominance(individual *a, individual *b);

/* Funciones de Evaluación de tu modelo (eval.c) */
void evaluate_pop(population *pop, problem_instance *pi);
void evaluate_ind(individual *ind, problem_instance *pi);
void objective_function_one(individual *ind, problem_instance *pi);
void objective_function_two(individual *ind, problem_instance *pi);
void objective_function_three(individual *ind, problem_instance *pi);
void constraint_one(individual *ind, problem_instance *pi);
void constraint_two(individual *ind, problem_instance *pi);
void constraint_three(individual *ind, problem_instance *pi);

/* Ordenamiento y elitismo (fillnds.c) */
void fill_nondominated_sort(population *mixed_pop, population *new_pop);
void crowding_fill(population *mixed_pop, population *new_pop, int count, int front_size, list *cur);

/* Creación de soluciones (initialize.c) */
void initialize_pop(population *pop, problem_instance *pi);
void initialize_ind(individual *ind, problem_instance *pi);

/* Manejo de listas enlazadas (list.c) */
void insert(list *node, int x);
list* del(list *node);

/* Fusión de poblaciones y copias (merge.c) */
void merge(population *pop1, population *pop2, population *pop3);
void copy_ind(individual *ind1, individual *ind2);

/* Clasificación por Pareto (rank.c) */
void assign_rank_and_crowding_distance(population *new_pop);

/* Generación de archivos de salida (report.c) */
void report_pop(population *pop, FILE *fpt);
void report_feasible(population *pop, FILE *fpt);
void report_ind(individual *ind, FILE *fpt);

/* Algoritmo de ordenamiento rápido (sort.c) */
void quicksort_front_obj(population *pop, int objcount, int obj_array[], int obj_array_size);
void q_sort_front_obj(population *pop, int objcount, int obj_array[], int left, int right);
void quicksort_dist(population *pop, int *dist, int front_size);
void q_sort_dist(population *pop, int *dist, int left, int right);

/* Selección de padres (tourselect.c) */
void selection(population *old_pop, population *new_pop, problem_instance *pi);
individual* tournament (individual *ind1, individual *ind2);

/* Consultas de la instancia (problem_instance.c) */
int isAPOI(int name, problem_instance pi);
int getNodeId(int name, problem_instance pi);
void printProblemInstance(problem_instance *pi);

# endif