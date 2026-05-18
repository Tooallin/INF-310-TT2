#/bin/bash

dirInstances="Instances"
instance=$1
seed=$5
shift 5

#maximo de evaluaciones totales
evaluaciones=30000000

while [ $# != 0 ]; do
    flag="$1"
    case "$flag" in
        -popsize) if [ $# -gt 1 ]; then
              arg="$2"
              shift
              popsize=$arg
            fi
            ;;
        -pcross) if [ $# -gt 1 ]; then
              arg="$2"
              shift
              pcross=$arg
            fi
            ;;
	    
        -pmut) if [ $# -gt 1 ]; then
              arg="$2"
              shift
              pmut=$arg
            fi
            ;;
        -pcross_greedy) if [ $# -gt 1 ]; then
              arg="$2"
              shift
              pcross_greedy=$arg
            fi
            ;;
        -pmut_greedy) if [ $# -gt 1 ]; then
              arg="$2"
              shift
              pmut_greedy=$arg
            fi
            ;;
        *) echo "Unrecognized flag or argument: $flag"
            ;;
        esac
    shift
done

gen=$(awk "BEGIN {printf \"%d\",(${evaluaciones}/${popsize})}")

params="${popsize} ${gen} ${pcross} ${pmut} ${pcross_greedy} ${pmut_greedy}"
screen=salida
screen2=salida2

rm -rf ${screen}

echo "./nsga2r ${dirInstances}/${instance} 0.${seed} ${params} > ${screen}"
./nsga2r ${dirInstances}/${instance} 0.${seed} ${params} > ${screen}

# Buscar óptimo en archivo
exec<"optimos.txt"
# nombreinstancia hv pr1 pr2 pr3
while read line; do
    set -- $line
    name=$1
    if [[ ${instance} == ${name} ]]; then
        optimo=$2
        pr1=$3
        pr2=$4
        pr3=$5
        echo "nombre: ${name}, optimo: ${optimo}, pr1: ${pr1}, pr2: ${pr2}, pr3: ${pr3}"
    fi
done

# Calcular hv y guardar en quality
echo ${pr1}
echo ${pr2}
echo ${pr3}
#factor=2
#pr1=$(awk "BEGIN {printf \"%.1f\",${pr1}*${factor}}" | sed 's/,/./')
#pr2=$(awk "BEGIN {printf \"%.1f\",${pr2}*${factor}}" | sed 's/,/./')
#pr3=$(awk "BEGIN {printf \"%.1f\",${pr3}*${factor}}" | sed 's/,/./')
#echo ${pr1}
#echo ${pr2}
#echo ${pr3}

if [ -s quality.out ]; then
	dirhv="hv-1.3-src"
	echo "./${dirhv}/hv -r \"${pr1} ${pr2} ${pr3}\" quality.out > out/${screen2}"
	./${dirhv}/hv -r "${pr1} ${pr2} ${pr3}" quality.out > ${screen2}

	hv=$(tail -1 ${screen2})
	gap=$(awk "BEGIN {printf \"%.2f\",100.00*(${optimo}-${hv})/${optimo}}")
else
	gap=100
fi

runlength=$(echo ${gap} | sed 's/,/./')

solved="SAT"
runtime=0
best_sol=0

echo "Result for ParamILS: ${solved}, ${runtime}, ${runlength}, ${best_sol}, ${seed}"

