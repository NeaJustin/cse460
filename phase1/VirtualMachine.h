#ifndef VIRTURALMACHINE_H
#define VIRTURALMACHINE_H
/***************************************************************************
 * Richard Vargas, Justin Pulido
 * September 26, 2018
 * VirtualMachine.h
 * The Virtual Machine.h will set the registers, memory allocation, pc 
   counter, base and limit, and the register sizes. As well as creating 
   a map that is used for the function calls. 
 * 100% complete. 
***************************************************************************/

#include <fstream>
#include <vector>
#include <map>

using namespace std;

class VirtualMachine {
public:
	VirtualMachine();
	void run(fstream&, fstream&, fstream&);
	int get_clock();
	void load(int &rd, int &i, int &addr, int &constant);//0
	void store(int &rd, int &addr);//1
	void add(int &rd, int &i, int &rs, int &constant);//2
	void addc(int &rd, int &i, int &rs, int &constant);//3
	void sub(int &rd, int &i, int &rs, int &constant);//4
	void subc(int &rd, int &i, int &rs, int &constant);//5
	void _and(int &rd, int &i, int &rs, int &constant);//6
	void _xor(int &rd, int &i, int &rs, int &constant);//7
	void complement(int &rd);//8
	void shl(int &rd);//9
	void shla(int &rd);//10
	void shr(int &rd);//11
	void shra(int &rd);//12
	void compr(int &rd, int &i, int &rs, int &constant);//13
	void get_stat(int &rd);//14
	void put_stat(int &rd);//15
	void jump(int &addr);//16
	void jumpl(int &addr);//17
	void jumpe(int &addr);//18
	void jumpg(int &addr);//19
	void call(int &addr);//20
	void _return();//21
	void read(int &rd, fstream&);//22
	void write(int &rd, fstream&);//23
	void noop();//25
	void inc_Clock() { clock++; };
private:
	int registerSize;
	int memorySize;
	int base, limit;
	int pc, ir, sr, sp, clock;
	vector<int> reg;
	vector<int> memory;
};

#endif
