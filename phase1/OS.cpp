/*********************************************************************************
 * Justin Pulido
 * September 30, 2018
 * OS.cpp
 * The program will be reading a text file and grabbing the load inputs and be 
   passed into the virtual machine to be made for instrunctions. After it is 
   translated then it will be read for the isntrunctions to do its use the 
   regristers and be added into into the correct spots. *fix this, sound weird 
   and unclear. 
 * The program solves this program from using the Assembler, and Virtual machine
   where it will then grab the string from the file text and pass it to the 
   Assembler to get the op code and the input, then passed into the virtual
   Machine and will have perform different actions bacsed on the opcodes and 
   the instrunction that comes with it. From the instrunction of the Virtual 
   Machine it will execute the instrunctions and perform the tasks it is
   required to do. *fix this too, there is a lot of repetitivness. 
 *
********************************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cstdlib>

#include "Assembler.h"
#include "VirtualMachine.h"

using namespace std;

int main(int argc, char * argv[])
{
    //calling the constructor from both the virtual and assembler. 
    Assembler as;
    VirtualMachine vm;

    //checking to see if there is a file input when the program starts. 
    //if it does not have a file being passed through, exit
    if(argc == 1) {
        cout << "Please enter a file name." << endl;
        exit(1);
    }
    //taking the argument value and passsing it to in1. 
    string in1 = argv[1];
    int pos = in1.find(".");
    //checking to see if the argument value has a s after 
    //finding the . suffix. if it does not then exit the program. 
    //if it does then continue. 
    if(pos > in1.length() || in1[pos+1] != 's') {
        cout << "No .s suffix. Not a valid file." << endl;
        exit(2);
    }

    string name = in1.substr(0, pos);
    //creating an outpt for by grabbing the original name
    //and changing the .s with a .o
    string output_File = name + ".o";
   
    //creating a file stream for assembly and the object_code
    fstream assembly, Object_Code;
    //converting the file input to character string to be read into
    //the file. 
    assembly.open(in1.c_str(), ios::in);
    //converting the output file into a character of strings to be
    //outputed into the file
    Object_Code.open(output_File.c_str(), ios::out);

    //
    if(as.assemble(assembly, Object_Code)) {
        cout << "Assembler Error." << endl;
        assembly.close();
        Object_Code.close();
        exit(3);
    }
    assembly.close();
    Object_Code.close();

    Object_Code.open(output_File.c_str(), ios::in);
    fstream in, out;
    in.open((name + ".in").c_str(), ios::in);
    out.open((name + ".out").c_str(), ios::out);
    
    vm.run(Object_Code, in, out);
    out << "\nClock = " << vm.get_clock() << endl;

    Object_Code.close();
    in.close();
    out.close();
    
    return 0;
}
