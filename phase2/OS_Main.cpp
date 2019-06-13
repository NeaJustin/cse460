/*********************************************************************************
 * Justin Pulido
 * September 30, 2018
 * Main.cpp
 * The program will be tasked with having to run multiple programs at the same
   time and having to allocate a set amount of time to each one. It will
   also need to save a stack of activation records for the fact programs 
   since those are the ones that will be building their own stack. 
 * The program accomplishes this task by calling the constructor in the OS
   then in the constructor it will open the programns and will follow the 
   procedure of giving the amount of time needed for the programs to run. 
 * 100% complete
********************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>

#include "Assembler.h"
#include "VirtualMachine.h"
#include "OS.h"

using namespace std;

int main(int argc, char * argv[])
{
    cout << "*************************************************" << endl;
    cout << "Starting Phase 2" << endl;
    cout << "*************************************************" << endl;

    OS os;
    os.run();
    cout << "Successful completion!" << endl;
    return 0;
}
