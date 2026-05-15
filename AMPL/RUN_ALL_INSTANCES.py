import os
import subprocess
from pathlib import Path

# Configuración de directorios y archivos
ruta_ampl = "/home/alessandro.cintolesi/ampl.linux-intel64/ampl"
carpeta_instancias = Path("Instancias")
archivo_template = "MCTOPTW.run"
archivo_ejecucion = "MCTOPTW_temp.run"
carpeta_resultados = Path("Resultados")

# Crear carpeta de resultados si no existe
carpeta_resultados.mkdir(exist_ok=True)

# Leer la plantilla de AMPL
with open(archivo_template, "r", encoding="utf-8") as f:
    template_contenido = f.read()

# Iterar sobre todos los archivos .dat en la carpeta
for archivo_dat in carpeta_instancias.glob("*.dat"):
    # Definir el archivo de salida (.log)
    archivo_log = carpeta_resultados / f"{archivo_dat.stem}.log"

    # Comprobar si el archivo .log ya existe en la carpeta Resultados
    if archivo_log.exists():
        print(f"Saltando: {archivo_dat.name} (Ya existe su archivo .log)")
        continue
    
    print(f"Iniciando ejecución para: {archivo_dat.name}")
    
    # Reemplazar el comodín con la ruta real de la instancia
    ruta_instancia = archivo_dat.as_posix()
    nuevo_run = template_contenido.replace("data __INSTANCIA__;", f"data {ruta_instancia};")
    
    # Guardar el archivo .run temporal
    with open(archivo_ejecucion, "w", encoding="utf-8") as f:
        f.write(nuevo_run)
    
    # Definir el archivo de salida (.log)
    archivo_log = carpeta_resultados / f"{archivo_dat.stem}.log"
    
    # Ejecutar AMPL llamando al sistema
    with open(archivo_log, "w", encoding="utf-8") as log_file:
        subprocess.run([ruta_ampl, archivo_ejecucion], stdout=log_file, stderr=subprocess.STDOUT)

print("\n¡Ejecución por lotes finalizada!")

# Limpiar archivo temporal
if os.path.exists(archivo_ejecucion):
    os.remove(archivo_ejecucion)