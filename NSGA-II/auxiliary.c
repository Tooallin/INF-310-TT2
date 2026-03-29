/* Funciones auxiliares (no son parte de la lógica evolutiva principal del algoritmo NSGA-II, 
   pero son utilidades matemáticas y de navegación necesarias para manipular el arreglo de genes) */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* Función para retornar el máximo entre dos variables de tipo double.
   Muy útil al momento de evaluar dominancia, aplicar castigos a la factibilidad 
   o calcular distancias de crowding distance. */
double maximum(double a, double b) {
	if (a > b) {
		return(a);
	}
	return (b);
}

/* Función para retornar el mínimo entre dos variables de tipo double. */
double minimum(double a, double b) {
	if (a < b) {
		return (a);
	}
	return (b);
}

/* Función para encontrar los índices de inicio y fin de una ruta específica dentro del cromosoma.
   Basado en tu representación, el gen es un arreglo de números enteros de largo N+M. 
   Los delimitadores están representados por el valor -1 y segmentan el gen. */
void find_route_bounds(individual *ind, int route, int *start_index, int *end_index) {
	int i;
	int current = 0; // Contador de separadores (-1) que hemos cruzado
	int inside_route = 0; // Bandera para saber si ya marcamos el inicio de la ruta actual
	int start = -1, end = -1;

	// Recorremos el cromosoma de principio a fin
	for (i = 0; i < gene_length; i++) {
		// Si encontramos un delimitador (-1), aumentamos el contador de rutas y reiniciamos la bandera
		if (ind->gene[i] == -1) {
			current++; 
			inside_route = 0;
			continue; // Saltamos a la siguiente iteración para no evaluar el -1
		}
		
		// Si el segmento actual coincide con la ruta que buscamos (route es 1-indexed)
		if (current == route - 1) {
			if (!inside_route) {
				start = i; // Marcamos el índice de inicio en el primer nodo de esta ruta
				inside_route = 1;
			}
			end = i; // Actualizamos el índice final iterativamente hasta topar con el siguiente -1
		}
		
		// Optimización: si ya pasamos la ruta que buscábamos, rompemos el ciclo para ahorrar CPU
		if (current > route - 1) break;
	}

	// Devolvemos los resultados a las variables originales a través de los punteros
	*start_index = start;
	*end_index = end;
	return;
}

/* Función para encontrar los índices de inicio y fin del último segmento del cromosoma.
   Según tu modelo, el segmento final M+1 contiene los POIs no visitados por ninguna de las rutas. */
void find_last_route_bounds(individual *ind, int *start_index, int *end_index) {
	int i;
	int last_sep = -1;

	// Recorremos todo el gen buscando en qué índice exacto está posicionado el último separador (-1)
	for (i = 0; i < gene_length; i++) {
		if (ind->gene[i] == -1) {
			last_sep = i;
		}
	}

	// El inicio de los POIs no visitados es la posición inmediatamente posterior al último separador (-1)
	*start_index = last_sep + 1;
	
	// El final de los POIs no visitados es simplemente la última posición del arreglo
	*end_index = gene_length - 1;
	return;
}