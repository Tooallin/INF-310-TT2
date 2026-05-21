#!/bin/bash

maxEvaluations=10000
algo=NSGA2

scenario=All.scn
        
seed=2	
outputTuner=ParamILS_A${algo}_IAll_S${seed}.out
echo "time ruby paramils2.3.8-source/param_ils_2_3_run.rb -numRun ${seed} -approach focused -userunlog 1 -validN 0 -pruning 0 -maxEvals ${maxEvaluations} -scenariofile ${scenario} > ${outputTuner}"
time ruby paramils2.3.8-source/param_ils_2_3_run.rb -numRun ${seed} -approach focused -userunlog 1 -validN 0 -pruning 0 -maxEvals ${maxEvaluations} -scenariofile ${scenario} > ${outputTuner}	

