/* Conjuntos */
param o; set O := {o}; # nodo inicial (Corresponde al nodo 1)
param s; set S := {s}; # nodo final (Corresponde al nodo N)
set POI;               # conjunto de POIs (Nodos 2 al N-1)

set N := O union POI union S;                    # conjunto de nodos
set A := {i in N, j in N: i != j};               # conjunto de arcos
set M;                                           # conjunto de rutas
set Z;                                           # conjunto de categorias

/* Parametros */
param TM;                # tiempo maximo de las rutas (T_max)
param E{Z};              # cuota maxima por categoria de nodo (E_z)
param e{N,Z};            # valor del nodo para cada categoria (e_iz)
param SCORE{N};          # score por visitar el nodo (S_i)
param OT{N};             # tiempo de apertura del nodo (O_i)
param TT{N};             # tiempo de atencion del nodo (T_i)
param CT{N};             # tiempo de cierre del nodo (C_i)
param t{A};              # tiempo de desplazarse por un arco (t_ij)
param BIG default 10000; # numero muy grande
param cantobj := 3;      # cantidad de objetivos del problema

/* Conjuntos, Parametros y Variables para la Normalizacion */
param cantejc := 11;                   # cantidad de ejecuciones para la frontera de pareto
set objetivos := {1..cantobj};         # conjunto de objetivos del problema
set ejecuciones := {1..cantejc};       # conjunto de ejecuciones para la frontera de pareto
param g default 0;                     # identifica un objetivo en particular
param sigma{ejecuciones,objetivos};    # ponderadores para la frontera de pareto
param betha{objetivos} default 0;      # ponderadores de cada objetivo
param MV{objetivos} default 999999999; # mejor valor alcanzado para cada objetivo
param PV{objetivos} default 0;         # peor valor alcanzado para cada objetivo
var F{objetivos};                      # funciones objetivo del problema

/* Variables */
var x{A,M} binary;      # 1 si el nodo j es visitado despues del nodo i en la ruta m (Eq. 1.2.1)
var y{N,M} binary;      # 1 si el nodo i es visitado en la ruta m (Eq. 1.2.1)
var tt{N,M} >= 0;       # tiempo en el que comienza la visita al nodo i en la ruta m (s_im)
var P{M} integer >= 0;  # puntaje del tour m (P_m)
var P_max integer >= 0; # mayor puntaje encontrado
var P_min integer >= 0; # menor puntaje encontrado

/* Funcion Objetivo General */
minimize F01 : F[g];                                                                 
minimize F02 : sum {i in objetivos} betha[i] * ( MV[i] - F[i] ) / ( MV[i] - PV[i] ); 

/* Restricciones y Ecuaciones del Modelo */
subject to

/* Objetivos (Ecuaciones 1, 2 y 3) */
# Eq 1: Maximizar el puntaje (se minimiza el negativo). Solo considera POIs.
O1 : F[1] = - sum {m in M, i in POI} SCORE[i] * y[i,m];

# Eq 2: Minimizar el tiempo total empleado (tiempos de servicio + tiempos de viaje).
O2 : F[2] = sum {m in M} (sum {i in N: i != s} TT[i] * y[i,m] + sum {(i,j) in A: i != s and j != o} t[i,j] * x[i,j,m]);

# Eq 3: Minimizar la brecha entre la ruta con mayor y menor puntaje.
O3 : F[3] = P_max - P_min;

/* Ecuacion 5: Tours comenzando en nodo inicial y terminando en nodo final */
R1a : sum {m in M, j in N: j != o} x[o,j,m] = card(M);
R1b : sum {m in M, i in N: i != s} x[i,s,m] = card(M);

/* Ecuacion 6: Conservacion de flujo para nodos intermedios */
R2a {k in POI, m in M} : sum {i in N: i != s and i != k} x[i,k,m] = y[k,m];
R2b {k in POI, m in M} : sum {j in N: j != o and j != k} x[k,j,m] = y[k,m];

/* Ecuacion 7: Precedencia temporal (requiere un solver que soporte constraints lógicos, o usar Big-M) */
R3 {(i,j) in A, m in M} : x[i,j,m] = 1 ==> tt[j,m] = tt[i,m] + TT[i] + t[i,j];

/* Ecuacion 8: Asegurarse que un nodo POI es visitado a lo mas una vez en total */
R4 {i in POI} : sum {m in M} y[i,m] <= 1;

/* Ecuacion 9: No superar la cuota maxima por categoria */
R5 {z in Z} : sum {m in M, i in N} e[i,z] * y[i,m] <= E[z];

/* Ecuacion 10: Visitar un POI dentro de su ventana de tiempo */
R6 {i in N, m in M} : OT[i] <= tt[i,m] <= CT[i];

/* Ecuacion 11: Respetar la duracion maxima para cada ruta individual */
R7 {m in M} : sum {i in N: i != s} TT[i] * y[i,m] + sum {(i,j) in A: i != s and j != o} t[i,j] * x[i,j,m] <= TM;

/* Ecuacion 4: Definicion de puntajes para evaluar el balance (O3) */
R8 {m in M} : P[m] = sum {i in POI} SCORE[i] * y[i,m];
R9 {m in M} : P_max >= P[m];
R10 {m in M} : P_min <= P[m];