#ifndef ASSEMBLER_H
#define ASSEMBLER_H

/***********************************************************************
 * Richard Vargas, Justin Pulido
 * Assembler.h
 * September 27th, 2018
 * The .h file will hold the map in the constructor, as well as being 
   able to get the op codes and match it with the proper bytes that
   it represents. 
 * 100% complete
***********************************************************************/
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>

using namespace std;

class NullPointerException: public runtime_error {
public:
    NullPointerException(): runtime_error("Null Function Pointer!") { }
};

class Assembler {
    typedef int (Assembler::*FP)(istringstream &);
    map<string, FP> jumpTable;
public:
    Assembler(); //default constructor
    int assemble(fstream&, fstream&);// ceating a file stream for both input and output
    int load(istringstream &);  
    int loadi(istringstream &); 
    int store(istringstream &);
    int add(istringstream &);
    int addi(istringstream &);
    int addc(istringstream &);
    int addci(istringstream &);
    int sub(istringstream &);
    int subi(istringstream &);
    int subc(istringstream &);
    int subci(istringstream &);
    int and_instr(istringstream &);
    int andi(istringstream &);
    int xor_instr(istringstream &);
    int xori(istringstream &);
    int compl_instr(istringstream &);
    int shl(istringstream &);
    int shla(istringstream &);
    int shr(istringstream &);
    int shra(istringstream &);
    int compr(istringstream &);
    int compri(istringstream &);
    int getstat(istringstream &);
    int putstat(istringstream &);
    int jump(istringstream &);
    int jumpl(istringstream &);
    int jumpe(istringstream &);
    int jumpg(istringstream &);
    int call(istringstream &);
    int return_instr(istringstream &);
    int read_instr(istringstream &);
    int write_instr(istringstream &);
    int halt(istringstream &);
    int noop(istringstream &);
}; // Assembler

#endif

