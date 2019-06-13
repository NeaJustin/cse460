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

#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

class NullPointerException: public runtime_error 
{
public:
    NullPointerException(): runtime_error("Null Function Pointer!") { }
};

class Assembler 
{
private:
    typedef int (Assembler::*FP)(istringstream &);
    map<string, FP> instrunction_set;
    string s;       //variable for string is s
    int op;         //creating variable for opcode 
public:
    Assembler();         //default constructor
    int assemble(fstream&, fstream&); //creating a file stream for both input and out
    string get_string(); //grabbing the string for load or loadi
    int get_op();        //grabbing opcode
    int string_to_instrunction(string s); //getting string, changing to op code 
    int constant; //setting constant for a load input from the file. 
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
    int _and(istringstream &);
    int andi(istringstream &);
    int _xor(istringstream &);
    int xori(istringstream &);
    int complement(istringstream &);
    int shl(istringstream &);
    int shla(istringstream &);
    int shr(istringstream &);
    int shra(istringstream &);
    int compr(istringstream &);
    int compri(istringstream &);
    int get_stat(istringstream &);
    int put_stat(istringstream &);
    int jump(istringstream &);
    int jumpl(istringstream &);
    int jumpe(istringstream &);
    int jumpg(istringstream &);
    int call(istringstream &);
    int _return(istringstream &);
    int read(istringstream &);
    int write(istringstream &);
    int halt(istringstream &);
    int noop(istringstream &);  
};

#endif
