satTest: clean File.o Proof.o Solver.o satTest.o
	g++ -o $@ -g File.o Proof.o Solver.o satTest.o

File.o: File.cpp
	g++ -c -g File.cpp

Proof.o: Proof.cpp
	g++ -c -g Proof.cpp

Solve.o: Solver.cpp
	g++ -c -g Solver.cpp

satTest2.o: satTest2.cpp
	g++ -c -g satTest2.cpp

clean:
	rm -f *.o satTest tags
