shared:
	g++ -std=c++17 -fopenmp simulatedAnnealingCompartilhado.cpp -o shared
	./shared

distr:
	mpic++ simulatedAnnealingDistr.cpp -o simulatedAnnealingDistr
	mpiexec -n 4 ./simulatedAnnealingDistr