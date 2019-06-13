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

#include <fstream>
#include <vector>

using namespace std;

class VirtualMachine 
{
private:
    int msize;
    int rsize;
    int pc, ir, sr, sp, clock;
    vector<int> mem;
    vector<int> r;
    int base, limit;
public:
    VirtualMachine(): msize(256), rsize(4), clock(0) { mem.reserve(msize); r.reserve(rsize); }
    int run(int);
    int get_clock();

friend
    class OS;
    class PCB;
}; // VirtualMachine

#endif
