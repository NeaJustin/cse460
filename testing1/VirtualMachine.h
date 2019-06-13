#ifndef VIRTUALMACHINE_H
#define VIRTUALMACHINE_H

/*************************************************************************
 * Richard Vargas, Justin Pulido
 * September 26, 2018
 * VirtualMachine.h
 * The virtual machine will set the registers, and will now be responsible
   for keeping checking the clock with the time that is allocated to each
   program that is currently running. It will be doing so by checking the 
   sr values and seeing if it needs to be returned due to an error. 
 * 100% complete
*************************************************************************/

#include <vector>
#include <fstream>

using namespace std;

class VirtualMachine {
	int msize;
	int rsize;
	int pc, ir, sr, sp, clock;
	vector<int> mem;
	vector<int> r;
	int base, limit;
	int total_limit;
public:
    //setting the constructor to have a memory size of 256, register size of 4,
    //and reserving that mch memory in a vector and rserving it as well for r. 
    //and setting clock ot 0. 
	VirtualMachine(): msize(256), rsize(4), clock(0) { mem.reserve(msize); r.reserve(rsize); }
    //creating a load function that will load the program being passed in,
    //its base and its limit
    int load(fstream&, int base, int & limit);
    //having a run that will take a int, and two filestreams. 
	void run(int, fstream &, fstream &);
friend
    //having the class be a friend to the OS and the PCB to have access to certain
    //properties of it. 
	class OS;
    class PageTable;
    class TLB;
}; // VirtualMachine

#endif
