/*************************************************************************
 * Richar Vargas, Justin Pulido
 * October 17th, 2018
 * VirtualMachine.cpp
 * This file will be holding the functions to be called from the OS
   and will execute via the object codes represented by switch
   cases.  The main function of this program will be run, which has a
   loop that executes each instruction.
 * 100% Complete
************************************************************************/


#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdlib>

#include "VirtualMachine.h"

using namespace std;

void VirtualMachine::load(fstream& object_Code, int base, int & limit)
{
    for(limit = base; object_Code >> mem[limit]; limit++);
}

//takes in the time slice, the in and out files to run in the vm. It
//will then grab the opcodes and do the tasks that are needed for it to run 
//properly and be able to 
void VirtualMachine::run(int time_slice, fstream & in, fstream & out)
{
    const int debug = true;
    int opcode, rd, i, rs, constant, addr, j;
    //time_Allocated += clock;
    int interrupt = clock +  time_slice;
 
    while (pc < (base + limit) && (clock < interrupt)) {
        //program counter of memory is set to instrunction register. 
        ir = mem[pc];
        pc++;
        opcode = (ir&0xf800)>>11;
        //get instrunction register and by 0x600 and shift 9 times
        rd = (ir&0x600)>>9;
        //and 0x100 to ir and shift 8 times
        i = (ir&0x100)>>8;
        //and 0xc0 with ir and shift 6 times. 
        rs = (ir&0xc0)>>6;
        addr = ir&0xff;
        constant = addr;
        if (ir&0x80) {
            constant |= 0xffffff00; // if neg sign extend 
        }
            
        clock++;
    
        if (opcode == 0) { /* load loadi */
            //if imidiate constant is put into rd
            if (i) {
                r[rd] = constant;
            }
            else {
                //checking for out of bounds
                if (addr >= limit) {
                    //anding the sr with 0xffffff1f and oring with 0x60
                    //to set the return status to 011
                    sr &= 0xffffff1f;
                    sr |= 0x60; // error set return status to 011
                    return;
                }
                //setting memory address to register destination. 
                //takes 4 clock cycles in total to execute. 
                r[rd] = mem[addr];
                clock += 3;
            }
        } else if (opcode == 1) { /* store */
            //checking out of bounds
            if (addr >= limit) {
                sr &= 0xffffff1f;
                sr |= 0x60; //vm status return 010
                return;
            }
            //set register rd to mem address
            mem[addr] = r[rd];
            clock += 3;
        } else if (opcode == 2) { /* add addi */
            //grabbing the 15 bit of the number in destination register
            //to cehck for the sign bit. 
            int sign1 = (r[rd]&0x8000)>>15;
            int sign2;
            if (i) {
                //if its immediate
                //set sign2 to have the sign bit of the constant. 
                sign2 = (constant&0x8000)>>15;
                //add the destination register with the constant.
                //and set to the detination register. 
                r[rd] = r[rd] + constant;
            } else {
                //else grab the sign bit of the source register. 
                sign2 = (r[rs]&0x8000)>>15;
                //add both the source register and the destination
                //register and set it to the destination regiser. 
                r[rd] = r[rd] + r[rs];
            }
            // set CARRY
            if (r[rd]&0x10000) {
                sr |= 01;
            }
            else {
                //why 0xfffffffe?
                sr &= 0xfffffffe;
            }
            // set OVERFLOW
            if (sign1 == sign2 and sign1 != (r[rd]&0x8000)>>15) {
                sr |= 0x10;
            }
            else {
                sr &= 0xf;
            }
            // sign extend
            if (r[rd]&0x8000) {
                r[rd] |= 0xffff0000;
            }
            else {
                r[rd] &= 0xffff;
            }
        } else if (opcode == 3) { /* addc addci */
            //if immediate
            if (i) {
                //add the status register with the constant and the 
                //destination register. 
                r[rd] = r[rd] + constant + sr&01;
            }
            else {
                //add the source register with the status register and 
                //the destination register. 
                r[rd] = r[rd] + r[rs] + sr&01;
            }
            //set carry
            if (r[rd]&0x10000) {
                sr |= 01;
            }
            else {
                sr &= 0xfffffffe;
            }
            //sign extending. 
            if (r[rd]&0x8000) {
                r[rd] |= 0xffff0000;
            }
            else {
                r[rd] &= 0xffff;
            }
        } else if (opcode == 4) { /* sub subi */
            //grabbing the sing of the 16th bit in the destination register
            int sign1 = (r[rd]&0x8000)>>15;
            int sign2;
            //if its immediate
            if (i) {
                //set sign2 to the sign bit of the constant.
                sign2 = (constant&0x8000)>>15;
                //subtract the constant from the destination register. 
                r[rd] = r[rd] - constant;
            } else {
                //else set sign2 to be the sing bit of the source register
                sign2 = (r[rs]&0x8000)>>15;
                //subtract the source register from the destination register. 
                r[rd] = r[rd] - r[rs];
            }
            // set CARRY
            if (r[rd]&0x10000) {
                sr |= 01;
            }
            else {
                sr &= 0xfffffffe;
            }
            // set OVERFLOW
            if (sign1 != sign2 and sign2 == (r[rd]&0x8000)>>15) {
                sr |= 0x10;
            }
            else {
                sr &= 0xf;
            }
            // sign extend
            if (r[rd]&0x8000) {
                r[rd] |= 0xffff0000;
            }
            else {
                r[rd] &= 0xffff;
            }
        } else if (opcode == 5) { /* subc subci */
            if (i) {
                //subtract the status register with the constant and
                //the destination register. 
                r[rd] = r[rd] - constant - sr&01;
            }
            else {
                //else do the same but with the source register and 
                //not the constant
                r[rd] = r[rd] - r[rs] - sr&01;
            }
            //set carry
            if (r[rd]&0x10000) {
                sr |= 01;
            }
            else {
                sr &= 0xfffffffe;
            }
            //sign extend
            if (r[rd]&0x8000) {
                r[rd] |= 0xffff0000;
            }
            else  {
                r[rd] &= 0xffff;
            }
        } else if (opcode == 6) { /* and andi */
            //if immediate and the constant with the destination
            //register. 
            if (i) { 
                r[rd] = r[rd] & constant;
            }
            //else and the source register with the destination register. 
            else {
                r[rd] = r[rd] & r[rs];
            }
            r[rd] &= 0xffff;
            /*
            // sign extend
            if (r[rd]&0x8000) {
                r[rd] |= 0xffff0000;
            }
            else {
                r[rd] &= 0xffff;
            }
            */
        } else if (opcode == 7) { /* xor xori */
            //if immediate, mulitply the constant with the destination register.
            if (i) {
                r[rd] = r[rd] ^ constant;
            }
            //else multiply the source register with the destination register. 
            else {
                r[rd] = r[rd] ^ r[rs];
            }
            r[rd] &= 0xffff;
            /*
            //sign extending 
            if (r[rd]&0x8000) {
                r[rd] |= 0xffff0000;
            }
            else {
                r[rd] &= 0xffff;
            }
            */
        } else if (opcode == 8) { /* compl */
            //takes the complement of the destination register. 
            r[rd] = ~r[rd];
            r[rd] &= 0xffff;
            /*
            //sign extending  
            if (r[rd]&0x8000) {
                r[rd] |= 0xffff0000;
            }
            else {
                r[rd] &= 0xffff;
            }
            */
        } else if (opcode == 9) { /* shl */
            //shifting the destination register 1 time to the left
            r[rd] <<= 1;
            //if it is negative set the status register to be 01
            if (r[rd]&0x10000) {
                sr |= 01;
            }
            //else it is still positive
            else {
                sr &= 0xfffffffe;
            }
            /*
            //sign extending
            if (r[rd]&0x8000) {
                r[rd] |= 0xffff0000;
            }
            else {
                r[rd] &= 0xffff;
            }
            */
        } else if (opcode == 10) { /* shla */
            r[rd] <<= 1;
            if (r[rd]&0x10000) {
                sr |= 01;
                //setting the 15th bit to negative if overflow
                r[rd] |= 0x8000;
            } else {
                sr &= 0xfffffffe;
                //else OR it with 0x7fff
                r[rd] &= 0x7fff;
            }
            /*
            if (r[rd]&0x8000) {
                r[rd] |= 0xffff0000;
            }
            else {
                r[rd] &= 0xffff;
            }
            */
        } else if (opcode == 11) { /* shr */
            //anding the destination register with 16 ones
            r[rd] &= 0xffff;
            //if reg destination has a 1 in the very first bit
            //set the status register to 1
            if (r[rd]&01) {
                sr |= 01;
            }
            //else it does not and set the status register 
            //to be the same 
            else {
                sr &= 0xfffffffe;
            }
            //shift destination reg 1 time to the right
            r[rd] >>= 1;
        } else if (opcode == 12) { /* shra */
            //if the destination reg has a 1 in the first position
            //then set the status register to be 1
            if (r[rd]&01) {
                sr |= 01;
            }
            else {
                sr &= 0xfffffffe;
            }
            r[rd] >>= 1;
        } else if (opcode == 13) { /* compr  compri */
            //setting status register to be anded with two zeros
            //and one 
            sr &= 0xffffffe1;
            //if the destination reg is less than constant
            //set sr to be 010
            if (i) {
                if (r[rd]<constant) {
                    sr |= 010;
                }
                //if its equal to constant,
                //sr is set to 0 4's
                if (r[rd]==constant) {
                    sr |= 04;
					cout << "hi" << endl;
                }
                //if its greater than
                //set sr to 0 two's
                if (r[rd]>constant) {
                    sr |= 02;
                }
            } else {
                //if rd is less than rs 
                //set sr to be 010
                if (r[rd]<r[rs]) {
                    sr |= 010;
                }
                //if they are equal set sr to be
                //four zeros
                if (r[rd]==r[rs]) {
                    sr |= 04;
                }
                //if its getter than 
                //set sr to be 0 twos
                if (r[rd]>r[rs]) {
                    sr |= 02;
                }
            }
        } else if (opcode == 14) { /* getstat */
            r[rd] = sr;
        } else if (opcode == 15) { /* putstat */
            sr = r[rd];
        } else if (opcode == 16) { /* jump */
            pc = addr + base;
            //will set the address to the pc to jump
            //to that call 
            //each jump will also check for out of bounds
            if (addr >= limit) {
                sr &= 0xffffff1f;
                sr |= 0x40; //out of bounds
                return;
            }
        } else if (opcode == 17) { /* jumpl */
            //if sr is 2 then pc = addr + base
            if(sr & 010) {
                pc = addr + base;
            }
            //if its less jump less than,
            //it will check to see if the sr is 
            //010 and if it is, then it will jump 
            //to that address 
            if (addr >= limit) {
                sr &= 0xffffff1f;
                sr |= 0x40; //out of bounds reference
                return;
            }
        } else if (opcode == 18) { /* jumpe */
            if (sr & 04) {
                pc = addr;
            }
            //will jump to the address if the call is equal
            if (addr >= limit) {
                sr &= 0xffffff1f;
                sr |= 0x40; //out of bounds
                return;
            }
        } else if (opcode == 19) { /* jumpg */
            if(sr & 02) {
                pc = addr + base;
            }
            //will jump to that specific address if its greater
            //than.
            if (addr >= limit) {
                sr &= 0xffffff1f;
                sr |= 0x40; //vm status return 010
                return;
            }
        } else if (opcode == 20) { /* call */
            //checking to see if address is not greater
            //or equal to limit
            if (addr >= limit) {
                sr &= 0xffffff1f;
                sr |= 0x40;
                return;
            }
            //if the stack pointer is less than limit+6
            //there is overflow
            if (sp < limit+6) {
                sr &= 0xffffff1f;
                sr |= 0x60;//vm return status 011 
                //cout << "Stack Overflow\n";
                return;
            }
            //decrement stack pointer and set pc to sp of mem
            mem[--sp] = pc;
            //saving each register in memory
            for (j=0; j<4; j++) {
                mem[--sp] = r[j];
            }
            //setting sr to stack pointer in memory after decrementing
            //stack pointer
            mem[--sp] = sr;
            pc = addr + base;
            clock += 3;
        } else if (opcode == 21) { /* return */
            //checking for stack underflow
            if (sp > 256-6) {
                cerr << "stack underflow sp = " << sp << endl;
                sr &= 0xffffff1f;
                sr |= 0x80;//vm return status 100
                //cout << "Stack Underflow\n";
                return;
            }
            //setting stack pointer to sr and incrementing
            //stack pointer
            sr = mem[sp++];
            //setting stack pointer to memory registers 
            //and incrememnting stack pointer to re-use the 
            //saved registers
            for (j=3; j>=0; j--) {
                r[j] = mem[sp++];
            }
            pc = mem[sp++];
            clock += 3;
        } else if (opcode == 22) { /* read */
            // assert r[rd] is in the allowed range
            in >> r[rd];
            clock++;
            sr &= 0xfffff01f;
            sr |= 0xc0; //read
            sr |= rd<<8; //register
            return;
        } else if (opcode == 23) { /* write */
            out << r[rd] << endl;
            clock++;
            sr &= 0xfffff01f;
            sr |= 0xe0; //write
            sr |= rd<<8; // register
            return;
        } else if (opcode == 24) { /* halt */
            //breakes from the program. 
            sr &= 0xffffff1f;
            sr |= 0x20; //vm erturns 101
            return;
        } else if (opcode == 25) { /* noop */
           //will set the address to the pc to jump
            //to that call 
            //each jump will also check for out of bounds
            if (addr >= limit) {
                sr &= 0xffffff1f;
                sr |= 0x40; //out of bounds
                return;
            }
            /* do nothing */
        } else {
            cerr << "Bad opcode = " << opcode << endl;
            sr &= 0xffffff1f;
            sr |= 0xa0; //vm returns 101
            return;
        }
        //using as a debugger
        if (debug) {
            printf("ir=%d op=%d rd=%d i=%d rs=%d const=%d addr=%d\n", ir, opcode, rd, i, rs, constant, addr);
            printf("r[0]=%d r[1]=%d r[2]=%d r[3]=%d pc=%d sr=%d sp=%d clock=%d\n\n", r[0], r[1], r[2], r[3], pc, sr, sp, clock);
            //char c;
            //cin>>c;
        }
    }
    if (debug) {
        for (j=0; j<limit; j++) {
            printf("%8d", mem[j]);
            if ((j%8)==7) printf("\n");
        }
        cout << endl;
    }
    if(pc >= base + limit) {
        sr &= 0xffffff1f; //vm return status 000
        sr |= 0x40; // out of bounds
    } else { 
        sr &= 0xffffff1f; //time slice
    }
} /* main */
