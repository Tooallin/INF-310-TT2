/* Rutinas para leer y cargar los datos de la instancia desde un archivo .dat */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include "global.h"
# include "rand.h"

/* * Busca en el archivo de texto hasta encontrar la definición buscada (ej. "POI:=") */
void findDef(FILE *f, char *def) {
	char word[1024];
	while (fscanf(f, " %1023s", word)) {
		if(strcmp(word,def) == 0) break;
	}
}

/* * Elimina el punto y coma ';' al final de una línea para evitar errores de conversión a entero */
void removeSemicolon(char *line){
	char *p = strchr(line, ';');
	if (p) *p = '\0';
}

/* * Cuenta cuántos tokens (palabras/números) hay en una línea. 
 * Útil para saber dinámicamente cuántos POIs o Rutas (M) existen.
 */
int countWords(char *line){
	int words;
	char linet[1024], *token;
	strcpy(linet, line);

	words = 0;
	token = strtok(linet, " ");

	while( token != NULL ) {
		words ++;
		token = strtok(NULL, " ");
	}
	return words;
}

/* * Lee el nodo de origen (o) */
void read_o(FILE *f, problem_instance *pi) {
	int debug = 0, param_o;
	char line[1024];

	fgets(line, sizeof(line), f);
	if (debug) printf("Line: %s\n", line);

	removeSemicolon(line);
	param_o = atoi(line); // Convierte el string a entero
	if (debug) printf("o := %d\n", param_o);

	// Inicializa las propiedades del nodo de inicio (Puntaje 0, sin ventanas de tiempo)
	pi->param_o.id = param_o;
	pi->param_o.name = param_o;
	pi->param_o.SCORE = 0;
	pi->param_o.OT = 0;
	pi->param_o.TT = 0;
}

/* * Lee el nodo de destino (s) */
void read_s(FILE *f, problem_instance *pi) {
	int debug = 0, param_s;
	char line[1024];

	fgets(line, sizeof(line), f);
	if (debug) printf("Line: %s\n", line);

	removeSemicolon(line);
	param_s = atoi(line);
	if (debug) printf("s := %d\n", param_s);

	// Inicializa las propiedades del nodo de destino
	pi->param_s.id = param_s;
	pi->param_s.name = param_s;
	pi->param_s.SCORE = 0;
	pi->param_s.OT = 0;
	pi->param_s.TT = 0;
}

/* * Lee el conjunto de Puntos de Interés (POI) */
void read_POI(FILE *f, problem_instance *pi) {
	int debug = 0, id=0;
	char * token;
	char line[1024];

	fgets(line, sizeof(line), f);
	if (debug) printf("Line: %s\n", line);

	removeSemicolon(line);
	if (debug) printf("Line: %s\n", line);

	// Cuenta cuántos POIs hay en la línea y reserva la memoria exacta
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
}

/* * Lee la cantidad de rutas a generar (M) */
void read_M(FILE *f, problem_instance *pi) {
	int debug = 0;
	char line[1024];

	fgets(line, sizeof(line), f);
	removeSemicolon(line);
	pi->set_M=countWords(line);
}

/* * Lee la cantidad de categorías (Z) y prepara los arreglos de pertenencia */
void read_Z(FILE *f, problem_instance *pi) {
	int debug = 0;
	char line[1024];

	fgets(line, sizeof(line), f);
	removeSemicolon(line);
	pi->set_Z=countWords(line);

	// Inicializa en ceros la pertenencia a categorías para los nodos o y s, y el arreglo de cuotas
	pi->param_o.e = calloc(pi->set_Z, sizeof(int));
	pi->param_s.e = calloc(pi->set_Z, sizeof(int));
	pi->param_E = calloc(pi->set_Z, sizeof(int));
}

/* * Lee el presupuesto de tiempo máximo de las rutas (T_max) */
void read_TM(FILE *f, problem_instance *pi) {
	int debug = 0, param_TM;
	char line[1024];

	fgets(line, sizeof(line), f);
	removeSemicolon(line);
	param_TM = atoi(line);

	pi->param_TM = param_TM;
	
	// Por defecto, asume que los nodos de inicio y fin cierran cuando se acaba T_max
	pi->param_o.CT = pi->param_TM;
	pi->param_s.CT = pi->param_TM;
}

/* * Lee las cuotas máximas por categoría E_z */
void read_E(FILE *f, problem_instance *pi) {
	int debug = 0;
	int idE;
	char *id, *param_E;
	char line[1024];

	fgets(line, sizeof(line), f);

	while (1){
		fgets(line, sizeof(line), f);
		
		id=strtok(line, " ");
		if (strchr(id, ';') != NULL) break;

		param_E = strtok(NULL, " ");
		idE = atoi(id) - 1; // Ajusta a índice base 0
		pi->param_E[idE] = atoi(param_E);
	}
}

/* * Lee a qué categorías pertenece cada POI (Matriz e_iz).
 * ⚠️ ATENCIÓN: Este código asume que el archivo .dat está en formato lista con 3 enteros por línea:
 * [Nodo_i] [Categoria_z] [Valor_0_o_1].
 * Si usas el formato de matriz 2D de AMPL, fscanf() leerá basura y arruinará los datos de C.
 */
void read_e(FILE *f, problem_instance *pi) {
	int debug = 0;
	int i, j, val;

	for (i = 0; i < pi->nPOI; i++) {
		pi->set_POI[i].e = calloc(pi->set_Z, sizeof(int));
	}

	// Lee exactamente 3 enteros en cada iteración
	while (fscanf(f, "%d %d %d", &i, &j, &val) == 3) {
		if (i == pi->param_o.name || i == pi->param_s.name) continue;
		pi->set_POI[i-1].e[j-1] = val; // Mapea la lectura a los arreglos base 0
	}
}

/* * Lee los puntajes de los nodos S_i */
void read_SCORE(FILE *f, problem_instance *pi) {
	int debug = 0;
	int idPOI;
	char *id, *SCORE;
	char line[1024];

	fgets(line, sizeof(line), f);

	while (1){
		fgets(line, sizeof(line), f);
		id=strtok(line, " ");
		if (strchr(id, ';') != NULL) break;

		// Solo registra puntajes para los POIs válidos
		if (isAPOI(atoi(id), *pi)){
			SCORE = strtok(NULL, " ");
			idPOI = getNodeId(atoi(id), *pi);
			pi->set_POI[idPOI].SCORE = atoi(SCORE);
		}
	}
}

/* Funciones homólogas para Open Time (OT), Travel Time (TT) y Close Time (CT) */
void read_OT(FILE *f, problem_instance *pi) { /* ... Similar a read_SCORE ... */ }
void read_TT(FILE *f, problem_instance *pi) { /* ... Similar a read_SCORE ... */ }
void read_CT(FILE *f, problem_instance *pi) { /* ... Similar a read_SCORE ... */ }

/* * Lee los tiempos de viaje (Matriz t_ij).
 * ⚠️ ATENCIÓN: Al igual que read_e, este parser espera un formato muy específico por línea.
 */
void read_t(FILE *f, problem_instance *pi) {
	int debug = 0, i;
	int totalNodes = pi->nPOI + 2;
	int idFrom, idTo;
	char *from, *to, *distance;
	char line[1024];

	// Construye dinámicamente la matriz de adyacencia
	pi->param_t = (double **)malloc(totalNodes * sizeof(double*));
	for(i=0; i<totalNodes; i++) {
		pi->param_t[i] = (double *)malloc(totalNodes * sizeof(double));
	}

	fgets(line, sizeof(line), f);

	while (1){
		fgets(line, sizeof(line), f);
		from=strtok(line, " ");
		if (strchr(from, ';') != NULL) break;

		to = strtok(NULL, " ");
		distance = strtok(NULL, " ");
		idFrom = atoi(from);
		idTo = atoi(to);

		pi->param_t[idFrom][idTo] = atof(distance);
	}
}

/* * Orquestador principal de lectura. Define el orden en el que se deben leer los parámetros. */
int readInputFile(char* filePath, problem_instance *pi) {
	int debug = 0;
	FILE* fh=fopen(filePath, "r");

	if (fh == NULL){
		printf("File does not exists %s", filePath);
		return 0;
	}

	// Llama secuencialmente a findDef y al lector específico
	findDef(fh, "o:="); read_o(fh, pi);
	findDef(fh, "s:="); read_s(fh, pi);
	findDef(fh, "POI:="); read_POI(fh, pi);
	findDef(fh, "M:="); read_M(fh, pi);
	findDef(fh, "Z:="); read_Z(fh, pi);
	findDef(fh, "TM:="); read_TM(fh, pi);
	findDef(fh, "E:="); read_E(fh, pi);
	findDef(fh, "e:="); read_e(fh, pi);
	findDef(fh, "SCORE:="); read_SCORE(fh, pi);
	findDef(fh, "OT:="); read_OT(fh, pi);
	findDef(fh, "TT:="); read_TT(fh, pi);
	findDef(fh, "CT:="); read_CT(fh, pi);
	findDef(fh, "t:="); read_t(fh, pi);

	fclose(fh);

	// Inicializa variables globales del NSGA-II en base a los datos leídos de la instancia
	gene_length = pi->nPOI + pi->set_M; // Nodos + Delimitadores
	n_routes = pi->set_M;
	n_objectives = 3;  // Score, Categorías, Brecha
	n_constraints = 3; // E_z, Ventanas de Tiempo, T_max

	return 0;
}