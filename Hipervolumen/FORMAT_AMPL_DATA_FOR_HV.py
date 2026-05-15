import os

def load_solutions_from_log(file_path):
    """
    Lee un archivo .log de AMPL y extrae los valores de F1, F2 y F3 
    de las tablas de resultados, adaptándose a distintos formatos de impresión.
    """
    solutions = []
    in_table = False
    f1_idx, f2_idx, f3_idx = -1, -1, -1
    
    with open(file_path, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            
            upper_line = line.upper()
            
            # Detectar el encabezado de la tabla (inicia con ":" y contiene F1, F2, F3)
            # Ya no exigimos que el ":=" esté en esta misma línea.
            if upper_line.startswith(":") and "F1" in upper_line and "F2" in upper_line and "F3" in upper_line:
                in_table = True
                parts = upper_line.split()
                try:
                    # Encontrar los índices dinámicamente usando la línea en mayúsculas
                    f1_idx = parts.index("F1")
                    f2_idx = parts.index("F2")
                    f3_idx = parts.index("F3")
                except ValueError:
                    # Índices por defecto si algo falla
                    f1_idx, f2_idx, f3_idx = 4, 5, 6
                continue
            
            # Procesar datos dentro de la tabla
            if in_table:
                # El punto y coma marca el final de la tabla. 
                # Usamos 'continue' en vez de 'break' para seguir buscando por si hay más tablas abajo.
                if line == ";":  
                    in_table = False
                    continue 
                
                # Ignoramos la línea que solo contiene ":=" debido al salto de línea de AMPL
                if line == ":=":
                    continue
                
                parts = line.split()
                # Asegurarnos de que la línea tiene suficientes columnas para ser una fila de datos válida
                if len(parts) > max(f1_idx, f2_idx, f3_idx):
                    try:
                        f1 = float(parts[f1_idx])
                        f2 = float(parts[f2_idx])
                        f3 = float(parts[f3_idx])
                        solutions.append((f1, f2, f3))
                    except ValueError:
                        # Si los datos no son convertibles a float (ej. texto colado), los ignoramos
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
    """Escribe los puntos en el formato especificado."""
    # Corrección: Se elimina el .txt para que quede exactamente como pediste
    output_path = os.path.join(output_dir, f"{instance_name}_data")
    
    with open(output_path, "w", encoding="utf-8") as out:
        out.write("#\n")
        # Mantener el signo en el primer valor (índice 0), aplicar abs() al resto
        unique_rows = set((int(sol[0]), abs(int(sol[1])), abs(int(sol[2]))) for sol in solutions)
        for row in sorted(unique_rows):
            out.write(f"{row[0]} {row[1]} {row[2]}\n")
        out.write("#\n")

def main():
    input_dir = "Datos AMPL sin Formatear"
    output_dir = "Datos AMPL Formateados"

    # Crear la carpeta de salida si no existe
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Verificar que exista la carpeta de entrada
    if not os.path.exists(input_dir):
        print(f"Error: No se encontró la carpeta '{input_dir}'.")
        return

    # Iterar sobre todos los archivos .log de la carpeta de entrada
    for filename in os.listdir(input_dir):
        if filename.endswith(".log"):
            file_path = os.path.join(input_dir, filename)
            
            # Cargar todas las soluciones iterando sobre la tabla del log
            all_solutions = load_solutions_from_log(file_path)
            
            if not all_solutions:
                print(f"Advertencia: No se encontraron soluciones en la tabla de {filename}.")
                continue
                
            # Filtrar dejano solo las no dominadas (Frente de Pareto)
            nd_solutions = filter_non_dominated(all_solutions)

            # Limpiar la extensión .log para obtener el nombre puro de la instancia
            clean_instance_name = filename.replace(".log", "")
            
            # Escribir el archivo
            write_output(clean_instance_name, nd_solutions, output_dir)
            
            print(f"Procesado con éxito: {clean_instance_name} ({len(nd_solutions)} soluciones no dominadas)")

if __name__ == "__main__":
    main()