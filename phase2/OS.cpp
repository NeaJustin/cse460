/***********************************************************************************************
* Justin Pulido, Richard Vargas
* OS.cpp
* November 6th, 2018
* The OS.cpp will be responsible for creating the pcb and being able to handle the
  context switching, time slicing, having multiple files be pushed onto the ready
  and waitq as well as popping off or deciding what should be done next. It will
  keep a total time for cpu_time for all the operations to be completed. It will 
  also have all the registers saved in the save state and having everything loaded
  back when it is being called from the loadState. Each which is done by the run
  in the os.cpp. *reword a little more
*70% completion. 
***********************************************************************************************/

#include <iostream>
#include <stack>
#include <string>
#include <cstdlib>
#include <fstream>

#include "OS.h"
#include "Assembler.h"
#include "VirtualMachine.h"
using namespace std;

OS::OS()
{
    sys_time = 0;
    idle_time = 0;
    
    //gather all assembly programs and send them to progs fstream
    system("ls *.s > progs");
    progs.open("./progs");
    
    //checking if progs stream has anything
    if(!progs.is_open()) {
        cout << "no file to read" << endl;
        exit(1);
    }
    
    //buffe rand filstreams for opening files
    int mem_Location = 0;
    string Asm_file_buffer;
    string Obj_file_buffer;
    fstream Asm;
    fstream Objc;

    //while the progs are being inputed into the buffer
    while(progs >> Asm_file_buffer) {
        string file_Prefix = Asm_file_buffer.substr(0, Asm_file_buffer.find("."));
        Obj_file_buffer = file_Prefix + ".o";
        cout << "Process: " << file_Prefix <<  "...";
        
        //opening assembly file stream
        Asm.open(Asm_file_buffer.c_str(), ios::in);
        Objc.open(Obj_file_buffer.c_str(), ios::out);
        as.assemble(Asm, Objc);
        cout << " Assembled.." << endl;
        Asm.close();
        Objc.close();
        
        Objc.open(Obj_file_buffer.c_str(), ios::in);
        //why calling mem_location twice??
        PCB *new_PCB = new PCB(file_Prefix, mem_Location, mem_Location);
        cout << "PCB made" << endl;
        
        //in and out stream for pcb in order to take input/output for programs
        string in_buffer = file_Prefix + ".in";
        string out_buffer = file_Prefix + ".out";
        new_PCB->in.open(in_buffer.c_str(), ios::in);
        new_PCB->out.open(out_buffer.c_str(), ios::out);
        
        //loading program into memory 
        for( ; Objc >> vm.mem[mem_Location]; mem_Location++);
        cout << new_PCB->limit << endl;
        cout << new_PCB->base << endl;
        Objc.close();
        pcb.push_back(new_PCB);
        readyQ.push(new_PCB);
        cout << "loaded" << endl;
    }
    cout << "****************************************************\n";
    cout << "All programs loaded" << endl;
    cout << "****************************************************\n";

    running = readyQ.front();
    readyQ.pop();
}



//deconstructor
OS::~OS() 
{
    //delete new_PCB;
}

/************************************************************************************
 *run will be responsible for handling the total cpu_time and checking the ready,
  and waitq. As well as checking the running to see if it is null or not. Each
  time it will check there is anything running and if nothing is running,
  then it will grab from the readyq and have it set to the running to execute. 
  then it will load the state that it was at previously and add the time slice. 
  if it the programs are still waiting for their turn to execute, it will sit in 
  idle mode.
************************************************************************************/
void OS::run()
{
    do {
        //checking to see if the running is not null and the readyq is not
        //empty. 
        if(running != NULL || !readyQ.empty()) {
            //if running is null and ready q is not empty
            if(running == NULL && !readyQ.empty()) {
                //push the front of the ready q onto the running
                //then pop the readyq to have another program in the front
                running = readyQ.front();
                readyQ.pop();
            }
            //load the state of the program. 
            load_State();
            cout << "Current program running: " << running->prog << endl;
            //adding the time slice for the program after it is running 
            //an extended amount of time. Then context Switching. 
            running->cpu_time += vm.run(Time_Slice);
            context_Switch();
        }
        else {
            //sitting in idle time.
            cout << "In else of run" << endl;
            vm.clock++;
            idle_time++;
            //checking the waitq and this takes 5 clock ticks. 
            if(check_WaitQ()) {
                idle_time += 5;
                //context_Switch();
            }
        }
        //why commented out?
        //context_Switch();
        //collecting the total time. 
        int collective_Time = vm.get_clock();
      //while the readyq, waitq is not empty and the running is 
      //not null, continue going through the loop
    } while(!readyQ.empty() || !waitQ.empty() || running != NULL);
    return;
}

//loading the state of the file and setting the buffer to the beginning to
//continue reading from where it left off. 
void OS::load_State()
{
    //getting the running program from the program counter and setting 
    //it to the program counter of the vm. 
    vm.pc = running->pc;
    //loading the gernal purpose registers into the vm
    for(int i = 0; i < 4; i++) {
        vm.r[i] = running->r[i];
    }
    //loading ir, sr, sp, base, and limit
    vm.ir = running->ir;
    vm.sr = running->sr;
    vm.sp = running->sp;
    vm.base = running->base;
    vm.limit = running->limit;
    
    //as long as the stack pointer is not 256 keep running
    if(vm.sp != 256) {
        //adding st to the end of the running program for its stack. 
        string Stack_buffer = running->prog + ".st";
        //creating a stack_load file stream. 
        fstream stack_Load;
        //opening the .in file of the program.
        stack_Load.open(Stack_buffer.c_str(), ios::in);
        while(running->sp != 256) {
            //grab the running programs stack pointer and 
            //put it into the stack_Load.
            stack_Load >> vm.mem[running->sp];
            //increment stack pointer
            running->sp++;
        }
        //closing the file 
        stack_Load.close();
    }
    //open the .in and .out file of the running program. 
    running->in.open(running->prog + ".in");
    running->out.open(running->prog + ".out");
    return;
}

/*************************************************************************
 *will save the state of the current program that is running. Which
  includes the gerneral purpose registers, ir, sp, sr, base and limit. 
  Making sure that the stack does not equal 256 as well so that the
  stack is never empty. 
*************************************************************************/
void OS::save_State()
{
    //setting the virtual machine program counter to the running program
    //counter.
    running->pc = vm.pc;
    //saving the general purpose registers. 
    for(int i = 0; i < 4; i++) {
        running->r[i] = vm.r[i];    
    }
    //saving the ir, sr, sp, base and limit
    running->ir = vm.ir;
    running->sr = vm.sr;
    running->sp = vm.sp;
    running->base = vm.base;
    running->limit = vm.limit;

    //as long as the stack pointer of vm is not 256 then
    //continue. 
    if(vm.sp != 256) {
        //creating a stack buffer of the program and adding .st 
        //to the end of it. 
        string Stack_buffer = running->prog + ".st";
        //creating a file stream of stack_load. 
        fstream stack_Load;
        //opening the out file of the programs. 
        stack_Load.open(Stack_buffer.c_str(), ios::out);
           
        while(running->sp != 256) {
            //putting the stack pointer of the virtual machine
            //memory onto the stack_load. 
            stack_Load << vm.mem[running->sp];
            //incrememnting stack pointe. 
            running->sp++;
        }
        //close stack_load. 
        stack_Load.close();
    }
    return;
}

//will be responsible for allocating a specific amount of time for each
//program to run. Each time switching between the different programs 
//that are running. 
void OS::context_Switch()
{
    //adding 5 clock ticks to the system
    sys_time += 5;
    //checking system time
    cout << sys_time << endl;
    check_WaitQ();
    save_State();
    cout << "save_State Reached" << endl;

    int vm_Return_Status = running->sr;
    vm_Return_Status &= 0x3E0;
    switch(vm_Return_Status & 0x3E0) {
        case(0xE0): {
            cout << "IO output reached" << endl;
            //and the vm return status with 0x300 and shift it
            //over 8 times 
            int reg = (vm_Return_Status&0x300)>>8;
            //put the register of the running into the out
            //file 
            running->out << running->r[reg];
            //push it onto the wait q
            waitQ.push(running);
            //add 27 clock ticks to the vm.clock
            running->io_completion = vm.clock + 27;
            //close the in and out file 
            running->out.close();
            running->in.close();
            //set running to null 
            running = NULL;
            break;
        }
        case(0xC0): {
            //checking for invalid opcode and if it is,
            //then terminate the program and close the in and out file 
            //and set the running to null 
            running->out << "Invalid opcode." << endl;
            cout << running->prog << " experience invalid opcode" << endl;
            cout << "Terminating..." << endl;
            running->out.close();
            running->in.close();
            running = NULL;
            break;
        }
        case(0x80): {
            //if the program causes a stack underflow
            //terminate the program.
            running->out << "Stack underflow.." << endl;
            cout << running->prog << " caused stack underflow" << endl;
            cout << "terminating program.." << endl;
            running->out.close();
            running->in.close();
            running = NULL;
            break;
        }
        case(0x60): {
            //if the program causes a stack overflow,
            //then terminate the program. 
            running->out << "Stack overflow.." << endl;
            cout << running->prog << " caused stack overflow.." << endl;
            cout << "terminating program.." << endl;
            running->out.close();
            running->in.close();
            running = NULL;
            break;
        }
        case(0x40): {
            //checking for out of bounds error and if it does have one
            //then terminate the program and display to the user which 
            //program is causing it
            running->out << "out of bound error" << endl;
            cout << running->prog << " has an out of bound error." << endl;
            cout << "terminating program.." << endl;
            running->out.close();
            running->in.close();
            running = NULL; 
            break;
        }
        case(0x20): {
            //if the program has a successfull halt then display to the user
            //that and terminate the program. 
            cout << running->prog << " successfull halt" << endl;
            cout << "terminating.." << endl;
            running->out.close();
            running->in.close();
            running = NULL;
            break;   
        }
        default: {
            //else close the in and out of the running
            //then push onto the readyQnand set running to NULL
            running->out.close();
            running->in.close();
            readyQ.push(running);
            running = NULL;
            break;
        }
    }
    return;
}

//checking the wait q to see if the io has completed, and passes it to 
//the ready queue
bool OS::check_WaitQ()
{
    //if there is nothing on the waitq return false
    if(waitQ.front() == NULL) {
        return false;
    }
    
    //if the waitQ io time completion is less then or equal to the
    //virtual machines clock
    if(waitQ.front()->io_completion <= vm.clock) {
        //it has completed its operation
        cout << "completed I/O" << endl;
        //push it onto the readyQ.
        readyQ.push(waitQ.front());
        //pop off the waitq 
        waitQ.pop();
        return true;
    }
    //else return false
    return false;
}

