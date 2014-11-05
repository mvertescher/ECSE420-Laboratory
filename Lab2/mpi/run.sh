#!/bin/sh

cd ./p2p-cons && make && cd ..
cd ./p2p-cycl && make && cd ..
cd ./bcast-cons && make && cd ..
cd ./bcast-cycl && make && cd ..

# Size of matrix N (1024, 2048, 4096)
#N=4
# Number of processes P (2, 4, 8, 16)
#P=2

#mpiexec -np $P ./p2p-cons/program $N
#mpiexec -np $P ./p2p-cycl/program $N
#mpiexec -np $P ./bcast-cons/program $N
#mpiexec -np $P ./bcast-cycl/program $N

#mpiexec -np 4 ./p2p-cycl/program 20
#mpiexec -np 4 ./bcast-cycl/program 20
#mpiexec -np 8 ./bcast-cycl/program 1024


#mpiexec -np 3 ./bcast-cons/program 9
#mpiexec -np 2 ./bcast-cons/program 1024
#mpiexec -np 4 ./bcast-cons/program 1024

echo 'Running Gaussian Elimination'

for (( N = 1024; N<=4096; N*=2 )) do
	for (( P = 2; P<=16; P*=2 )) do
		echo 'Running p2p-cons N = '$N ' P = '$P 
		mpiexec -np $P ./p2p-cons/program $N
		echo 'Running p2p-cycl N = '$N ' P = '$P 
		mpiexec -np $P ./p2p-cycl/program $N
		echo 'Running bcast-cons N = '$N ' P = '$P 
		mpiexec -np $P ./bcast-cons/program $N
		echo 'Running bcast-cycl N = '$N ' P = '$P 
		mpiexec -np $P ./bcast-cycl/program $N
	done
done
