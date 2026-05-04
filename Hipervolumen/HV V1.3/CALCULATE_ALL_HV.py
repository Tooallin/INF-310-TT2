import subprocess
import os

def obtener_peores_valores(archivo):
    peor_obj0 = float('-inf')
    peor_obj1 = float('-inf')
    peor_obj2 = float('-inf')

    datos_encontrados = False

    if not os.path.exists(archivo):
        return None

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
                    
                    # El peor valor siempre es el mayor
                    if v0 > peor_obj0: 
                        peor_obj0 = v0
                    if v1 > peor_obj1: 
                        peor_obj1 = v1
                    if v2 > peor_obj2: 
                        peor_obj2 = v2
                        
                    datos_encontrados = True
                except ValueError:
                    continue

    if datos_encontrados:
        return peor_obj0, peor_obj1, peor_obj2
    return None

def main():
    # Directorio único donde se encuentran los datos
    dir_nsga2 = "../Datos NSGA-II Formateados"
    comandos = []

    # Verificar que exista la carpeta
    if not os.path.exists(dir_nsga2):
        print(f"Error: No se encontró la carpeta '{dir_nsga2}'.")
        return

    # Iterar dinámicamente sobre todos los archivos dentro de la carpeta
    for nombre_archivo in os.listdir(dir_nsga2):
        archivo_nsga2 = os.path.join(dir_nsga2, nombre_archivo)

        # Asegurarnos de que estamos leyendo un archivo y no una subcarpeta
        if not os.path.isfile(archivo_nsga2):
            continue

        peores_valores = obtener_peores_valores(archivo_nsga2)

        if peores_valores:
            # Formatear el punto de referencia
            ref = f"{peores_valores[0]} {peores_valores[1]} {peores_valores[2]}"
            print(f"[{nombre_archivo}] Punto de referencia calculado: {ref}")

            # Añadir comando para NSGA-II
            comandos.append(f'./hv -r "{ref}" "{archivo_nsga2}"')
        else:
            print(f"Advertencia: No se encontraron datos válidos para el archivo {nombre_archivo}")

    # Ejecutar todos los comandos generados
    print(f"\nSe encontraron {len(comandos)} comandos listos para ejecutar.\n")
    for cmd in comandos:
        print(f"Ejecutando: {cmd}")
        subprocess.run(cmd, shell=True)

if __name__ == "__main__":
    main()