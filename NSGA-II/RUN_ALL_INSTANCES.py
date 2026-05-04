import subprocess
import itertools
import time
from pathlib import Path

# Parámetros del algoritmo
random_seeds = [
    0.123, 0.237, 0.351, 0.468, 0.582, 
    0.619, 0.703, 0.745, 0.862, 0.974
]

popsize = 500
ngen = 1000
pcross = 0.6
pmut = 0.1

# === Carga dinámica de instancias ===

# 1. Definir la ruta base de las instancias.
base_dir = Path("../Instancias")

# 2. Lista de las subcarpetas que contienen las instancias
carpetas_instancias = [
    "solomon-100",
]

instance_routes = []

# 3. Recorrer cada carpeta y extraer las rutas de los archivos
for carpeta in carpetas_instancias:
    ruta_carpeta = base_dir / carpeta
    
    if ruta_carpeta.exists() and ruta_carpeta.is_dir():
        for archivo in ruta_carpeta.iterdir():
            if archivo.is_file() and archivo.suffix not in ['.py', '.zip', '.txt']:
                instance_routes.append(str(archivo))
    else:
        print(f"Advertencia: No se encontró la carpeta -> {ruta_carpeta}")

print(f"Se encontraron un total de {len(instance_routes)} instancias para evaluar.\n")


# === Ejecutar combinaciones ===
for instance, seed in itertools.product(instance_routes, random_seeds):
    print(f"\nSolving {instance} with seed {seed}")
    command = [
        "./nsga2r",
        instance,
        str(seed),
        str(popsize),
        str(ngen),
        str(pcross),
        str(pmut)
    ]

    start_time = time.time()
    subprocess.run(command)
    elapsed = time.time() - start_time
    print(f"->Tcpu {elapsed:.2f}")