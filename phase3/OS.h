#ifndef OS_H
#define OS_H
/************************************************************************
 * Richard Vargas, Justin Pulido
 * OS.h
 * November 7th, 2018
 * If the stack *.st does not exist, then there is no stack to pull in.
 * if there is a stack, load it into MAX-1 (255)
 * When program is done executing, if stack pointer = 256, do not save anything
 * Else save the stack to *.st 
 * 100% complete
************************************************************************/

#include <fstream>
#include <list>
#include <queue>
#include <string>

#include "VirtualMachine.h"
#include "Assembler.h" 

using namespace std;

class OS;

class PCB 
{
    int pc;		//Program counter
    int r[4];	//Registers
    int ir;		//Instruction register
    int sr;		//status register
    int sp;		//stack pointer
    int base;	//where memory starts for program
    int limit;	//limit for memory where is ends

    string prog;   //will take in program name
    fstream in;	   //writing *.in
    fstream out;   //writing *.out
    fstream stack; //writing *.st

    int io_completion; //input and output completion

    int turn_around_time;
    int cpu_time;           //used for the total cpu time
    int wait_time;          //total wait time for all procceses 
  	int wait_time_begin;	//setting the wait time to have a starting value
    int io_time;   			//Current I/O time								
  	int io_time_begin; 		//Where the I/O starts from										
public:
  	//p is program name string, b is where in actual memory size 
	//of the program and allocated memory neeedprogram starts, 
  	//and l is limit(the max size)
    PCB(const string &p, const int &b, const int &l):
        prog(p), base(b), limit(l), pc(b), sr(2), sp(256), 
        cpu_time(0), wait_time(0), io_time(0), wait_time_begin(0) { }
friend 
  	//can accesses functions and variables the operating.
    class OS; 
};

class OS 
{
private:	
    //class OS has objects VirtualMachine and Assembler
    VirtualMachine vm;
    Assembler as;

    list<PCB *> pcb;        // linked list of jobs
    queue<PCB *> readyQ; 	//queue for processes ready to execute in CPU
    queue<PCB *> waitQ;		//queue for waiting to be executed in cpu
    PCB * running;  		//Current processes being executed
    const static int Time_Slice = 15;   		//setting time slice to 15
    const static int Context_Switch_Time = 5;   //setting context switch time to 5

    fstream progs;  		//program file stream
    int idle_time; 	    	//total time that process waits to for I/O, etc.
    int sys_time;  		    //total sys time
public:
    OS();					//constructor
    ~OS(); 					//destructor
    void run(); 			//Executes in the VM
    void load_State(); 		//loading the states that were saved from the programs.
    void save_State(); 		//saving the state of a program to refer 
    void Context_Switch();  //switching from one process to another.
};

#endif
