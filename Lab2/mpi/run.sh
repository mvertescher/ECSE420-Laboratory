#!/bin/bash

#time ./run.sh

cd ./p2p-cons && make && cd ..
cd ./p2p-cycl && make && cd ..
cd ./bcast-cons && make && cd ..
cd ./bcast-cycl && make && cd ..

# Size of matrix N (1024, 2048, 4096)
# Number of processes P (2, 4, 8, 16)

serial_exec="1"

# Assume folder hist created
echo 'Creating Gaussian Elimination histograms and charts'
for (( N = 1024; N<=4096; N*=2 )) do
	speedupfile="./speedup/speedup$N.dat"
	rm $speedupfile
	if [ "$N" = "1024" ]; then
		serial_exec="3.7552061081"
	elif [ "$N" = "2048" ]; then
		serial_exec="30.538867950"
	else  
		serial_exec="241.31324387"
	fi

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
		
		IFS=$' ' read p2p_cons_exec p2p_cons_proc p2p_cons_comm <<< $(mpiexec -np $P ./p2p-cons/program $N)
		IFS=$' ' read p2p_cycl_exec p2p_cycl_proc p2p_cycl_comm <<< $(mpiexec -np $P ./p2p-cycl/program $N)
		IFS=$' ' read bcast_cons_exec bcast_cons_proc bcast_cons_comm <<< $(mpiexec -np $P ./bcast-cons/program $N)
		IFS=$' ' read bcast_cycl_exec bcast_cycl_proc bcast_cycl_comm <<< $(mpiexec -np $P ./bcast-cycl/program $N)

		echo "version t_exec t_proc t_comm" >> $histfile
		echo "p2p-cons $p2p_cons_exec $p2p_cons_proc $p2p_cons_comm" >> $histfile
		echo "p2p-cycl $p2p_cycl_exec $p2p_cycl_proc $p2p_cycl_comm" >> $histfile
		echo "bcast-cons $bcast_cons_exec $bcast_cons_proc $bcast_cons_comm" >> $histfile
		echo "bcast-cycl $bcast_cycl_exec $bcast_cycl_proc $bcast_cycl_comm" >> $histfile

		p2p_cons_su=$(bc <<< "scale = 10; $serial_exec / $p2p_cons_exec")
		p2p_cycl_su=$(bc <<< "scale = 10; $serial_exec / $p2p_cycl_exec")
		bcast_cons_su=$(bc <<< "scale = 10; $serial_exec/ $bcast_cons_exec")
		bcast_cycl_su=$(bc <<< "scale = 10; $serial_exec / $bcast_cycl_exec")
		echo "$P $p2p_cons_su $p2p_cycl_su $bcast_cons_su $bcast_cycl_su"
		echo "$P $p2p_cons_su $p2p_cycl_su $bcast_cons_su $bcast_cycl_su" >> $speedupfile
	done
done

exit 0