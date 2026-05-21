#!/bin/bash

# Asegura que el script se ejecute en el directorio donde está guardado
cd "$(dirname "$0")" || exit

# Define una variable de tiempo para unificar los nombres de los archivos
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

# Define los nombres del archivo de log y del archivo PID
LOG_FILE="run_all_instances_${TIMESTAMP}.log"
PID_FILE="run_all_instances_${TIMESTAMP}.pid"

echo "====================================================="
echo "Iniciando corrida de instancias en segundo plano..."
echo "Script: RUN_ALL_INSTANCES.py"
echo "Log: $LOG_FILE"
echo "====================================================="

# Ejecución
nohup python3 -u RUN_ALL_INSTANCES.py > "$LOG_FILE" 2>&1 &

# Captura el ID del proceso (PID)
PID=$!

# Guarda el PID en el archivo
echo $PID > "$PID_FILE"

echo "✅ Proceso iniciado con éxito (PID: $PID)."
echo "💾 El PID ha sido guardado de forma segura en: $PID_FILE"
echo "👉 Para ver el progreso en tiempo real, ejecuta: tail -f $LOG_FILE"
echo "🛑 Para detener el proceso en cualquier momento, ejecuta: kill \$(cat $PID_FILE)"
echo "====================================================="