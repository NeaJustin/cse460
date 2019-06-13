OS: OS_Main.o OS.o VirtualMachine.o Assembler.o
	g++ -o OS OS_Main.o OS.o VirtualMachine.o Assembler.o PageTable.o

OS_Main.o: OS_Main.cpp
	g++ -c OS_Main.cpp

OS.o: OS.cpp OS.h
	g++ -c OS.cpp

PageTable.o: PageTable.cpp PageTable.h
	g++ -c PageTable.cpp

VirtualMachine.o: VirtualMachine.cpp VirtualMachine.cpp
	g++ -c VirtualMachine.cpp

Assembler.o: Assembler.cpp Assembler.h
	g++ -c Assembler.cpp

clean:
	rm *.o OS
