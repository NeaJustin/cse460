/**********************************************************************************
* Justin Pulido, Richard Vargas
* OS.cpp
* November 6th, 2018
* The OS.cpp will be responsible for creating the pcb and being able to handle the
  context switching, time slicing, having multiple files be pushed onto the ready
  and waitq as well as popping off or deciding what should be done next. It will
  keep a total time for cpu_time for all the operations to be completed. It will 
  also have all the registers saved in the save state and having everything loaded
  back when it is being called from the loadState. Each which is done by the run
  in the os.cpp. Now we are going to be adding multiprogramming degree which
  will make it to where only 5 Programs running at a time. When one program 
  terminates, another will now come in and it is created to have its page table
  as well. 
*70% completion. 
**********************************************************************************/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <queue>
#include "OS.h"
#include "VirtualMachine.h"
#include "Assembler.h"

using namespace std;

/**********************************************************************************
 *The os will gather all the programs and load them into a progs file. In the progs
  file it will then make sure that the progs is open and it will be passing the 
  prog into the progs. where it will grab the file and rename it to the .o file.
  Each program will then have its own pcb that will then have its own base and
  limit created along with it. Where it is loaded when it needs to run. 
**********************************************************************************/
OS::OS()
{
    idle_time = 0;
    sys_time = 0;
    multiProgDeg = 5;
    system("ls *.s > progs");
    progs.open("progs", ios::in);
    assert(progs.is_open());

    int base=0, limit, pageAmount, wordSize;
    string prog, FILE_BUFFER;

    while (progs>>prog) {
        fstream src, obj;
        waitingFiles.pop();
        int pos = prog.find(".");
        //grabbing the name of the file 
        string prog_name = prog.substr(0, pos);

        //open the program for the in file.
        src.open(prog.c_str(), ios::in);
        //open the object of the program name of the .o 
        obj.open((prog_name+".o").c_str(), ios::out);
        //checking to see if the src and obj is open. 
        assert(src.is_open() and obj.is_open());

        if (as.assemble(src, obj)) {
            cout << "Assembler Error in " << prog << "\n";
            src.close();
            obj.close();
            continue;
        }

        //close src and obj
        src.close();
        obj.close();
        //open the o file of the object and passing it. 
        obj.open((prog_name+".o").c_str(), ios::in);
        assert(obj.is_open());
        //loading the object into the vm with its base and limit
        wordSize = 0;
        while(getline(obj,FILE_BUFFER))
        {   
            wordSize ++;
        }
        cout << wordSize << endl;
        if((wordSize) % 8 == 0) pageAmount = (wordSize)/8;
        else pageAmount = ((wordSize)/8) + 1;        
        obj.close();
        //creating pcb for the job and giving it a name, base and limit
        PCB * job = new PCB(prog_name, base, limit-base,pageAmount);
        job->in.open((prog_name+".in").c_str(), ios::in);
        job->out.open((prog_name+".out").c_str(), ios::out);
        job->stack.open((prog_name+".st").c_str(), ios::in | ios::out);
        job->disk.open((prog_name+".o").c_str(), ios::in | ios::out);
        //assert((job->in).is_open() and (job->out).is_open() and (job->stack).is_open());
        //creatign the .st files. 
		system(string("touch " + prog_name + ".st").c_str());
        pcb.push_back(job);
        base = limit;
    }
    vm.total_limit = limit;

    //pushing the program that is on the pcb onto the readyQ to be executed.
	//also checking to see if there are already 5 programs executing. If the 
	//multi program degree is not less than 0, push the job from the 
	//pcb to the readyQ to execute. If it is less than 0, then check 
	//the waiting Files and push it onto the readyQ if there is anything 
	//waiting. 
    for (list<PCB *>::iterator i = pcb.begin(); i != pcb.end(); i++)
    {
        if (multiProgDeg > 0)
        {
            readyQ.push(*i);
            readyQ.back()->disk >> vm.mem[base];
            multiProgDeg--;
        }
        else 
        {
            waitingFiles.push(*i);
            //readyQ.push(waitingFiles);
        }
    }
}

/*********************************************************************************
 *Check waiting Files will take the base and the limit of the prgram which
  is currently running that has ended. It will then pop it from the waiting
  files and have a new process enter that will then go through the os 
  to be able to create its stack file and need to create its page table,
  and the .o and .st file.  
*********************************************************************************/
void OS::checkWaitingFiles(int base, int limit)
{
/*
    if(!waitingFiles.empty()) {
        cout << "Hello" << endl;
        readyQ.push(waitingFiles.front());
    }
    if(waitingFiles.empty()) {
        cout << "hello";   
    }
*/
}


/**********************************************************************************
 its turn around time, cpu time, wait time, io time, total cpu time, system time, 
 idle time, final clock, system cpu utilization, and throughput. Then it will 
 close the in, ot and stack of the object on the pcb. Then after rit is done 
 it will close the progs file to stop execution. 
**********************************************************************************/
OS::~OS()
{
    list<PCB *>::iterator i;

    int cpu_time = 0;

    //grabbing the begging of the pcb and adding its time and adding it to the 
    //cpu time.
    for (i = pcb.begin(); i != pcb.end(); i++)
        cpu_time += (*i)->cpu_time;

    //used for displaying the turn around time, cpuu time, wait time,
    //and io time for the first out.
    //second out displays the total cpu time, system time, total cpu 
    //utilization and the throughput 
    //then it closes the .in, .out, and stack. 
    for (i = pcb.begin(); i != pcb.end(); i++) {
        (*i)->out << "Turn around time = " << (*i)->turn_around_time << ", CPU time = " << (*i)->cpu_time
              << ", Wait time = " << (*i)->wait_time << ", IO time = " << (*i)->io_time << endl;

        (*i)->out << "Total CPU time = " << cpu_time << ", System time = " << sys_time
              << ", Idle time = " << idle_time << ", Final clock = " << vm.clock << endl
              << "Real CPU Utilization = " << setprecision(3) << (float) cpu_time / vm.clock * 100 << "%, "
              << "System CPU Utilization = " << (float) (vm.clock - idle_time) / vm.clock * 100 << "%, "
              << "Throughput = " << pcb.size() / ((float) vm.clock / 1000) << endl;
			  
		(*i)->out << "Number of Page-faults = " << (*i)->page_faults << ", "
			  << "Hit Ratio = " << setprecision(3) << (float) (*i)->page_hits/((*i)->page_hits+(*i)->page_faults)
			  << "%" << endl;

        (*i)->in.close();
        (*i)->out.close();
        (*i)->stack.close();
    }

    //close progs. 
    progs.close();
}

/*************************************************************************************
 *run will be responsible for handling the total cpu time and checking the ready and 
  waitq. As well as checking the rnning to see if it is null or not. Each time it 
  will check will check if anything is running and if nothing is running, then it 
  will grab from the readyq na dhave it set to the running to execute. Then it will 
  load the state that it was at previously and add the time slice. If the prgrams are
  in the waitq and not finished with their time to be completed and put into the 
  readyq, the os will sit in Idle mode(time). 
*************************************************************************************/
void OS::run()
{
    int time_stamp;

    running = readyQ.front();
    readyQ.pop();

    //while there are still programs to run    
    while (running != 0) {
        //adding the clock of the vm to the context switch time. 
        vm.clock += CONTEXT_SWITCH_TIME;
        //adding context switch onto the system time. 
        sys_time += CONTEXT_SWITCH_TIME;
        //loading the state of the program. 
        loadState();

        //creating a time stamp of the clock. 
        time_stamp = vm.clock;
        //passing time sl;ice, the running in file and the running out file 
        vm.run(TIME_SLICE, running->in, running->out);
        //subtract the time stamp with the vm clock and add it to the cpu time
        running->cpu_time += (vm.clock - time_stamp);

        contextSwitch();
    }
}

/*************************************************************************************
 *will be responsible for allocating a specific amount of time for each program
  to run. Each time switching between the different programs where the programs 
  are switching between one another. Now the context switch will also be 
  be checking the cases ,1,2,3,4,5 and 32 to see when a program has occured errors
  within it. When there is an error it will call for the checkWaitingFiles to see
  if there are any programs that are going to be needed to be entered.  
*************************************************************************************/
void OS::contextSwitch() 
{
    //if the wait q is not empty, and the io completion is not less than
    //or equal to the clock of the vm. 
    //then push it on the readyQ. Start a new wait time for the beginning
    //subtract the clock from the waitQ of the time begin. 
    //then pop the waitQ
    while (not waitQ.empty() and waitQ.front()->io_completion <= vm.clock) {
        readyQ.push(waitQ.front());
        waitQ.front()->wait_time_begin = vm.clock;
        waitQ.front()->io_time += (vm.clock - waitQ.front()->io_time_begin);
        waitQ.pop();
    }

    int vm_status = (vm.sr >> 5) & 047;
    switch (vm_status) {
        case 0: //Time slice
            readyQ.push(running);
            running->wait_time_begin = vm.clock;
            break;
        case 1: //Halt
            //passing terminated from the program into the out file.
            running->out << running->prog << ": Terminated\n";
            //running->out << running->prog << running->turn_around_time;
            //setting the clock equal to the turn around time. 
            running->turn_around_time = vm.clock;
            checkWaitingFiles(running->base,running->limit);
            break;
        case 2: //Out of Bound Error
            running->out << running->prog << ": Out of bound Error, pc = " << vm.pc << endl;
            running->turn_around_time = vm.clock;
            checkWaitingFiles(running->base,running->limit);
            break;
        case 3: //Stack Overflow
            //get the running program and show where it caused the overflow
            //in the out program. 
            running->out << running->prog << ": Stack overflow, pc = " << vm.pc << ", sp = " << vm.sp << endl;
            //grab the clock and set it to the turn around time of the running. 
            running->turn_around_time = vm.clock;
            checkWaitingFiles(running->base,running->limit);
            break;
        case 4: //Stack Underflow
            //writing to the out file of the program that has stack underflow error
            running->out << running->prog << ": Stack underflow, pc = " << vm.pc << ", sp = " << vm.sp << endl;
            //setting vm clock to turn around time
            running->turn_around_time = vm.clock;
            checkWaitingFiles(running->base,running->limit);
            break;
        case 5: //Bad Opcode
            //getting the running program and writing to its out file that it 
            //contains a bad opcode and grabbing it from the vm of its program 
            //counter. Which specific line is causing the error.
            running->out << running->prog << ": Bad opcode, pc = " << vm.pc << endl;
            //setting clock to turn arond time.  
            running->turn_around_time = vm.clock;
            checkWaitingFiles(running->base,running->limit);
            break;
        case 6: //Read
        case 7: //Write
            //pushing the running program onto the waitq
            waitQ.push(running);
            //adding 27 clock ticks to the vm clock and setting it to the 
            //io completion of the running. 
            running->io_completion = vm.clock + 27;
            running->io_time_begin = vm.clock;
            break;
        case 32://Page fault
            //Pull in page from disc here
        default:
            //will say there was an if there was something wrong with the 
            //program that was running. 
            cerr << running->prog << ": Unexpected status = " << vm_status 
                 << " pc = " << vm.pc << " time = " << vm.clock << endl;
            running->out << running->prog << ": Unexpected status: " << vm_status 
                 << " pc = " << vm.pc << " time = " << vm.clock << endl;
            running->turn_around_time = vm.clock;
    }

    //calling save state
    saveState();
    //set running to 0
    running = 0;

    //if readyq is not empty then set the running to the front of the q,
    //set the wait time by taking the clock and subracting it from the 
    //wait time begin from the running.
    //then pop the readyq. 
    if (not readyQ.empty()) {
        running = readyQ.front();
        running->wait_time += (vm.clock - running->wait_time_begin);
        readyQ.pop();
    } 
    //if the waitq is not empty, then set the running program to be the 
    //front of the waitq. Then pop the wait q and have its idle time be
    //set to the io completion of the running minus the vm clock. 
    //after, set the clock to the running of the io completion. 
    else if (not waitQ.empty()){
        running = waitQ.front();
        waitQ.pop();
        idle_time += (running->io_completion - vm.clock);
        vm.clock = running->io_completion;
        // assume all of context switch time is incurred after idle time
        running->io_time += (vm.clock - running->io_time_begin);
    }
}
/*********************************************************************************
 *load state will grab the current running progam and set it to the program 
  counterr of the virtual machine. Then it will grab the general purpose 
  registers, sp, base, sr, ir, base, limit, and make sure to start at the 
  beginning of the stack for the program to be read properly. 
*********************************************************************************/
void OS::loadState()
{
    //setting running pc to the vm program counter
    vm.pc = running->pc;
    //setting running registers to the vm registers. 
    for (int i = 0; i < 4; i++)
        vm.r[i] = running->r[i];
    //setting the vm ir running ir, and doing the same for the sr, base
    //limit, and sp
    vm.ir = running->ir;
    vm.sr = running->sr;
    vm.base = running->base;
    vm.limit = running->limit;
    vm.sp = running->sp;
    //starting at the beginning of the stack again. 
    //running->stack.seekg(0, ios::beg);
    //having the running stack open and passing the prog.st be passed
    //as a string of the in. 
    running->stack.seekg(0, ios::beg);
    //setting i to be the stack pointer of the vm and checking to see if
    //i is less then the size of memory size, and the running stack does not 
    //fail as well.
    for (int i = vm.sp; i < vm.msize and not running->stack.fail(); i++)
        running->stack >> vm.mem[i];
}

/*********************************************************************************
 *will save the state of the currrent program by grabbing the vitural machine 
  and setting the registers to the running.  
*********************************************************************************/
void OS::saveState()
{
    //set the program counter of the vm to the running
    running->pc = vm.pc;
    //grabbing the general purpose registers.. 
    for (int i = 0; i < 4; i++)
        running->r[i] = vm.r[i];
    //set the ir, sr, sp, base and limit to the running. 
    running->ir = vm.ir;
    running->sr = vm.sr;
    running->base = vm.base;
    running->limit = vm.limit;
    running->sp = vm.sp;
    //starting at the beginning of the stack. 
    //running->stack.seekp(0, ios::beg);
    running->stack.seekp(0, ios::beg);
    for (int i = vm.sp; i < vm.msize; i++)
        running->stack << vm.mem[i] << endl;
}
