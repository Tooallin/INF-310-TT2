# INF-310-TT2
Generación de rutas turísticas para el problema de orientación en equipo con múltiples restricciones y ventanas de tiempo: Enfoque Multiobjetivo basado en NSGA-II

## Comandos a mano
 * ``nohup bash ToDoParamILS.sh > OUT &`` desde la carpeta ``/INF-310-TT2/ParamILS`` para sintonizar NSGA-II.
 * ``./RUN_ALL_AMPL.sh`` desde la carpeta ``/INF-310-TT2/AMPL`` para ejecutar todas las instancias en AMPL.
 * ``./RUN_ALL_NSGA2R_VX.sh`` desde la carpeta ``/INF-310-TT2/NSGA-II`` para ejecutar todas las instancias en NSGA-II (Sin sintonización).
 * ``./nsga2r "../Instancias/toy-instances/toy.dat" 0.123 100 300000 0.8 0.1`` para ejecutar la instancia de juguete.
 * ``./nsga2r "../Instancias/solomon-100/xxx.dat" 0.123 100 300000 0.8 0.1`` para ejecutar una instancia del conjunto de instancias de 100 POI.
 * ``./nsga2r "../Instancias/homberger_200_customer_instances/xxx.dat" 0.123 100 300000 0.8 0.1`` para ejecutar una instancia del conjunto de instancias de 200 POI.
 * ``./nsga2r "../Instancias/homberger_400_customer_instances/xxx.dat" 0.123 100 300000 0.8 0.1`` para ejecutar una instancia del conjunto de instancias de 400 POI.
 * ``./nsga2r "../Instancias/homberger_600_customer_instances/xxx.dat" 0.123 100 300000 0.8 0.1`` para ejecutar una instancia del conjunto de instancias de 600 POI.
 * ``./nsga2r "../Instancias/homberger_800_customer_instances/xxx.dat" 0.123 100 300000 0.8 0.1`` para ejecutar una instancia del conjunto de instancias de 800 POI.
 * ``./nsga2r "../Instancias/homberger_1000_customer_instances/xxx.dat" 0.123 100 300000 0.8 0.1`` para ejecutar una instancia del conjunto de instancias de 1000 POI.

## NSGA-II V0:
La versión 0 del algoritmo no realiza ningun cambio en su funcionamiento más allá de adecuar el algoritmo al problema del MCTOPTW, introduciendo tal cual como son mencionados los operadores actualmente. Esto incluye:
 * **Cruzamientos:** RBX.
 * **Mutaciones:** Insert, Remove, ARS y ERS.

## NSGA-II V1 Fix de Rutas Vacias
La versión 1.0 modifica los operadores ya incluidos para que no puedan dejar rutas vacias. Este cambio aplica unicamente a los operadores:
 * **Cruzamientos:** RBX.
 * **Mutaciones:** Remove.

## NSGA-II V2 Operadores Greedy
La versión 2.0 añade dos nuevos operadores de cruzamiento y 3 nuevos operadores de mutación. Los nuevos operadores consisten en:
 * **Cruzamiento Sub-tour Exchange:** Selecciona aleatoriamente un bloque de solo 2 a 4 POIs consecutivos del Padre 1 que formen una secuencia temporalmente muy eficiente. Hereda ese bloque al hijo y usa al Padre 2 para completar los extremos de la ruta.
 * **Cruzamiento Elitist Route Crossover:** Selecciona específicamente una ruta elitista (el 50% de las veces la que posea mayor Score total y el otro 50% la ruta más eficiente). Esto garantiza que los mejores bloques constructivos se protejan y se transmitan intactos a la siguiente generación.
 * **Mutación Greedy Insert:** Evalúa un subconjunto de nodos no visitados y selecciona aquel que ofrezca la mejor relación costo-beneficio. Una vez seleccionado el mejor nodo, busca insertarlo en la posición exacta de la ruta que genere el menor impacto en el tiempo total, maximizando el uso eficiente de las ventanas de tiempo.
 * **Mutación Replace:**  Selecciona un POI que ya se encuentra dentro de una ruta activa pero que aporta un puntaje muy bajo, y lo intercambia directamente por un POI del segmento de "no visitados" que posea un puntaje mayor. Este operador es clave para rutas que ya han alcanzado su límite de tiempo máximo, permitiendo mejorar el puntaje sin necesidad de vaciar la ruta primero.
 * **Mutación Min-Team Mutation:** Analiza el individuo completo y detecta específicamente cuál de sus rutas tiene el menor puntaje. En lugar de mutar todo el individuo al azar, concentra todo el esfuerzo de búsqueda local únicamente en esa ruta deficiente. Esto ayuda a levantar el puntaje total y, al mismo tiempo, contribuye directamente a tu tercer objetivo de balanceo de cargas.

Adicionalmente, incorpora un balanceo en los operadores definiendo distintas probabilidades de aplicarlos dependiendo de si son un operador **greedy** o **random**. Las probabilidades son las siguientes:
 * **Operadores de Cruzamiento:** 40% de probabilidades de aplicar **Elitist Route Crossover**, 40% de probabilidades de aplicar **Sub-tour Exchange** y 20% de probabilidades de aplicar **Router Based Crossover**.
 * **Operadores de Mutación:** 70% de probabilidades de aplicar un operador **Greedy (Greedy Insert, Replace, Min-Team)** y un 30% de probabilidades de aplicar un operador **Random (ARS, ERS, Insert, Remove)**.