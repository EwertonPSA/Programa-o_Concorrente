all:
	mpicc studentspar.c -o students -lm
run:
	mpirun -np 48 --hostfile my_hostfile students <input.in
compParalelo:
	mpicc studentspar.c -o students -lm
compSequencial:
	mpicc studentsseq.c -o students -lm
runParalelo:
	mpirun -np 48 --hostfile my_hostfile students <input.in
runSequencial:
	mpirun -np 1 students <input.in