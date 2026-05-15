/* NSGA-II routine (implementation of the 'main' function) */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <unistd.h>
# include <string.h>
# include <sys/stat.h>
# include <sys/types.h>
# include "global.h"
# include "rand.h"

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
	int debug;
	char *instance_route;
	char *instance_name;
	char output_dir[512];
	char fpt1_route[512];
	char fpt2_route[512];
	char fpt3_route[512];
	char fpt5_route[512];
	char fpt_quality_route[512];
	FILE *fpt1;
	FILE *fpt2;
	FILE *fpt3;
	FILE *fpt5;
	FILE *fpt_quality;
	problem_instance *pi;
	population *parent_pop;
	population *child_pop;
	population *mixed_pop;

	debug = 1;
	pi = malloc(sizeof(problem_instance));

	if (argc < 2) {
		printf("\n Usage ./nsga2r instance_route random_seed popsize ngen pcross pmut \n ./nsga2r Instances/c101.dat 0.123 500 2500 0.6 0.01\n");
		exit(1);
	}

	seed = (double)atof(argv[2]);
	if (seed <= 0.0 || seed >= 1.0) {
		printf("\n Entered seed value is wrong, seed value must be in (0,1) \n");
		exit(1);
	}

	instance_route = argv[1];
	instance_name = strrchr(instance_route, '/');
	if (instance_name != NULL) {
		instance_name++;
	} else {
		instance_name = instance_route;
	}

	snprintf(output_dir, sizeof(output_dir), "Outputs/%s", instance_name);
	mkdir(output_dir, 0777);

	snprintf(fpt1_route, sizeof(fpt1_route), "%s/initial_pop_%s_%.3f.out", output_dir, instance_name, seed);
	snprintf(fpt2_route, sizeof(fpt2_route), "%s/final_pop_%s_%.3f.out", output_dir, instance_name, seed);
	snprintf(fpt3_route, sizeof(fpt3_route), "%s/best_pop_%s_%.3f.out", output_dir, instance_name, seed);
	snprintf(fpt5_route, sizeof(fpt5_route), "%s/params_%s_%.3f.out", output_dir, instance_name, seed);
	snprintf(fpt_quality_route, sizeof(fpt_quality_route), "quality.out");

	fpt1 = fopen(fpt1_route,"w");
	fpt2 = fopen(fpt2_route,"w");
	fpt3 = fopen(fpt3_route,"w");
	fpt5 = fopen(fpt5_route,"w");
	fpt_quality = fopen(fpt_quality_route, "w");
	
	fprintf(fpt1,"# This file contains the data of initial population\n");
	fprintf(fpt2,"# This file contains the data of final population\n");
	fprintf(fpt3,"# This file contains the data of final feasible population (if found)\n");
	fprintf(fpt5,"# This file contains information about inputs as read by the program\n");

	readInputFile(instance_route, pi);

	popsize = atoi(argv[3]);
	if (popsize < 4 || (popsize % 4) != 0) {
		printf("\n population size read is : %d",popsize);
		printf("\n Wrong population size entered, hence exiting \n");
		exit (1);
	}

	ngen = atoi(argv[4]);
	if (ngen < 1) {
		printf("\n number of generations read is : %d",ngen);
		printf("\n Wrong nuber of generations entered, hence exiting \n");
		exit (1);
	}

	pcross_bin = atof(argv[5]);
	if (pcross_bin < 0.0 || pcross_bin > 1.0) {
		printf("\n Probability of crossover entered is : %e",pcross_bin);
		printf("\n Entered value of probability of crossover of binary variables is out of bounds, hence exiting \n");
		exit (1);
	}

	pmut_bin = atof(argv[6]);
	if (pmut_bin < 0.0 || pmut_bin > 1.0) {
		printf("\n Probability of mutation entered is : %e",pmut_bin);
		printf("\n Entered value of probability  of mutation of binary variables is out of bounds, hence exiting \n");
		exit (1);
	}

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
	
	nbinmut = 0;
	nrealmut = 0;
	nbincross = 0;
	nrealcross = 0;
	parent_pop = (population *)malloc(sizeof(population));
	child_pop = (population *)malloc(sizeof(population));
	mixed_pop = (population *)malloc(sizeof(population));
	allocate_memory_pop(parent_pop, popsize);
	allocate_memory_pop(child_pop, popsize);
	allocate_memory_pop(mixed_pop, 2*popsize);

	randomize();
	initialize_pop(parent_pop, pi);
	if (debug) printf("\n Initialization done, now performing first generation\n");
	decode_pop(parent_pop);
	if (debug) printf("\n Decode done\n");
	evaluate_pop(parent_pop, pi);
	if (debug) printf("\n Evaluate done\n");
	assign_rank_and_crowding_distance(parent_pop);
	if (debug) printf("\n Rank and Crowding distance done\n");
	
	report_pop(parent_pop, fpt1);
	
	if (debug) printf("\n Report done\n");
	if (debug) printf("\n gen = 1");
	fflush(stdout);

	/*if (choice!=0)
		onthefly_display (parent_pop,gp,1);*/

	fflush(fpt1);
	fflush(fpt2);
	fflush(fpt3);
	fflush(fpt5);
	
	sleep(1);
	for (i = 2; i <= ngen; i++) {
		selection(parent_pop, child_pop, pi);
		mutation_pop(child_pop);
		decode_pop(child_pop);
		evaluate_pop(child_pop, pi);
		merge(parent_pop, child_pop, mixed_pop);
		fill_nondominated_sort (mixed_pop, parent_pop);
		/* Comment following four lines if information for all
		generations is not desired, it will speed up the execution */
		if (debug) printf("\n gen = %d",i);
	}
	if (debug) printf("\n Generations finished, now reporting solutions");
	
	report_pop(parent_pop,fpt2);
	report_feasible(parent_pop,fpt3);

	for (i = 0; i < popsize; i++) {
		// Filtramos solo los individuos factibles (igual que best)
		if (parent_pop->ind[i].constr_violation == 0.0) {
			// Imprime los 3 objetivos separados por espacio y sin decimales (%.0f)
			fprintf(fpt_quality, "%.0f %.0f %.0f\n", 
				parent_pop->ind[i].obj[0], 
				parent_pop->ind[i].obj[1], 
				parent_pop->ind[i].obj[2]);
		}
	}

	if (nreal != 0) {
		fprintf(fpt5,"\n Number of crossover of real variable = %d",nrealcross);
		fprintf(fpt5,"\n Number of mutation of real variable = %d",nrealmut);
	}
	if (nbin != 0) {
		fprintf(fpt5,"\n Number of crossover of binary variable = %d",nbincross);
		fprintf(fpt5,"\n Number of mutation of binary variable = %d",nbinmut);
	}
	
	fflush(stdout);
	fflush(fpt1);
	fflush(fpt2);
	fflush(fpt3);
	fflush(fpt5);
	fflush(fpt_quality);
	
	fclose(fpt1);
	fclose(fpt2);
	fclose(fpt3);
	fclose(fpt5);
	fclose(fpt_quality);
	
	if (nbin != 0){
		free (min_binvar);
		free (max_binvar);
		free (nbits);
	}
	deallocate_memory_pop(parent_pop, popsize);
	deallocate_memory_pop(child_pop, popsize);
	deallocate_memory_pop(mixed_pop, 2*popsize);
	free(parent_pop);
	free(child_pop);
	free(mixed_pop);
	if (debug) printf("\n Routine successfully exited \n");
	return (0);
}