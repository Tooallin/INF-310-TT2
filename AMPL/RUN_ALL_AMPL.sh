#!/bin/bash

# Define el nombre del archivo de log
LOG_FILE="run_all_instances_$(date +%Y%m%d_%H%M%S).log"

echo "Iniciando RUN_ALL_INSTANCES.py en segundo plano..."
echo "Los logs se guardarán en: $LOG_FILE"

# Ejecución
nohup python3 -u RUN_ALL_INSTANCES.py > "$LOG_FILE" 2>&1 &

# Captura el ID del proceso (PID) y lo muestra
echo "Proceso iniciado con éxito (PID: $!)."