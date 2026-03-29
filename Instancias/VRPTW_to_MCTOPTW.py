import os
import math
import random
from pathlib import Path

def convert_vrptw_to_dat(txt_filepath, dat_filepath):
    nodes = []
    reading_customers = False
    
    vehicle_number = 1
    vehicle_capacity = 1
    
    # Fijamos una semilla para que el resultado aleatorio sea reproducible
    random.seed(42)
    
    # 1. Leer el archivo .TXT
    with open(txt_filepath, 'r') as file:
        lines = file.readlines()
        
    for i, line in enumerate(lines):
        parts = line.strip().split()
        
        if not parts:
            continue
            
        if len(parts) >= 2 and parts[0] == 'NUMBER' and parts[1] == 'CAPACITY':
            for j in range(i+1, len(lines)):
                val_parts = lines[j].strip().split()
                if val_parts:
                    vehicle_number = int(val_parts[0])
                    vehicle_capacity = int(val_parts[1])
                    break
            continue
            
        if parts[0] == 'CUST' or (parts[0] == 'CUST' and parts[1] == 'NO.'):
            reading_customers = True
            continue
            
        if reading_customers and parts[0].isdigit():
            node_id = int(parts[0])
            x = float(parts[1])
            y = float(parts[2])
            demand = float(parts[3])       
            ready_time = float(parts[4])   
            due_date = float(parts[5])     
            service_time = float(parts[6]) 
            
            nodes.append({
                'id': node_id,
                'x': x,
                'y': y,
                'score': demand,
                'ot': ready_time,
                'ct': due_date,
                'tt': service_time
            })

    if not nodes:
        return False

    # 2. Manejo del nodo inicial y final
    depot = nodes[0]
    s_id = nodes[-1]['id'] + 1
    s_node = depot.copy()
    s_node['id'] = s_id
    nodes.append(s_node)

    # 3. Extraer variables y sets
    poi = [str(n['id']) for n in nodes if n['id'] != 0 and n['id'] != s_id]
    num_clientes = len(poi)
    tm = int(s_node['ct'])

    # Nueva lógica de rutas: 2 rutas hasta 200 POIs, sumando 1 ruta extra por cada 200 POIs adicionales
    m_max = max(2, math.ceil(num_clientes / 200) + 1)
        
    z_max = math.ceil(num_clientes / vehicle_capacity) + 2 
    
    # Aseguramos que no haya más categorías que clientes (para garantizar al menos 1 por categoría)
    z_max = min(z_max, num_clientes)
    
    # Las variables set_m y set_z ahora solo se definen una vez aquí.
    set_m = [str(i) for i in range(1, m_max + 1)]
    set_z = [str(i) for i in range(1, z_max + 1)]

    # 4. Asignación Aleatoria de Categorías (Controlando rareza y vacíos)
    # Iniciamos todas las categorías con al menos 1 POI
    category_counts = {z: 1 for z in range(1, z_max + 1)}
    
    if num_clientes > z_max:
        # Usamos pesos exponenciales (0.5, 0.25, 0.125...) para hacerlas cada vez más raras
        weights = [0.5**z for z in range(z_max)]
        total_weight = sum(weights)
        probs = [w / total_weight for w in weights]
        
        remaining_pois = num_clientes - z_max
        
        # Repartimos los POIs restantes proporcionalmente
        for z in range(1, z_max + 1):
            add = int((num_clientes - z_max) * probs[z-1])
            category_counts[z] += add
            remaining_pois -= add
            
        # Si sobra algún POI por los redondeos (int), se lo damos a la categoría 1 (la más común)
        category_counts[1] += remaining_pois
        
    # Creamos la lista exacta de asignaciones y la mezclamos
    assignment_list = []
    for z, count in category_counts.items():
        assignment_list.extend([z] * count)
        
    random.shuffle(assignment_list)
    
    # Asignamos la lista ya mezclada a los POIs
    node_categories = {}
    for i, n_id in enumerate(poi):
        node_categories[n_id] = assignment_list[i]

    # 5. Escribir el archivo .DAT
    with open(dat_filepath, 'w') as file:
        file.write(f"param o := 0;\n")
        file.write(f"param s := {s_id};\n\n")
        
        file.write(f"set POI := {' '.join(poi)};\n")
        file.write(f"set M := {' '.join(set_m)};\n")
        file.write(f"set Z := {' '.join(set_z)};\n\n")
        
        file.write(f"param TM := {tm};\n\n")
        
        # Parámetro E (Relajación según rareza)
        file.write("param E :=\n")
        for z in range(1, z_max + 1):
            # Relajación lineal: rho va de 0.5 (mitad permitida) a 1.0 (totalmente relajado)
            if z_max > 1:
                rho = 0.5 + 0.5 * ((z - 1) / (z_max - 1))
            else:
                rho = 1.0
                
            # La cuota es el porcentaje rho del total de POIs que existen en esa categoría
            e_val = math.ceil(category_counts[z] * rho)
            file.write(f"{z} {e_val}\n")
        file.write(";\n\n")
        
        # Parámetro e
        file.write("param e :=\n")
        for n in nodes:
            line = []
            for z in range(1, z_max + 1):
                belongs = 1 if (str(n['id']) in poi and node_categories.get(str(n['id'])) == z) else 0
                line.append(f"{n['id']} {z} {belongs}")
            file.write(" ".join(line) + "\n")
        file.write(";\n\n")
        
        file.write("param SCORE :=\n")
        for n in nodes:
            file.write(f"{n['id']} {int(n['score'])}\n")
        file.write(";\n\n")
        
        file.write("param OT :=\n")
        for n in nodes:
            file.write(f"{n['id']} {int(n['ot'])}\n")
        file.write(";\n\n")
        
        file.write("param TT :=\n")
        for n in nodes:
            file.write(f"{n['id']} {int(n['tt'])}\n")
        file.write(";\n\n")
        
        file.write("param CT :=\n")
        for n in nodes:
            file.write(f"{n['id']} {int(n['ct'])}\n")
        file.write(";\n\n")
        
        file.write("param t :=\n")
        for n1 in nodes:
            for n2 in nodes:
                if n1['id'] != n2['id']:
                    dist = math.sqrt((n2['x'] - n1['x'])**2 + (n2['y'] - n1['y'])**2)
                    dist_rounded = round(dist, 2)
                    file.write(f"{n1['id']} {n2['id']} {dist_rounded}\n")
        file.write(";\n")
        
    return True

def process_all_folders(base_directory):
    base_path = Path(base_directory)
    
    if not base_path.is_dir():
        print(f"Error: La carpeta '{base_directory}' no existe.")
        return

    print(f"Buscando archivos .txt en: {base_path.absolute()}")
    procesados = 0
    
    for txt_file in base_path.rglob('*.txt'):
        dat_file = txt_file.with_suffix('.dat')
        
        try:
            exito = convert_vrptw_to_dat(str(txt_file), str(dat_file))
            if exito:
                print(f"  [OK] Convertido: {txt_file.name} -> {dat_file.name}")
                procesados += 1
            else:
                print(f"  [OMITIDO] {txt_file.name}: No tiene el formato de nodos esperado.")
        except Exception as e:
            print(f"  [ERROR] Fallo al procesar {txt_file.name}: {e}")
            
    print(f"\nProceso finalizado. Total de archivos convertidos: {procesados}")

if __name__ == '__main__':
    CARPETA_BASE = '.' 
    process_all_folders(CARPETA_BASE)