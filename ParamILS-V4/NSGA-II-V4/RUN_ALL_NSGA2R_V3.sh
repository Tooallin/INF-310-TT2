#!/bin/bash

# Asegura que el script se ejecute en el directorio donde está guardado
# Esto evita problemas con las rutas relativas (como "../Instancias" o "./nsga2r")
cd "$(dirname "$0")" || exit

# Define el nombre del archivo de log con fecha y hora
LOG_FILE="run_all_instances_$(date +%Y%m%d_%H%M%S).log"

echo "====================================================="
echo "Iniciando corrida de instancias en segundo plano..."
echo "Script: RUN_ALL_INSTANCES.py"
echo "Log: $LOG_FILE"
echo "====================================================="

# Ejecución
nohup python3 -u RUN_ALL_INSTANCES.py > "$LOG_FILE" 2>&1 &

# Captura el ID del proceso (PID)
PID=$!

echo "✅ Proceso iniciado con éxito (PID: $PID)."
echo "👉 Para ver el progreso en tiempo real, ejecuta: tail -f $LOG_FILE"
echo "🛑 Para detener el proceso en cualquier momento, ejecuta: kill $PID"
echo "====================================================="