all: os

os: Assembler.h VirtualMachine2.h os.h Assembler.cpp VirtualMachine2.cpp os.cpp

OS_Main.cpp
	g++ -Wall -Wextra -o os Assembler.cpp VirtualMachine2.cpp OS.cpp OS_Main.cpp

clean:
	rm -rf os
