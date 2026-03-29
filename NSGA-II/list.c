/* Implementación de una Lista Doblemente Enlazada personalizada para el manejo dinámico de frentes */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* * Función para insertar un elemento 'x' (el índice de un individuo) en la lista.
 * La inserción se realiza inmediatamente DESPUÉS del nodo especificado por 'node'.
 */
void insert(list *node, int x) {
	list *temp;
	
	// Validación de seguridad: no se puede insertar después de un nodo inexistente
	if (node == NULL) {
		printf("\n Error!! asked to enter after a NULL pointer, hence exiting \n");
		exit(1);
	}
	
	// 1. Reserva memoria para el nuevo eslabón de la lista
	temp = (list *)malloc(sizeof(list));
	
	// 2. Asigna el valor (en NSGA-II, 'x' suele ser el índice del individuo en el arreglo de la población)
	temp->index = x;
	
	// 3. Reconfigura los punteros para insertar el nuevo nodo en la cadena
	temp->child = node->child; // El hijo del nuevo nodo es el antiguo hijo del nodo de referencia
	temp->parent = node;       // El padre del nuevo nodo es el nodo de referencia
	
	// 4. Si el nodo de referencia ya tenía un hijo, actualiza al padre de ese hijo
	if (node->child != NULL) {
		node->child->parent = temp;
	}
	
	// 5. Finalmente, conecta el nodo de referencia con el nuevo nodo
	node->child = temp;
	return;
}

/* * Función para eliminar un nodo específico ('node') de la lista.
 * Reconecta el eslabón anterior con el siguiente para no romper la cadena, 
 * libera la memoria del nodo eliminado y retorna un puntero al nodo anterior (parent).
 */
list* del(list *node) {
	list *temp;
	
	// Validación de seguridad
	if (node == NULL) {
		printf("\n Error!! asked to delete a NULL pointer, hence exiting \n");
		exit(1);
	}
	
	// 1. Guarda la referencia al nodo padre (el eslabón anterior)
	temp = node->parent;
	
	// 2. Conecta al padre directamente con el hijo del nodo que se va a eliminar, saltándolo
	temp->child = node->child;
	
	// 3. Si el nodo a eliminar tenía un hijo, actualiza su puntero 'parent' hacia el abuelo
	if (temp->child != NULL) {
		temp->child->parent = temp;
	}
	
	// 4. Libera la memoria que ocupaba el nodo en la RAM
	free (node);
	
	// 5. Retorna el nodo padre. Esto es muy útil en los bucles (como en fillnds.c) 
	// para no perder la posición en la lista al iterar y borrar elementos simultáneamente.
	return (temp);
}