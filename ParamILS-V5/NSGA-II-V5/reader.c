/* Routine for evaluating population members  */
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include "global.h"
# include "rand.h"

void removeSemicolon(char *line){
    char *p = strchr(line, ';');
    if (p) *p = '\0';
}

void skip_to_keyword(FILE *f, const char *keyword) {
    char word[1024];
    rewind(f); 
    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, keyword) == 0) return;
    }
}

void read_o(FILE *f, problem_instance *pi) {
    char word[1024];
    skip_to_keyword(f, "o");
    fscanf(f, "%1023s", word);
    fscanf(f, "%1023s", word);
    removeSemicolon(word);
    
    int param_o = atoi(word);
    pi->param_o.id = param_o;
    pi->param_o.name = param_o;
    pi->param_o.SCORE = 0;
    pi->param_o.OT = 0;
    pi->param_o.TT = 0;
}

void read_s(FILE *f, problem_instance *pi) {
    char word[1024];
    skip_to_keyword(f, "s");
    fscanf(f, "%1023s", word);
    fscanf(f, "%1023s", word);
    removeSemicolon(word);
    
    int param_s = atoi(word);
    pi->param_s.id = param_s;
    pi->param_s.name = param_s;
    pi->param_s.SCORE = 0;
    pi->param_s.OT = 0;
    pi->param_s.TT = 0;
}

void read_POI(FILE *f, problem_instance *pi) {
    char word[1024];
    skip_to_keyword(f, "POI");
    fscanf(f, "%1023s", word);

    int capacity = 100;
    pi->nPOI = 0;
    pi->set_POI = malloc(capacity * sizeof(POI));

    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, ";") == 0) break;
        
        int has_semi = (strchr(word, ';') != NULL);
        removeSemicolon(word);
        
        if (strlen(word) > 0) {
            if (pi->nPOI >= capacity) {
                capacity *= 2;
                pi->set_POI = realloc(pi->set_POI, capacity * sizeof(POI));
            }
            pi->set_POI[pi->nPOI].id = pi->nPOI;
            pi->set_POI[pi->nPOI].name = atoi(word);
            pi->nPOI++;
        }
        if (has_semi) break;
    }
}

void read_M(FILE *f, problem_instance *pi) {
    char word[1024];
    skip_to_keyword(f, "M");
    fscanf(f, "%1023s", word);

    pi->set_M = 0;
    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, ";") == 0) break;
        int has_semi = (strchr(word, ';') != NULL);
        removeSemicolon(word);
        
        if (strlen(word) > 0) pi->set_M++;
        if (has_semi) break;
    }
}

void read_Z(FILE *f, problem_instance *pi) {
    char word[1024];
    skip_to_keyword(f, "Z");
    fscanf(f, "%1023s", word);

    pi->set_Z = 0;
    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, ";") == 0) break;
        int has_semi = (strchr(word, ';') != NULL);
        removeSemicolon(word);
        
        if (strlen(word) > 0) pi->set_Z++;
        if (has_semi) break;
    }

    pi->param_o.e = calloc(pi->set_Z, sizeof(int));
    pi->param_s.e = calloc(pi->set_Z, sizeof(int));
    pi->param_E = calloc(pi->set_Z, sizeof(int));
}

void read_TM(FILE *f, problem_instance *pi) {
    char word[1024];
    skip_to_keyword(f, "TM");
    fscanf(f, "%1023s", word);
    fscanf(f, "%1023s", word);
    removeSemicolon(word);
    
    pi->param_TM = atoi(word);
    pi->param_o.CT = pi->param_TM;
    pi->param_s.CT = pi->param_TM;
}

void read_E(FILE *f, problem_instance *pi) {
    char word[1024];
    skip_to_keyword(f, "E");
    fscanf(f, "%1023s", word);

    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, ";") == 0) break;
        int idE = atoi(word) - 1;
        
        fscanf(f, "%1023s", word);
        int has_semi = (strchr(word, ';') != NULL);
        removeSemicolon(word);
        
        pi->param_E[idE] = atoi(word);
        if (has_semi) break;
    }
}

void read_e(FILE *f, problem_instance *pi) {
    char word[1024];
    int i, j;

    for (i = 0; i < pi->nPOI; i++) {
        pi->set_POI[i].e = calloc(pi->set_Z, sizeof(int));
    }

    skip_to_keyword(f, "e");
    
    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, ":=") == 0) break;
    }

    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, ";") == 0) break;
        
        int row = atoi(word);
        for (j = 0; j < pi->set_Z; j++) {
            fscanf(f, "%1023s", word);
            int has_semi = (strchr(word, ';') != NULL);
            removeSemicolon(word);
            int val = atoi(word);
            
            if (row != pi->param_o.name && row != pi->param_s.name) {
                if (isAPOI(row, *pi)) {
                    int idPOI = getNodeId(row, *pi);
                    pi->set_POI[idPOI].e[j] = val;
                }
            }
            if (has_semi && j == pi->set_Z - 1) break; 
        }
        if (strchr(word, ';') != NULL) break;
    }
}

void read_SCORE(FILE *f, problem_instance *pi) {
    char word[1024];
    skip_to_keyword(f, "SCORE");
    fscanf(f, "%1023s", word);
    
    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, ";") == 0) break;
        int row = atoi(word);
        
        fscanf(f, "%1023s", word);
        int has_semi = (strchr(word, ';') != NULL);
        removeSemicolon(word);
        
        if (isAPOI(row, *pi)) {
            int idPOI = getNodeId(row, *pi);
            pi->set_POI[idPOI].SCORE = atoi(word);
        }
        if (has_semi) break;
    }
}

void read_OT(FILE *f, problem_instance *pi) {
    char word[1024];
    skip_to_keyword(f, "OT");
    fscanf(f, "%1023s", word);
    
    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, ";") == 0) break;
        int row = atoi(word);
        
        fscanf(f, "%1023s", word);
        int has_semi = (strchr(word, ';') != NULL);
        removeSemicolon(word);
        
        if (isAPOI(row, *pi)) {
            int idPOI = getNodeId(row, *pi);
            pi->set_POI[idPOI].OT = atoi(word);
        }
        if (has_semi) break;
    }
}

void read_TT(FILE *f, problem_instance *pi) {
    char word[1024];
    skip_to_keyword(f, "TT");
    fscanf(f, "%1023s", word);
    
    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, ";") == 0) break;
        int row = atoi(word);
        
        fscanf(f, "%1023s", word);
        int has_semi = (strchr(word, ';') != NULL);
        removeSemicolon(word);
        
        if (isAPOI(row, *pi)) {
            int idPOI = getNodeId(row, *pi);
            pi->set_POI[idPOI].TT = atoi(word);
        }
        if (has_semi) break;
    }
}

void read_CT(FILE *f, problem_instance *pi) {
    char word[1024];
    skip_to_keyword(f, "CT");
    fscanf(f, "%1023s", word);
    
    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, ";") == 0) break;
        int row = atoi(word);
        
        fscanf(f, "%1023s", word);
        int has_semi = (strchr(word, ';') != NULL);
        removeSemicolon(word);
        
        if (isAPOI(row, *pi)) {
            int idPOI = getNodeId(row, *pi);
            pi->set_POI[idPOI].CT = atoi(word);
        }
        if (has_semi) break;
    }
}

void read_t(FILE *f, problem_instance *pi) {
    char word[1024];
    int i;
    int totalNodes = pi->nPOI + 2; 

    pi->param_t = (double **)malloc(totalNodes * sizeof(double*));
    for(i = 0; i < totalNodes; i++) {
        pi->param_t[i] = (double *)malloc(totalNodes * sizeof(double));
    }

    skip_to_keyword(f, "t");
    fscanf(f, "%1023s", word);

    while (fscanf(f, "%1023s", word) == 1) {
        if (strcmp(word, ";") == 0) break;
        int from = atoi(word);
        
        fscanf(f, "%1023s", word);
        int to = atoi(word);
        
        fscanf(f, "%1023s", word);
        int has_semi = (strchr(word, ';') != NULL);
        removeSemicolon(word);
        double dist = atof(word);
        
        pi->param_t[from][to] = dist;
        
        if (has_semi) break;
    }
}

int readInputFile(char* filePath, problem_instance *pi) {
    int debug = 0;
    FILE* fh = fopen(filePath, "r");

    if (fh == NULL){
        printf("File does not exists %s\n", filePath);
        return 0;
    }

    if (debug) printf("Reading: %s \n", filePath);

    read_o(fh, pi);
    read_s(fh, pi);
    read_POI(fh, pi);
    read_M(fh, pi);
    read_Z(fh, pi);
    read_TM(fh, pi);
    read_E(fh, pi);
    read_e(fh, pi);
    read_SCORE(fh, pi);
    read_OT(fh, pi);
    read_TT(fh, pi);
    read_CT(fh, pi);
    read_t(fh, pi);

    fclose(fh);
    if (debug) printf("End Reading! \n");

    gene_length = pi->nPOI + pi->set_M;
    n_routes = pi->set_M;
    n_objectives = 3;
    n_constraints = 3;

    if (debug) printProblemInstance(pi);

    return 0;
}