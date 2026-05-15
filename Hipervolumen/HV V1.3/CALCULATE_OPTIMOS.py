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
						
						# El peor valor global siempre es el mayor encontrado
						if v0 > peor_obj0: peor_obj0 = v0
						if v1 > peor_obj1: peor_obj1 = v1
						if v2 > peor_obj2: peor_obj2 = v2
						
						datos_encontrados = True
					except ValueError:
						continue

	if datos_encontrados:
		# Actualización: Ajustar los puntos de referencia según su signo
		peor_obj0 = peor_obj0 * 0.9 if peor_obj0 < 0 else peor_obj0 * 1.1
		peor_obj1 = peor_obj1 * 0.9 if peor_obj1 < 0 else peor_obj1 * 1.1
		peor_obj2 = peor_obj2 * 0.9 if peor_obj2 < 0 else peor_obj2 * 1.1
		
		return peor_obj0, peor_obj1, peor_obj2
	
	return None

def main():
	# Directorio donde se encuentran los datos de AMPL
	dir_ampl = "../Datos AMPL Formateados"

	# Verificar que exista la carpeta
	if not os.path.exists(dir_ampl):
		print(f"Error: Asegúrate de que la carpeta '{dir_ampl}' exista.")
		return

	# Obtener los nombres de archivos válidos en la carpeta
	archivos_ampl = set(f for f in os.listdir(dir_ampl) if os.path.isfile(os.path.join(dir_ampl, f)))

	print(f"Se encontraron {len(archivos_ampl)} archivos para procesar. Generando 'optimos.txt' y 'All.inst'...")

	# Abrimos AMBOS archivos en modo escritura ("w")
	with open("optimos.txt", "w") as f_out, open("All.inst", "w") as f_inst:
		for nombre_archivo in archivos_ampl:
			archivo_ampl = os.path.join(dir_ampl, nombre_archivo)

			# Calculamos el peor punto ajustado solo para este archivo de AMPL
			peores_valores = obtener_peores_valores_global([archivo_ampl])

			if peores_valores:
				# Formatear el punto de referencia
				ref0, ref1, ref2 = peores_valores
				ref_str = f"{ref0} {ref1} {ref2}"
				
				# Comando para calcular el hipervolumen
				cmd = f'./hv -r "{ref_str}" "{archivo_ampl}"'
				
				# Ejecutar el comando, pero en lugar de imprimirlo, capturamos su salida
				resultado = subprocess.run(cmd, shell=True, capture_output=True, text=True)
				
				if resultado.returncode == 0:
					# Convertimos el texto de ./hv a float, lo redondeamos y lo pasamos a entero
					hipervolumen = int(round(float(resultado.stdout.strip())))
					
					# Cambiamos "_data" por ".dat" en el nombre de la instancia
					nombre_final = nombre_archivo.replace("_data", ".dat")
					
					# Escribimos en optimos.txt
					f_out.write(f"{nombre_final} {hipervolumen} {int(round(ref0))} {int(round(ref1))} {int(round(ref2))}\n")
					
					# NUEVO: Escribimos en All.inst solo el nombre de la instancia
					f_inst.write(f"{nombre_final}\n")
				else:
					print(f"Error al ejecutar ./hv para {nombre_archivo}: {resultado.stderr.strip()}")
			else:
				print(f"Advertencia: No se encontraron datos válidos para la instancia {nombre_archivo}")

	print("\n¡Proceso terminado exitosamente! Revisa los archivos 'optimos.txt' y 'All.inst'.")

if __name__ == "__main__":
	main()