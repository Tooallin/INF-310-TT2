/* Rutina principal de NSGA-II (Implementación de la función 'main' y ciclo evolutivo) */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <unistd.h>
# include <string.h>

# include "global.h"
# include "rand.h"

// Variables globales para la configuración del algoritmo y el problema
int nreal;
int nbin;
int n_objectives;
int n_constraints;
int popsize;
double pcross_real;
double pcross_bin;
double pmut_real;
double pmut_bin;
double eta_c;
double eta_m;
int gene_length;
int n_routes;
int ngen;
int nbinmut;
int nrealmut;
int nbincross;
int nrealcross;
int *nbits;
double *min_realvar;
double *max_realvar;
double *min_binvar;
double *max_binvar;
int bitlength;
int choice;
int obj1;
int obj2;
int obj3;
int angle1;
int angle2;

int main (int argc, char **argv) {
	int i;
	int debug = 0; // Bandera para imprimir mensajes de seguimiento en consola
	FILE *fpt1; // Archivo: Población inicial
	FILE *fpt2; // Archivo: Población final
	FILE *fpt3; // Archivo: Mejores soluciones factibles
	FILE *fpt4; // Archivo: Historial de todas las generaciones
	FILE *fpt5; // Archivo: Registro de parámetros utilizados

	// Reserva de memoria para la instancia del problema MCTOPTW
	problem_instance *pi = malloc(sizeof(problem_instance));
	char *instance_route;
	char *instance_name;
	char fpt1_route[256];
	char fpt2_route[256];
	char fpt3_route[256];
	char fpt4_route[256];
	char fpt5_route[256];
	
	// Punteros a las 3 poblaciones clave del Algoritmo NSGA-II
	population *parent_pop; // P_g (Población actual/padres)
	population *child_pop;  // Q_g (Población hija)
	population *mixed_pop;  // R_g (Población combinada de tamaño 2N)

	// 1. VALIDACIÓN DE ARGUMENTOS DE ENTRADA (CLI)
	if (argc < 2) {
		printf("\n Usage ./nsga2r instance_route random_seed popsize ngen pcross pmut \n ./nsga2r Instances/c101.dat 0.123 500 2500 0.6 0.01\n");
		exit(1);
	}

	// 2. CONFIGURACIÓN DE LA SEMILLA ALEATORIA
	seed = (double)atof(argv[2]);
	if (seed <= 0.0 || seed >= 1.0) {
		printf("\n Entered seed value is wrong, seed value must be in (0,1) \n");
		exit(1);
	}

	// 3. CONFIGURACIÓN DE RUTAS Y ARCHIVOS DE SALIDA
	instance_route = argv[1];
	instance_name = strrchr(instance_route, '/');
	if (instance_name != NULL) {
		instance_name++;
	} else {
		instance_name = instance_route;
	}
	
	// Prepara los nombres de los archivos de reporte inyectando el nombre de la instancia y la semilla
	snprintf(fpt1_route, sizeof(fpt1_route), "Outputs/initial_pop_%s_%.3f.out", instance_name, seed);
	snprintf(fpt2_route, sizeof(fpt2_route), "Outputs/final_pop_%s_%.3f.out", instance_name, seed);
	snprintf(fpt3_route, sizeof(fpt3_route), "Outputs/best_pop_%s_%.3f.out", instance_name, seed);
	snprintf(fpt4_route, sizeof(fpt4_route), "Outputs/all_pop_%s_%.3f.out", instance_name, seed);
	snprintf(fpt5_route, sizeof(fpt4_route), "Outputs/params_%s_%.3f.out", instance_name, seed);

	// Apertura de archivos en modo escritura ("w")
	fpt1 = fopen(fpt1_route,"w");
	fpt2 = fopen(fpt2_route,"w");
	fpt3 = fopen(fpt3_route,"w");
	fpt4 = fopen(fpt4_route,"w");
	fpt5 = fopen(fpt5_route,"w");
	fprintf(fpt1,"# This file contains the data of initial population\n");
	fprintf(fpt2,"# This file contains the data of final population\n");
	fprintf(fpt3,"# This file contains the data of final feasible population (if found)\n");
	fprintf(fpt4,"# This file contains the data of all generations\n");
	fprintf(fpt5,"# This file contains information about inputs as read by the program\n");

	// 4. CARGA DE LA INSTANCIA DEL MCTOPTW
	// Lee el archivo .dat (ej. c101.dat) y llena la estructura 'pi' con nodos, ventanas de tiempo y costos.
	readInputFile(instance_route, pi);

	// 5. LECTURA DE PARÁMETROS EVOLUTIVOS
	popsize = atoi(argv[3]); // Tamaño de la población (N)
	if (popsize < 4 || (popsize % 4) != 0) {
		printf("\n population size read is : %d",popsize);
		printf("\n Wrong population size entered, hence exiting \n");
		exit (1);
	}

	ngen = atoi(argv[4]); // Número máximo de generaciones (MaxGen)
	if (ngen < 1) {
		printf("\n number of generations read is : %d",ngen);
		printf("\n Wrong nuber of generations entered, hence exiting \n");
		exit (1);
	}

	pcross_bin = atof(argv[5]); // Probabilidad de cruzamiento (Route-based crossover)
	if (pcross_bin < 0.0 || pcross_bin > 1.0) {
		printf("\n Probability of crossover entered is : %e",pcross_bin);
		printf("\n Entered value of probability of crossover of binary variables is out of bounds, hence exiting \n");
		exit (1);
	}

	pmut_bin = atof(argv[6]); // Probabilidad de mutación (Insert, Remove, Swap...)
	if (pmut_bin < 0.0 || pmut_bin > 1.0) {
		printf("\n Probability of mutation entered is : %e",pmut_bin);
		printf("\n Entered value of probability  of mutation of binary variables is out of bounds, hence exiting \n");
		exit (1);
	}

	// Escritura de cabeceras y parámetros en los archivos de log
	if (debug) printf("\n Input data successfully entered, now performing initialization \n");
	fprintf(fpt5,"\n Population size = %d",popsize);
	fprintf(fpt5,"\n Number of generations = %d",ngen);
	fprintf(fpt5,"\n Number of objective functions = %d",n_objectives);

	fprintf(fpt5,"\n Number of binary variables = %d",nbin);
	if (nbin != 0) {
		for (i = 0; i < nbin; i++) {
			fprintf(fpt5,"\n Number of bits for binary variable %d = %d",i+1,nbits[i]);
			fprintf(fpt5,"\n Lower limit of binary variable %d = %e",i+1,min_binvar[i]);
			fprintf(fpt5,"\n Upper limit of binary variable %d = %e",i+1,max_binvar[i]);
		}
		fprintf(fpt5,"\n Probability of crossover of binary variable = %e",pcross_bin);
		fprintf(fpt5,"\n Probability of mutation of binary variable = %e",pmut_bin);
	}
	fprintf(fpt5,"\n Seed for random number generator = %e",seed);

	bitlength = 0;
	if (nbin != 0) {
		for (i=0; i<nbin; i++) {
			bitlength += nbits[i];
		}
	}

	fprintf(fpt1,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n", n_objectives, n_constraints, nreal, bitlength);
	fprintf(fpt2,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n", n_objectives, n_constraints, nreal, bitlength);
	fprintf(fpt3,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n", n_objectives, n_constraints, nreal, bitlength);
	fprintf(fpt4,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n", n_objectives, n_constraints, nreal, bitlength);
	
	// 6. ASIGNACIÓN DE MEMORIA (Llamadas a allocate.c)
	nbinmut = 0;
	nrealmut = 0;
	nbincross = 0;
	nrealcross = 0;
	parent_pop = (population *)malloc(sizeof(population));
	child_pop = (population *)malloc(sizeof(population));
	mixed_pop = (population *)malloc(sizeof(population));
	allocate_memory_pop(parent_pop, popsize);   // Población N
	allocate_memory_pop(child_pop, popsize);    // Población N
	allocate_memory_pop(mixed_pop, 2*popsize);  // Población 2N

	// =========================================================================
	//  7. INICIALIZACIÓN DE NSGA-II (Generación 1)
	// =========================================================================
	randomize();
	
	// Inicializa genotipos respetando el T_max inicial
	initialize_pop(parent_pop, pi); 
	if (debug) printf("\n Initialization done, now performing first generation\n");
	
	// Mapeo Genotipo a Fenotipo (vacío en este caso por representación directa)
	decode_pop(parent_pop); 
	if (debug) printf("\n Decode done\n");
	
	// Evalúa Funciones Objetivo 1, 2, 3 y las Restricciones 9, 10, 11
	evaluate_pop(parent_pop, pi); 
	if (debug) printf("\n Evaluate done\n");
	
	// Clasifica en frentes (Rank) y calcula distancia de diversidad (Crowding)
	assign_rank_and_crowding_distance(parent_pop); 
	if (debug) printf("\n Rank and Crowding distance done\n");
	
	// Reporte de la generación inicial
	report_pop(parent_pop, fpt1); 
	if (debug) printf("\n Report done\n");
	fprintf(fpt4,"# gen = 1\n");
	report_pop(parent_pop,fpt4);
	if (debug) printf("\n gen = 1");
	fflush(stdout);

	fflush(fpt1); fflush(fpt2); fflush(fpt3); fflush(fpt4); fflush(fpt5);
	sleep(1);

	// =========================================================================
	//  8. CICLO EVOLUTIVO PRINCIPAL (Generaciones 2 hasta MaxGen)
	// =========================================================================
	for (i = 2; i <= ngen; i++) {
		
		// 8.1 CREACIÓN DE POBLACIÓN HIJA (Q_g)
		// Ejecuta Torneos Binarios para seleccionar padres y luego aplica Crossover.
		selection(parent_pop, child_pop, pi);
		
		// 8.2 MUTACIÓN
		// Aplica aleatoriamente Swap, Insert o Remove sobre la población hija.
		mutation_pop(child_pop);
		
		decode_pop(child_pop);
		
		// 8.3 EVALUACIÓN DE LA POBLACIÓN HIJA
		// Calcula nuevos puntajes, tiempos y castigos.
		evaluate_pop(child_pop, pi);
		
		// 8.4 COMBINACIÓN DE POBLACIONES
		// Ejecuta Línea 5 del Algoritmo: R_g = P_g U Q_g
		merge(parent_pop, child_pop, mixed_pop);
		
		// 8.5 ORDENAMIENTO Y PODA (Reemplazo Generacional)
		// Ejecuta Líneas 6 a 17 del Algoritmo 1.
		// Ordena a los 2N individuos de mixed_pop y rescata a los N mejores 
		// (por dominancia y crowding distance) guardándolos de vuelta en parent_pop.
		fill_nondominated_sort (mixed_pop, parent_pop);
		
		// Reporte de la generación actual
		/* Comment following four lines if information for all
		generations is not desired, it will speed up the execution */
		fprintf(fpt4,"# gen = %d\n",i);
		report_pop(parent_pop,fpt4);
		fflush(fpt4);
		if (debug) printf("\n gen = %d",i);
	}
	
	// =========================================================================
	//  9. FIN DEL ALGORITMO Y REPORTES
	// =========================================================================
	if (debug) printf("\n Generations finished, now reporting solutions");
	
	// Escribe el frente de Pareto final en el archivo fpt2
	report_pop(parent_pop,fpt2);
	
	// Escribe exclusivamente las soluciones que tuvieron castigo = 0 en fpt3
	report_feasible(parent_pop,fpt3);
	
	// Reporte de contadores estadísticos de operadores
	if (nreal != 0) {
		fprintf(fpt5,"\n Number of crossover of real variable = %d",nrealcross);
		fprintf(fpt5,"\n Number of mutation of real variable = %d",nrealmut);
	}
	if (nbin != 0) {
		fprintf(fpt5,"\n Number of crossover of binary variable = %d",nbincross);
		fprintf(fpt5,"\n Number of mutation of binary variable = %d",nbinmut);
	}
	fflush(stdout);
	fflush(fpt1); fflush(fpt2); fflush(fpt3); fflush(fpt4); fflush(fpt5);
	
	// 10. LIBERACIÓN DE MEMORIA Y CIERRE DE ARCHIVOS
	fclose(fpt1); fclose(fpt2); fclose(fpt3); fclose(fpt4); fclose(fpt5);
	if (nbin != 0){
		free (min_binvar);
		free (max_binvar);
		free (nbits);
	}
	
	// Devuelve la RAM solicitada para evitar memory leaks (Llamadas a allocate.c)
	deallocate_memory_pop(parent_pop, popsize);
	deallocate_memory_pop(child_pop, popsize);
	deallocate_memory_pop(mixed_pop, 2*popsize);
	free(parent_pop);
	free(child_pop);
	free(mixed_pop);
	free(pi);
	
	if (debug) printf("\n Routine successfully exited \n");
	return (0);
}