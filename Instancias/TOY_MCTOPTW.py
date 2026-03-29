import math

def generar_instancia_juguete(dat_filepath="juguete_3FO.dat"):
    nodos = []
    
    # Nodo 0: Depósito (Centro)
    nodos.append({'id': 0, 'x': 50, 'y': 50, 'score': 0})
    
    # FO3: Clúster Cercano y Pobre (Nodos 1 al 10)
    for i in range(1, 11):
        nodos.append({'id': i, 'x': 50 + (i%3), 'y': 50 + (i%2), 'score': 5})
        
    # FO1: Clúster Lejano y Rico (Nodos 11 al 20)
    for i in range(11, 21):
        nodos.append({'id': i, 'x': 90 + (i%3), 'y': 90 + (i%2), 'score': 100})
        
    # FO2: Gemelo Oeste (Nodos 21 al 25)
    for i in range(21, 26):
        nodos.append({'id': i, 'x': 10 + (i%2), 'y': 50 + (i%2), 'score': 50})
        
    # FO2: Gemelo Este (Nodos 26 al 30)
    for i in range(26, 31):
        nodos.append({'id': i, 'x': 90 - (i%2), 'y': 50 - (i%2), 'score': 50})
        
    # Nodo 31: Depósito Final
    nodos.append({'id': 31, 'x': 50, 'y': 50, 'score': 0})

    poi = [str(n['id']) for n in nodos if n['id'] != 0 and n['id'] != 31]
    
    with open(dat_filepath, 'w') as f:
        f.write("param o := 0;\n")
        f.write("param s := 31;\n\n")
        
        f.write(f"set POI := {' '.join(poi)};\n")
        f.write("set M := 1 2;\n")
        f.write("set Z := 1 2 3;\n\n")
        
        # Tiempo restrictivo para no poder visitar todo (150 de tiempo)
        f.write("param TM := 150;\n\n")
        
        # Parámetro E (Relajado para que no estorbe tus nuevos objetivos)
        f.write("param E :=\n1 30\n2 30\n3 30\n;\n\n")
        
        # Parámetro e (Asignación equitativa básica, ya no es tu objetivo principal)
        f.write("param e :=\n")
        for n in nodos:
            cat = (n['id'] % 3) + 1
            e1 = 1 if cat == 1 and n['id'] not in [0,31] else 0
            e2 = 1 if cat == 2 and n['id'] not in [0,31] else 0
            e3 = 1 if cat == 3 and n['id'] not in [0,31] else 0
            f.write(f"{n['id']} 1 {e1} 2 {e2} 3 {e3}\n")
        f.write(";\n\n")
        
        # Scores
        f.write("param SCORE :=\n")
        for n in nodos:
            f.write(f"{n['id']} {n['score']}\n")
        f.write(";\n\n")
        
        # Ventanas de tiempo (Todas abiertas para que no restrinjan)
        for param in ["OT", "CT", "TT"]:
            f.write(f"param {param} :=\n")
            for n in nodos:
                val = 0 if param == "OT" else (150 if param == "CT" else 10)
                if n['id'] in [0, 31] and param == "TT": val = 0
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
        
    print(f"Instancia generada con éxito: {dat_filepath}")

if __name__ == '__main__':
    generar_instancia_juguete()