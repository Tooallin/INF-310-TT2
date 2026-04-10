/* Routine for evaluating population members  */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include "global.h"
# include "rand.h"

void findDef(FILE *f, char *def) {
	char word[1024];
	while (fscanf(f, " %1023s", word)) {
		if(strcmp(word,def) == 0) break;
	}
}

void removeSemicolon(char *line){
	char *p = strchr(line, ';');
	if (p) *p = '\0';
}

int countWords(char *line){
	int words;
	char linet[16384], *token;
	strcpy(linet, line);

	words = 0;
	token = strtok(linet, " ");

	while( token != NULL ) {
		words ++;
		token = strtok(NULL, " ");
	}
	return words;
}

void read_o(FILE *f, problem_instance *pi) {
	int debug = 0, param_o;
	char line[1024];

	fgets(line, sizeof(line), f);
	if (debug) printf("Line: %s\n", line);

	removeSemicolon(line);
	param_o = atoi(line);
	if (debug) printf("o := %d\n", param_o);

	pi->param_o.id = param_o;
	pi->param_o.name = param_o;
	pi->param_o.SCORE = 0;
	pi->param_o.OT = 0;
	pi->param_o.TT = 0;
}

void read_s(FILE *f, problem_instance *pi) {
	int debug = 0, param_s;
	char line[1024];

	fgets(line, sizeof(line), f);
	if (debug) printf("Line: %s\n", line);

	removeSemicolon(line);
	param_s = atoi(line);
	if (debug) printf("s := %d\n", param_s);

	pi->param_s.id = param_s;
	pi->param_s.name = param_s;
	pi->param_s.SCORE = 0;
	pi->param_s.OT = 0;
	pi->param_s.TT = 0;
}

void read_POI(FILE *f, problem_instance *pi) {
	int debug = 0, id=0;
	char * token;
	char line[16384];

	fgets(line, sizeof(line), f);
	if (debug) printf("Line: %s\n", line);

	removeSemicolon(line);
	if (debug) printf("Line: %s\n", line);

	pi->nPOI = countWords(line);
	pi->set_POI = malloc(pi->nPOI * sizeof(POI));
	if (debug) printf("nPOI: %d\n", pi->nPOI);

	token=strtok(line, " ");
	while (token != NULL && id < pi->nPOI) {
		pi->set_POI[id].id = id;
		pi->set_POI[id].name = atoi(token);
		token = strtok(NULL, " ");
		id++;
	}
	
	if (debug) {
		for ( id=0; id<pi->nPOI; id++ ) {
			printf ("%d, %d\n", pi->set_POI[id].id, pi->set_POI[id].name);
		}
	}
}

void read_M(FILE *f, problem_instance *pi) {
	int debug = 0;
	char line[1024];

	fgets(line, sizeof(line), f);
	if (debug) printf("Line: %s\n", line);

	removeSemicolon(line);
	if (debug) printf("Line: %s\n", line);

	pi->set_M=countWords(line);
	if (debug) printf("set_M: %d\n", pi->set_M);
}

void read_Z(FILE *f, problem_instance *pi) {
	int debug = 0;
	char line[1024];

	fgets(line, sizeof(line), f);
	if (debug) printf("Line: %s\n", line);

	removeSemicolon(line);
	if (debug) printf("Line: %s\n", line);

	pi->set_Z=countWords(line);
	if (debug) printf("set_Z: %d\n", pi->set_Z);

	pi->param_o.e = calloc(pi->set_Z, sizeof(int));
	pi->param_s.e = calloc(pi->set_Z, sizeof(int));
	pi->param_E = calloc(pi->set_Z, sizeof(int));
}

void read_TM(FILE *f, problem_instance *pi) {
	int debug = 0, param_TM;
	char line[1024];

	fgets(line, sizeof(line), f);
	if (debug) printf("Line: %s\n", line);

	removeSemicolon(line);
	param_TM = atoi(line);
	if (debug) printf("TM := %d\n", param_TM);

	pi->param_TM = param_TM;
	pi->param_o.CT = pi->param_TM;
	pi->param_s.CT = pi->param_TM;
}

void read_E(FILE *f, problem_instance *pi) {
	int debug = 0;
	int idE;
	char *id, *param_E;
	char line[1024];

	fgets(line, sizeof(line), f);

	while (1){
		fgets(line, sizeof(line), f);
		if (debug) printf("Line: %s\n", line);

		id=strtok(line, " ");
		if (strchr(id, ';') != NULL) break;

		param_E = strtok(NULL, " ");
		idE = atoi(id) - 1;
		if (debug) printf("id: %s, idE: %d param_E: %s\n", id, idE, param_E);
		pi->param_E[idE] = atoi(param_E);
	}
}

void read_e(FILE *f, problem_instance *pi) {
	int debug = 0;
	int i, j, val;
	int idPOI;

	for (i = 0; i < pi->nPOI; i++) {
		pi->set_POI[i].e = calloc(pi->set_Z, sizeof(int));
	}

	while (fscanf(f, "%d %d %d", &i, &j, &val) == 3) {
		if (debug) printf("i: %d, j: %d e: %d\n", i, j, val);
		if (i == pi->param_o.name || i == pi->param_s.name) continue;

		if (isAPOI(i, *pi)) {
            idPOI = getNodeId(i, *pi);
            pi->set_POI[idPOI].e[j-1] = val; 
        }
	}
}

void read_SCORE(FILE *f, problem_instance *pi) {
	int debug = 0;
	int idPOI;
	char *id, *SCORE;
	char line[1024];

	fgets(line, sizeof(line), f);

	while (1){
		fgets(line, sizeof(line), f);
		if (debug) printf("Line: %s\n", line);

		id=strtok(line, " ");
		if (strchr(id, ';') != NULL) break;

		if (isAPOI(atoi(id), *pi)){
			SCORE = strtok(NULL, " ");
			idPOI = getNodeId(atoi(id), *pi);
			if (debug) printf("id: %s, idPOI: %d SCORE: %s\n", id, idPOI, SCORE);
			pi->set_POI[idPOI].SCORE = atoi(SCORE);
		}
	}
}

void read_OT(FILE *f, problem_instance *pi) {
	int debug = 0;
	int idPOI;
	char *id, *OT;
	char line[1024];

	fgets(line, sizeof(line), f);

	while (1){
		fgets(line, sizeof(line), f);
		if (debug) printf("Line: %s\n", line);

		id=strtok(line, " ");
		if (strchr(id, ';') != NULL) break;

		if (isAPOI(atoi(id), *pi)){
			OT = strtok(NULL, " ");
			idPOI = getNodeId(atoi(id), *pi);
			if (debug) printf("id: %s, idPOI: %d OT: %s\n", id, idPOI, OT);
			pi->set_POI[idPOI].OT = atoi(OT);
		}
	}
}

void read_TT(FILE *f, problem_instance *pi) {
	int debug = 0;
	int idPOI;
	char *id, *TT;
	char line[1024];

	fgets(line, sizeof(line), f);

	while (1){
		fgets(line, sizeof(line), f);
		if (debug) printf("Line: %s\n", line);

		id=strtok(line, " ");
		if (strchr(id, ';') != NULL) break;

		if (isAPOI(atoi(id), *pi)){
			TT = strtok(NULL, " ");
			idPOI = getNodeId(atoi(id), *pi);
			if (debug) printf("id: %s, idPOI: %d TT: %s\n", id, idPOI, TT);
			pi->set_POI[idPOI].TT = atoi(TT);
		}
	}
}

void read_CT(FILE *f, problem_instance *pi) {
	int debug = 0;
	int idPOI;
	char *id, *CT;
	char line[1024];

	fgets(line, sizeof(line), f);

	while (1){
		fgets(line, sizeof(line), f);
		if (debug) printf("Line: %s\n", line);

		id=strtok(line, " ");
		if (strchr(id, ';') != NULL) break;

		if (isAPOI(atoi(id), *pi)){
			CT = strtok(NULL, " ");
			idPOI = getNodeId(atoi(id), *pi);
			if (debug) printf("id: %s, idPOI: %d CT: %s\n", id, idPOI, CT);
			pi->set_POI[idPOI].CT = atoi(CT);
		}
	}
}

void read_t(FILE *f, problem_instance *pi) {
	int debug = 0, i;
	int totalNodes = pi->nPOI + 2;
	int idFrom, idTo;
	char *from, *to, *distance;
	char line[1024];

	pi->param_t = (double **)malloc(totalNodes * sizeof(double*));
	for(i=0; i<totalNodes; i++) {
		pi->param_t[i] = (double *)malloc(totalNodes * sizeof(double));
	}

	fgets(line, sizeof(line), f);

	while (1){
		fgets(line, sizeof(line), f);
		if (debug) printf("Line: %s\n", line);

		from=strtok(line, " ");
		if (strchr(from, ';') != NULL) break;

		to = strtok(NULL, " ");
		distance = strtok(NULL, " ");
		idFrom = atoi(from);
		idTo = atoi(to);

		if (debug) printf("from: %d, to:%d , param_t: %f\n", idFrom, idTo, atof(distance));
		pi->param_t[idFrom][idTo] = atof(distance);
	}
}

int readInputFile(char* filePath, problem_instance *pi) {
	int debug = 0;
	FILE* fh=fopen(filePath, "r");

	/*check if file exists*/
	if (fh == NULL){
		printf("File does not exists %s", filePath);
		return 0;
	}

	if (debug) printf("Reading: %s \n", filePath);

	findDef(fh, "o:=");
	read_o(fh, pi);
	if (debug) printf("End read_o! \n");

	findDef(fh, "s:=");
	read_s(fh, pi);
	if (debug) printf("End read_s! \n");

	findDef(fh, "POI:=");
	read_POI(fh, pi);
	if (debug) printf("End read_POI! \n");

	findDef(fh, "M:=");
	read_M(fh, pi);
	if (debug) printf("End read_M! \n");

	findDef(fh, "Z:=");
	read_Z(fh, pi);
	if (debug) printf("End read_Z! \n");

	findDef(fh, "TM:=");
	read_TM(fh, pi);
	if (debug) printf("End read_TM! \n");

	findDef(fh, "E:=");
	read_E(fh, pi);
	if (debug) printf("End read_E! \n");

	findDef(fh, "e:=");
	read_e(fh, pi);
	if (debug) printf("End read_e! \n");

	findDef(fh, "SCORE:=");
	read_SCORE(fh, pi);
	if (debug) printf("End read_SCORE! \n");

	findDef(fh, "OT:=");
	read_OT(fh, pi);
	if (debug) printf("End read_OT! \n");

	findDef(fh, "TT:=");
	read_TT(fh, pi);
	if (debug) printf("End read_TT! \n");

	findDef(fh, "CT:=");
	read_CT(fh, pi);
	if (debug) printf("End read_CT! \n");

	findDef(fh, "t:=");
	read_t(fh, pi);
	if (debug) printf("End read_t! \n");

	fclose(fh);
	if (debug) printf("End Reading! \n");

	gene_length = pi->nPOI + pi->set_M;
	n_routes = pi->set_M;
	n_objectives = 3;
	n_constraints = 3;

	if (debug) printProblemInstance(pi);
	if (debug) getchar();

	return 0;
}


