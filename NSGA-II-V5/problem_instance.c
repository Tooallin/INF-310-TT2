/* Routine for evaluating population members  */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include "global.h"
# include "rand.h"

int isAPOI(int name, problem_instance pi) {
	int i;
	int count=0;
	for (i = 0; i < pi.nPOI; i++){
		if (name == pi.set_POI[i].name)
			return 1;
		count++;
	}
	if (name == pi.param_o.name || name == pi.param_s.name) {
		return 0;
	}
	return -1;
}

int getNodeId(int name, problem_instance pi){
	int i;
	for (i = 0; i < pi.nPOI; i++){
		if (name == pi.set_POI[i].name)
			return pi.set_POI[i].id;
	}
	if (name == pi.param_o.name) {
		return pi.param_o.id;
	}
	if (name == pi.param_s.name) {
		return pi.param_s.id;
	}
	return -1;
}

void printProblemInstance(problem_instance *pi){
	int i, j;

	printf("\n");
	printf("Problem Instance!\n\n");
	printf("param o := %d;\n", pi->param_o.name);
	printf("param s := %d;\n", pi->param_s.name);

	printf("\n");

	printf("set POI :=\n");
	for (i = 0; i < pi->nPOI; i++) {
		printf("%d SCORE(%d) OT(%d) TT(%d) CT(%d)", pi->set_POI[i].name, pi->set_POI[i].SCORE, pi->set_POI[i].OT, pi->set_POI[i].TT, pi->set_POI[i].CT);
		for (j = 0; j < pi->set_Z; j++) {
			printf(" e%d(%d)", j+1, pi->set_POI[i].e[j]);
		}
		printf("\n");
	}
	printf(";\n\n");

	printf("set M :=\n");
	for (i = 1; i <= pi->set_M; i++) {
		printf("%d\n", i);
	}
	printf(";\n\n");

	printf("set Z :=\n");
	for (i = 1; i <= pi->set_Z; i++) {
		printf("%d\n", i);
	}
	printf(";\n\n");

	printf("set E :=\n");
	for (i = 0; i < pi->set_Z; i++) {
		printf("%d\n", pi->param_E[i]);
	}
	printf(";\n\n");

	return;
}
