os: os_main.o OS.o VirtualMachine.o Assembler.o
	g++ -o os os_main.o OS.o VirtualMachine.o Assembler.o PageTable.o

os_main.o: os_main.cpp
	g++ -c os_main.cpp

OS.o: OS.cpp OS.h
	g++ -c OS.cpp

PageTable.o: PageTable.cpp PageTable.h
	g++ -c PageTable.cpp

VirtualMachine.o: VirtualMachine.cpp VirtualMachine.h
	g++ -c VirtualMachine.cpp

Assembler.o: Assembler.cpp Assembler.h
	g++ -c Assembler.cpp

clean:
	rm *.o os
