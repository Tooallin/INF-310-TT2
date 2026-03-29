/* Rutinas para el manejo y consulta de la instancia del problema (MCTOPTW) */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>

# include "global.h"
# include "rand.h"

/* * Función para verificar si un nodo específico es un Punto de Interés (POI).
 * Retorna:
 * 1  -> Si el nodo es un POI (nodo intermedio a visitar).
 * 0  -> Si el nodo es el depósito de inicio (o) o de término (s).
 * -1  -> Si el nodo no existe en la instancia.
 */
int isAPOI(int name, problem_instance pi) {
	int i;
	int count=0; // Variable declarada pero no utilizada realmente en la lógica actual
	
	// Busca en el arreglo de POIs
	for (i = 0; i < pi.nPOI; i++){
		if (name == pi.set_POI[i].name)
			return 1;
		count++;
	}
	
	// Si no es POI, verifica si es el nodo inicial (o) o final (s)
	if (name == pi.param_o.name || name == pi.param_s.name) {
		return 0;
	}
	return -1;
}

/* * Función para obtener el identificador interno (ID) de un nodo dado su nombre.
 * Es crucial porque a veces los nodos no se llaman 0, 1, 2, 3... sino que pueden 
 * tener nombres como 98, 99 o 100 (como en la instancia c101.dat). Esta función 
 * traduce el "nombre" al "índice" que usa el arreglo interno en C.
 */
int getNodeId(int name, problem_instance pi){
	int i;
	
	// Busca el nombre en la lista de POIs y retorna su ID interno
	for (i = 0; i < pi.nPOI; i++){
		if (name == pi.set_POI[i].name)
			return pi.set_POI[i].id;
	}
	
	// Verifica si es el nodo inicial
	if (name == pi.param_o.name) {
		return pi.param_o.id;
	}
	
	// Verifica si es el nodo final
	if (name == pi.param_s.name) {
		return pi.param_s.id;
	}
	return -1;
}

/* * Función de depuración (Debug) para imprimir la instancia completa en consola.
 * El formato de salida está fuertemente inspirado en la sintaxis de AMPL, lo que 
 * facilita la comprobación cruzada de datos entre el algoritmo en C y el solver matemático.
 */
void printProblemInstance(problem_instance *pi){
	int i, j;

	printf("\n");
	printf("Problem Instance!\n\n");
	
	// Imprime los nodos de inicio y fin (o, s)
	printf("param o := %d;\n", pi->param_o.name);
	printf("param s := %d;\n", pi->param_s.name);

	printf("\n");

	// Imprime el conjunto de POIs con sus atributos: 
	// Nombre, Score (S_i), Apertura (OT), Servicio (TT), Cierre (CT) y Categorías (e_iz)
	printf("set POI :=\n");
	for (i = 0; i < pi->nPOI; i++) {
		printf("%d SCORE(%d) OT(%d) TT(%d) CT(%d)", pi->set_POI[i].name, pi->set_POI[i].SCORE, pi->set_POI[i].OT, pi->set_POI[i].TT, pi->set_POI[i].CT);
		for (j = 0; j < pi->set_Z; j++) {
			// Imprime si pertenece (1) o no (0) a cada una de las Z categorías
			printf(" e%d(%d)", j+1, pi->set_POI[i].e[j]);
		}
		printf("\n");
	}
	printf(";\n\n");

	// Imprime el conjunto de M rutas disponibles
	printf("set M :=\n");
	for (i = 1; i <= pi->set_M; i++) {
		printf("%d\n", i);
	}
	printf(";\n\n");

	// Imprime el conjunto de Z categorías disponibles
	printf("set Z :=\n");
	for (i = 1; i <= pi->set_Z; i++) {
		printf("%d\n", i);
	}
	printf(";\n\n");

	// Imprime los límites máximos permitidos (cuotas) para cada categoría E_z
	printf("set E :=\n");
	for (i = 0; i < pi->set_Z; i++) {
		printf("%d\n", pi->param_E[i]);
	}
	printf(";\n\n");

	return;
}