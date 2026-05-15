import subprocess
import os

def obtener_peores_valores_global(archivos):
    """
    Recibe una lista de rutas de archivos. 
    Busca en todos ellos y devuelve el peor (mayor) valor global para cada objetivo.
    """
    peor_obj0 = float('-inf')
    peor_obj1 = float('-inf')
    peor_obj2 = float('-inf')

    datos_encontrados = False

    for archivo in archivos:
        if not os.path.exists(archivo):
            continue

        with open(archivo, "r") as f:
            for linea in f:
                linea = linea.strip()
                # Ignorar líneas vacías o separadores (como '#')
                if not linea or linea.startswith("#"):
                    continue

                partes = linea.split()
                if len(partes) >= 3:
                    try:
                        v0 = float(partes[0])
                        v1 = float(partes[1])
                        v2 = float(partes[2])
                        
                        # El peor valor global siempre es el mayor encontrado en CUALQUIER archivo
                        if v0 > peor_obj0: peor_obj0 = v0
                        if v1 > peor_obj1: peor_obj1 = v1
                        if v2 > peor_obj2: peor_obj2 = v2
                        
                        datos_encontrados = True
                    except ValueError:
                        continue

    if datos_encontrados:
        return peor_obj0, peor_obj1, peor_obj2
    return None

def main():
    # Directorios donde se encuentran los datos
    dir_nsga2 = "../Datos NSGA-II Formateados"
    dir_ampl = "../Datos AMPL Formateados"
    comandos = []

    # Verificar que existan las carpetas
    if not os.path.exists(dir_nsga2) or not os.path.exists(dir_ampl):
        print("Error: Asegúrate de que ambas carpetas existan.")
        return

    # Obtener los nombres de archivos válidos en ambas carpetas
    archivos_nsga2 = set(f for f in os.listdir(dir_nsga2) if os.path.isfile(os.path.join(dir_nsga2, f)))
    archivos_ampl = set(f for f in os.listdir(dir_ampl) if os.path.isfile(os.path.join(dir_ampl, f)))

    # Unimos los nombres para asegurarnos de recorrer todas las instancias
    todas_las_instancias = archivos_nsga2.union(archivos_ampl)

    for nombre_archivo in todas_las_instancias:
        archivo_nsga2 = os.path.join(dir_nsga2, nombre_archivo)
        archivo_ampl = os.path.join(dir_ampl, nombre_archivo)

        # Agrupamos los archivos que existen para esta instancia
        archivos_a_leer = []
        if os.path.exists(archivo_nsga2): 
            archivos_a_leer.append(archivo_nsga2)
        if os.path.exists(archivo_ampl): 
            archivos_a_leer.append(archivo_ampl)

        # Calculamos el peor punto tomando en cuenta ambos archivos
        peores_valores = obtener_peores_valores_global(archivos_a_leer)

        if peores_valores:
            # Formatear el punto de referencia
            ref = f"{peores_valores[0]} {peores_valores[1]} {peores_valores[2]}"
            print(f"[{nombre_archivo}] Punto de referencia global calculado: {ref}")

            # Añadir comando para NSGA-II usando la referencia global
            if os.path.exists(archivo_nsga2):
                comandos.append(f'./hv -r "{ref}" "{archivo_nsga2}"')
            
            # Añadir comando para AMPL usando la MISMA referencia global
            if os.path.exists(archivo_ampl):
                comandos.append(f'./hv -r "{ref}" "{archivo_ampl}"')
        else:
            print(f"Advertencia: No se encontraron datos válidos para la instancia {nombre_archivo}")

    # Ejecutar todos los comandos generados
    print(f"\nSe encontraron {len(comandos)} comandos listos para ejecutar.\n")
    for cmd in comandos:
        print(f"Ejecutando: {cmd}")
        subprocess.run(cmd, shell=True)

if __name__ == "__main__":
    main()