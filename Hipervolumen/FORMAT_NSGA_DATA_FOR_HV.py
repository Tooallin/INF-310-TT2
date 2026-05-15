import os
import re

def load_solutions(file_list):
    solutions = []
    for file in file_list:
        with open(file, "r") as f:
            for line in f:
                if not line.strip():
                    continue
                parts = re.split(r'\s+', line.strip())
                if len(parts) >= 3:
                    try:
                        objs = tuple(float(x) for x in parts[:3])
                        solutions.append(objs)
                    except ValueError:
                        continue
    return solutions

def is_dominated(a, b):
    """Retorna True si 'a' es dominado por 'b'"""
    return all(bi <= ai for ai, bi in zip(a, b)) and any(bi < ai for ai, bi in zip(a, b))

def filter_non_dominated(solutions):
    """Devuelve solo las soluciones no dominadas del conjunto"""
    non_dominated = []
    for i, sol in enumerate(solutions):
        dominated = False
        for j, other in enumerate(solutions):
            if i != j and is_dominated(sol, other):
                dominated = True
                break
        if not dominated:
            non_dominated.append(sol)
    return non_dominated

def write_output(instance_name, solutions, output_dir):
    # Generar la ruta de salida dentro de la carpeta de formateados
    output_path = os.path.join(output_dir, f"{instance_name}_data")
    with open(output_path, "w") as out:
        out.write("#\n")
        # Mantener el signo en el primer valor (índice 0), aplicar abs() al resto
        unique_rows = set((int(sol[0]), abs(int(sol[1])), abs(int(sol[2]))) for sol in solutions)
        for row in sorted(unique_rows):
            out.write(f"{row[0]} {row[1]} {row[2]}\n")
        out.write("#\n")

def main():
    input_dir = "Datos NSGA-II sin Formatear"
    output_dir = "Datos NSGA-II Formateados"

    # Crear la carpeta de salida si no existe
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Verificar que exista la carpeta de entrada
    if not os.path.exists(input_dir):
        print(f"Error: No se encontró la carpeta '{input_dir}'.")
        return

    # Iterar sobre las carpetas dentro de "Datos NSGA-II sin Formatear"
    for instance_folder in os.listdir(input_dir):
        instance_path = os.path.join(input_dir, instance_folder)

        # Procesar solo si es un directorio
        if os.path.isdir(instance_path):
            # Recopilar todas las rutas de las distintas ejecuciones (semillas)
            # SE AÑADIÓ LA CONDICIÓN 'and "best" in f' AQUÍ
            file_list = [
                os.path.join(instance_path, f) 
                for f in os.listdir(instance_path) 
                if os.path.isfile(os.path.join(instance_path, f)) and "best" in f
            ]

            if not file_list:
                print(f"Advertencia: La carpeta {instance_folder} no contiene archivos con 'best' en el nombre.")
                continue

            # Cargar y filtrar soluciones
            all_solutions = load_solutions(file_list)
            if not all_solutions:
                continue
                
            nd_solutions = filter_non_dominated(all_solutions)

            # Limpiar el nombre de la instancia (quitar "_nsga2" si está presente en el nombre de la carpeta)
            clean_instance_name = instance_folder.replace(".dat", "")

            # Escribir el archivo en la carpeta de destino
            write_output(clean_instance_name, nd_solutions, output_dir)
            print(f"Procesada con éxito: {clean_instance_name} ({len(nd_solutions)} soluciones no dominadas)")

if __name__ == "__main__":
    main()