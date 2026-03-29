/* Rutinas para mostrar la información de la población usando Gnuplot en tiempo real */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <string.h>
# include <unistd.h>

# include "global.h"
# include "rand.h"

/* * Función para graficar la población actual (generación tras generación) sobre la marcha.
 * Recibe un puntero a la población, un canal de comunicación abierto con Gnuplot (FILE *gp)
 * y el número de la generación actual (ii).
 */
void onthefly_display(population *pop, FILE *gp, int ii) {
	int i;
	int flag;
	FILE *fpt;
    
	// 1. Abre (o sobrescribe) un archivo temporal llamado 'plot.out' donde guardará las coordenadas.
	fpt = fopen("plot.out","w");
	flag = 0; // Bandera para saber si encontramos al menos un individuo factible
    
	// 2. Recorre toda la población para extraer los datos a graficar
	for (i = 0; i < popsize; i++) {
        
		// FILTRO DE FACTIBILIDAD: Solo procesa individuos cuyas penalizaciones sean estrictamente 0.
		// Esto significa que respetan las restricciones 9, 10 y 11 de tu modelo del MCTOPTW.
		if (pop->ind[i].constr_violation == 0) {
            
			// Verifica si el usuario pidió un gráfico 2D o 3D.
			// 'choice' probablemente viene de la configuración inicial del programa.
			if (choice != 3)
				// Escribe los valores de los dos primeros objetivos en el archivo de texto.
				fprintf(fpt,"%e\t%e\n", pop->ind[i].obj[obj1-1], pop->ind[i].obj[obj2-1]);
			else
				// Escribe los valores de los TRES objetivos (Ideal para tu modelo MCTOPTW).
				fprintf(fpt,"%e\t%e\t%e\n", pop->ind[i].obj[obj1-1], pop->ind[i].obj[obj2-1], pop->ind[i].obj[obj3-1]);
			
            fflush(fpt); // Fuerza la escritura en disco
			flag = 1; // Marca que sí hay datos para graficar
		}
	}
    
	// 3. Ejecución de comandos de Gnuplot
	if (flag == 0) {
		// Si el algoritmo recién empieza y todos los individuos son infactibles, no grafica nada.
		printf("\n No feasible soln in this pop, hence no display");
	} else {
		// Si hay datos, envía las instrucciones de ploteo al proceso de Gnuplot
		if (choice != 3)
			// 'plot' es el comando de Gnuplot para gráficos 2D. 
			fprintf(gp, "set title 'Generation #%d'\n unset key\n plot 'plot.out' w points pointtype 6 pointsize 1\n", ii);
		else
			// 'splot' es el comando de Gnuplot para gráficos 3D. 
			// Incorpora angle1 y angle2 para definir la perspectiva de la cámara en el espacio tridimensional.
			fprintf(gp, "set title 'Generation #%d'\n set view %d,%d\n unset key\n splot 'plot.out' w points pointtype 6 pointsize 1\n", ii, angle1, angle2);
		
        fflush(gp); // Fuerza la ejecución del comando en Gnuplot
	}
    
	// 4. Cierra el archivo temporal de coordenadas
	fclose(fpt);
	return;
}