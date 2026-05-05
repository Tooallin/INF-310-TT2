import math

def generar_instancia_juguete(dat_filepath="toy-instances/toy.dat"):
    nodos = []
    
    # Nodo 0: Depósito Inicial (Centro)
    nodos.append({'id': 0, 'x': 50, 'y': 50, 'score': 0})
    
    # CLÚSTER 1: "El Dorado" (Nodos 1 al 5)
    # Lejos (Alta distancia), Rico (Alto Score). Consumirá todo el tiempo de 1 vehículo.
    for i in range(1, 6):
        nodos.append({'id': i, 'x': 85 + (i%2), 'y': 85 + (i%2), 'score': 100})
        
    # CLÚSTER 2: "La Vecindad" (Nodos 6 al 10)
    # Pegados al depósito (Mínima distancia), Pobres (Bajo Score).
    for i in range(6, 11):
        nodos.append({'id': i, 'x': 51 + (i%2), 'y': 51 + (i%2), 'score': 5})
        
    # CLÚSTER 3: "Gemelo Este" (Nodos 11 al 13)
    # Media distancia, Score moderado. Para balancear con el Oeste.
    for i in range(11, 14):
        nodos.append({'id': i, 'x': 75, 'y': 50 + (i%2), 'score': 50})
        
    # CLÚSTER 4: "Gemelo Oeste" (Nodos 14 al 16)
    # Media distancia, Score moderado. Para balancear con el Este.
    for i in range(14, 17):
        nodos.append({'id': i, 'x': 25, 'y': 50 - (i%2), 'score': 50})
        
    # Nodo 17: Depósito Final
    nodos.append({'id': 17, 'x': 50, 'y': 50, 'score': 0})

    poi = [str(n['id']) for n in nodos if n['id'] not in [0, 17]]
    
    with open(dat_filepath, 'w') as f:
        f.write("param o := 0;\n")
        f.write("param s := 17;\n\n")
        
        f.write(f"set POI := {' '.join(poi)};\n")
        f.write("set M := 1 2;\n") # 2 vehículos
        f.write("set Z := 1 2 3;\n\n")
        
        # Tiempo restrictivo (150). 
        # Ir a "El Dorado" cuesta ~49 de ida y ~49 de vuelta. Tiempo en ruta = 98.
        # Visitar sus 5 nodos cuesta 5 * 10 = 50. Total = 148. (Cabe exacto en 1 vehículo).
        f.write("param TM := 150;\n\n")
        
        # Parámetro E (Relajado)
        f.write("param E :=\n1 30\n2 30\n3 30\n;\n\n")
        
        # Parámetro e (Asignación equitativa básica para que el modelo no falle)
        f.write("param e : 1 2 3 :=\n")
        for n in nodos:
            cat = (n['id'] % 3) + 1
            e1 = 1 if cat == 1 and n['id'] not in [0,17] else 0
            e2 = 1 if cat == 2 and n['id'] not in [0,17] else 0
            e3 = 1 if cat == 3 and n['id'] not in [0,17] else 0
            f.write(f"{n['id']}   {e1} {e2} {e3}\n")
        f.write(";\n\n")
        
        # Scores
        f.write("param SCORE :=\n")
        for n in nodos:
            f.write(f"{n['id']} {n['score']}\n")
        f.write(";\n\n")
        
        # Ventanas de tiempo (Todas abiertas, tiempo de servicio 10)
        for param in ["OT", "CT", "TT"]:
            f.write(f"param {param} :=\n")
            for n in nodos:
                val = 0 if param == "OT" else (150 if param == "CT" else 10)
                if n['id'] in [0, 17] and param == "TT": val = 0
                f.write(f"{n['id']} {val}\n")
            f.write(";\n\n")
            
        # Matriz de Distancias (Cálculo exacto)
        f.write("param t :=\n")
        for n1 in nodos:
            for n2 in nodos:
                if n1['id'] != n2['id']:
                    dist = math.sqrt((n2['x'] - n1['x'])**2 + (n2['y'] - n1['y'])**2)
                    f.write(f"{n1['id']} {n2['id']} {round(dist, 2)}\n")
        f.write(";\n")
        
        # Parámetro sigma: Generación dinámica usando Método Das-Dennis
        H = 5 # Particiones (H=5 genera 21 ejecuciones para 3 objetivos)
        ejecuciones = []
        
        # Generar las combinaciones donde w1 + w2 + w3 = H
        for w1 in range(H + 1):
            for w2 in range(H + 1 - w1):
                w3 = H - w1 - w2
                # Normalizar dividiendo por H para que sumen 1.0
                ejecuciones.append((w1/H, w2/H, w3/H))
        
        # Escribir a archivo
        f.write("param cantejc := " + str(len(ejecuciones)) + ";\n")
        f.write("param sigma : 1 2 3 :=\n")
        for idx, (s1, s2, s3) in enumerate(ejecuciones, start=1):
            # Imprimir con 4 decimales para mayor precisión
            f.write(f"{idx}   {s1:.4f} {s2:.4f} {s3:.4f}\n")
        f.write(";\n\n")
    print(f"Instancia generada con éxito: {dat_filepath}")

if __name__ == '__main__':
    generar_instancia_juguete()