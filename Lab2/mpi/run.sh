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

serial-exec="1.2"

# Assume folder hist created
echo 'Creating Gaussian Elimination Histograms'
for (( N = 1024; N<=4096; N*=2 )) do
	speedupfile="./speedup/speedup$N.dat"
	rm $speedupfile
	echo "P p2p-cons p2p-cycl bcast-cons bcast-cycl" >> $speedupfile
	for (( P = 2; P<=16; P*=2 )) do
		histfile="./hist/$N-$P.dat"
		rm $histfile

		#echo "version t_exec t_proc t_comm" >> $histfile
		#echo 'Running p2p-cons N = '$N ' P = '$P 
		#echo "p2p-cons "$(mpiexec -np $P ./p2p-cons/program $N) >> $histfile
		#echo 'Running p2p-cycl N = '$N ' P = '$P 
		#echo "p2p-cycl "$(mpiexec -np $P ./p2p-cycl/program $N) >> $histfile
		#echo 'Running bcast-cons N = '$N ' P = '$P 
		#echo "bcast-cons "$(mpiexec -np $P ./bcast-cons/program $N) >> $histfile
		#echo 'Running bcast-cycl N = '$N ' P = '$P 
		#echo "bcast-cycl "$(mpiexec -np $P ./bcast-cycl/program $N) >> $histfile
		
		IFS=$' ' read p2p-cons-exec p2p-cons-proc p2p-cons-comm <<< $(mpiexec -np $P ./p2p-cons/program $N)
		IFS=$' ' read p2p-cycl-exec p2p-cycl-proc p2p-cycl-comm <<< $(mpiexec -np $P ./p2p-cycl/program $N)
		IFS=$' ' read bcast-cons-exec bcast-cons-proc bcast-cons-comm <<< $(mpiexec -np $P ./bcast-cons/program $N)
		IFS=$' ' read bcast-cycl-exec bcast-cycl-proc bcast-cycl-comm <<< $(mpiexec -np $P ./bcast-cycl/program $N)

		echo "version t_exec t_proc t_comm" >> $histfile
		echo "p2p-cons $p2p-cons-exec $p2p-cons-proc $p2p-cons-comm" >> $histfile
		echo "p2p-cycl $p2p-cycl-exec $p2p-cycl-proc $p2p-cycl-comm" >> $histfile
		echo "bcast-cons $bcast-cons-exec $bcast-cons-proc $bcast-cons-comm" >> $histfile
		echo "bcast-cycl $bcast-cycl-exec $bcast-cycl-proc $bcast-cycl-comm" >> $histfile

		# If the there are 4 procs, then append results to speedup file		
		#if [ "$P" = "4" ]; then
		p2p-cons-su=$(bc <<< "scale = 10; $p2p-cons-exec / $serial-exec)")
		p2p-cycl-su=$(bc <<< "scale = 10; $bcast-cycl-exec / $serial-exec)")
		bcast-cons-su=$(bc <<< "scale = 10; $p2p-cons-exec / $serial-exec)")
		bcast-cycl-su=$(bc <<< "scale = 10; $bcast-cycl-exec / $serial-exec)")
		echo "$P $p2p-cons-su $p2p-cycl-su $bcast-cons-su $bcast-cycl-su" >> $speedupfile
		#fi
		#break
	done
	break
done

