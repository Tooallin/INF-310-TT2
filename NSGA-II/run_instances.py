import subprocess
import itertools
import time

instance_routes = [
	"Instances/c104.dat",
]

random_seeds = [
	0.123,
	0.237,
	0.351,
	0.468,
	0.582,
	0.619,
	0.703,
	0.745,
	0.862,
	0.974
]

popsize = 500
ngen = 1000
pcross = 0.6
pmut = 0.1

# === Ejecutar combinaciones ===
for instance, seed in itertools.product(instance_routes, random_seeds):
	print(f"\nSolving {instance} with seed {seed}")
	command = [
		"./nsga2r",
		instance,
		str(seed),
		str(popsize),
		str(ngen),
		str(pcross),
		str(pmut)
	]

	start_time = time.time()
	subprocess.run(command)
	elapsed = time.time() - start_time
	print(f"->Tcpu {elapsed:.2f}")