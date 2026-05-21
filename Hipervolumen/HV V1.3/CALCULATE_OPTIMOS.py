import subprocess
import os

def obtener_peores_valores_global(archivos):
    """
    Recibe una lista de rutas de archivos. 
    Busca en todos ellos y devuelve el peor (mayor) valor global para cada objetivo,
    ajustado (x0.9 si es negativo, x1.1 si es positivo o cero).
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
                        
                        # El peor valor global siempre es el mayor encontrado en AMBOS archivos
                        if v0 > peor_obj0: peor_obj0 = v0
                        if v1 > peor_obj1: peor_obj1 = v1
                        if v2 > peor_obj2: peor_obj2 = v2
                        
                        datos_encontrados = True
                    except ValueError:
                        continue

    if datos_encontrados:
        # Ajustar los puntos de referencia según su signo
        peor_obj0 = peor_obj0 * 0.9 if peor_obj0 < 0 else peor_obj0 * 1.1
        peor_obj1 = peor_obj1 * 0.9 if peor_obj1 < 0 else peor_obj1 * 1.1
        peor_obj2 = peor_obj2 * 0.9 if peor_obj2 < 0 else peor_obj2 * 1.1
        
        return peor_obj0, peor_obj1, peor_obj2
    
    return None


def calcular_hv_con_referencia(archivo, ref0, ref1, ref2):
    """
    Ejecuta el comando ./hv para un archivo dado usando puntos de referencia predefinidos.
    Retorna el hipervolumen o None si hay error.
    """
    if not os.path.exists(archivo):
        return None
        
    ref_str = f"{ref0} {ref1} {ref2}"
    cmd = f'./hv -r "{ref_str}" "{archivo}"'
    
    resultado = subprocess.run(cmd, shell=True, capture_output=True, text=True)
    
    if resultado.returncode == 0:
        return int(round(float(resultado.stdout.strip())))
    else:
        print(f"  [Error] al ejecutar ./hv para {os.path.basename(archivo)}: {resultado.stderr.strip()}")
        return None


def main():
    # Directorios donde se encuentran los datos
    dir_ampl = "../Datos AMPL Formateados"
    dir_nsga2 = "../Datos NSGA-II Formateados"

    # Verificar que exista la carpeta principal
    if not os.path.exists(dir_ampl):
        print(f"Error: Asegúrate de que la carpeta '{dir_ampl}' exista.")
        return

    # Obtener los nombres de archivos válidos en la carpeta de AMPL
    archivos_ampl = set(f for f in os.listdir(dir_ampl) if os.path.isfile(os.path.join(dir_ampl, f)))

    print(f"Se encontraron {len(archivos_ampl)} archivos para procesar.\n")

    with open("optimos.txt", "w") as f_out, open("All.inst", "w") as f_inst:
        for nombre_archivo in sorted(archivos_ampl):
            archivo_ampl = os.path.join(dir_ampl, nombre_archivo)
            archivo_nsga2 = os.path.join(dir_nsga2, nombre_archivo)
            
            nombre_final = nombre_archivo.replace("_data", ".dat")

            # 1. Agrupar los archivos que realmente existen para esta instancia
            archivos_instancia = []
            if os.path.exists(archivo_ampl):
                archivos_instancia.append(archivo_ampl)
            if os.path.exists(archivo_nsga2):
                archivos_instancia.append(archivo_nsga2)

            # 2. Calcular los peores valores GLOBALES (compartidos por AMPL y NSGA-II)
            peores_valores = obtener_peores_valores_global(archivos_instancia)

            if not peores_valores:
                print(f"Advertencia: No se encontraron datos válidos en AMPL ni NSGA-II para la instancia {nombre_archivo}. Omitiendo.\n")
                continue

            ref0, ref1, ref2 = peores_valores

            # 3. Calcular HV para ambas instancias usando EL MISMO punto de referencia
            hv_ampl = calcular_hv_con_referencia(archivo_ampl, ref0, ref1, ref2)
            hv_nsga2 = calcular_hv_con_referencia(archivo_nsga2, ref0, ref1, ref2)

            # 4. Lógica de comparación y selección
            if hv_ampl is None:
                # Si AMPL falló pero NSGA-II existe
                seleccion = "NSGA-II (Fallback por falta de datos AMPL)"
                hv_final = hv_nsga2
            else:
                # Si AMPL existe, es la selección base
                seleccion = "AMPL"
                hv_final = hv_ampl

                # Comparamos si NSGA-II es excepcionalmente mejor
                if hv_nsga2 is not None:
                    if hv_nsga2 >= (3 * hv_ampl):
                        seleccion = "NSGA-II"
                        hv_final = hv_nsga2

            # Imprimir la comparación por consola
            str_hv_ampl = str(hv_ampl) if hv_ampl is not None else "No encontrado"
            str_hv_nsga2 = str(hv_nsga2) if hv_nsga2 is not None else "No encontrado"
            
            print(f"Instancia: {nombre_final}")
            print(f"  - Ref Point: {ref0:.2f}, {ref1:.2f}, {ref2:.2f}")
            print(f"  - HV AMPL:   {str_hv_ampl}")
            print(f"  - HV NSGA-II: {str_hv_nsga2}")
            print(f"  -> Utilizando: {seleccion}\n")
            print("-" * 40)

            # Escribir los resultados en los archivos de texto
            f_out.write(f"{nombre_final} {hv_final} {int(round(ref0))} {int(round(ref1))} {int(round(ref2))}\n")
            f_inst.write(f"{nombre_final}\n")

    print("\n¡Proceso terminado exitosamente! Revisa los archivos 'optimos.txt' y 'All.inst'.")

if __name__ == "__main__":
    main()