/*************************************************************************
 * Richar Vargas, Justin Pulido
 * September 26, 2018
 * VirtualMachine.cpp
 * This file will be holding the functions to be called in the main
   when it is time to execute while using the .h file that was created
   to get the information from it. (reword this, sounds horrible)
************************************************************************/

#include <iostream>
#include <stdio.h>
#include <string>
#include <cstdlib>

#include "VirtualMachine.h"

using namespace std;

//constructor for virtualMachine, reserves register size, and memory. 
//sets clock to 0
VirtualMachine::VirtualMachine() : registerSize(4), clock(0),memorySize(256) 
{
	memory.reserve(memorySize); 
	reg.reserve(registerSize);
}

int VirtualMachine::get_clock()
{
    return clock;
}

//takes rd, i, addr and constant. decides if the rd will be stored
//with a memory address or a constant. 
void VirtualMachine::load(int &rd, int &i, int &addr, int &constant)
{
	//if there is an i bit, add the rd with the constant
	//else add the memory address to the rd register. 
    if(i) {
        reg[rd] = constant;
    } else {
        reg[rd]=memory[addr];
    }
	//takes 4 clock cycles. 
	clock += 3;
	return;
}

//store takes the rd register and moves it into the memory address
void VirtualMachine::store(int &rd, int &addr)
{
    memory[addr] = reg[rd];
	//takes 4 clock cycles to execute.
    clock +=3;
	return;
}

//add will add both the rd and the constant or the rd and the rs. 
//it will also check for overflow of bits from 32767 to -32768.
void VirtualMachine::add(int &rd, int &i, int &rs, int &constant)
{
	//if there is an i bit, then check to see if the rd is positive.
    //and the constant is also positive. If it is, add them together 
    //and then check the 16th bit to see if it flipped. If yes,
    //set overflow. 
	if (i) {
        if(((0x8000&reg[rd]) == 0) && ((0x8000&constant) == 0))  {	
            reg[rd] += constant;
			//checking for overflow. 
            if((reg[rd] & 0x8000) == 0x8000) {
                sr |= 0x0010; //setting only overlow bit
                reg[rd] = (~reg[rd])+1; //complement         
                reg[rd] &= 0x0000ffff; //sign extend. 
                reg[rd] = -reg[rd]; 
            }
        }	
		//if rg and constant are negative, add them together
		//then check 16th bit and if flipped, set overflow.  
        else if(((0x8000 &reg[rd]) == 0x8000) && ((0x8000 & constant) == 0x8000))  {	           reg[rd]+=constant;
			//checking overflow. 
            if((reg[rd] & 0x8000) == 0) {
                sr |= 0x10; //setting overflow        
                reg[rd] = (~reg[rd]) + 1; //complement
            }
        }
        //else add normally 
        else {
            reg[rd] += constant;
        }
    }
    //adding register destination and reg source 
    else{
        if(((0x8000 & reg[rd]) == 0) && ((0x8000&reg[rs]) == 0)) {
            reg[rd] += reg[rs];
            if((reg[rd]&0x8000) == 0x8000) {
                sr |= 0x0010; //overflow
                reg[rd] = (~reg[rd]) + 1; //complement
                reg[rd] &= 0x0000ffff; //sign extend
                reg[rd] = -reg[rd];
            } 
            else if(((0x8000&reg[rd]) == 0x8000) && ((0x8000&reg[rs]) == 0x8000)) {
                reg[rd] += reg[rs];
                if((reg[rd]&0x8000) == 0) {
                    sr |= 0x10;
                    reg[rd] = (~reg[rd]) + 1;//complement
                }
            }
            else {
                reg[rd] += reg[rs];
            }
        }
    }

	//set CARRY
    if (reg[rd] & 0x10000) {
        sr |= 01;
    } else {
        reg[rd] &= 0xffff;
    }

	return;
}

//add carry uses the same algorithm above but factoring in the carry
//bit.
void VirtualMachine::addc(int &rd, int &i, int &rs, int &constant)
{
	int carryBit = sr;
	carryBit &= 0x01;
	if (i) {
		if (((0x8000 & reg[rd]) == 0) && ((0x8000 & constant) == 0)) {
			reg[rd] += constant + carryBit;
			if ((reg[rd] & 0x8000) == 0x8000) {
				sr |= 0x0010;
				reg[rd] = (~reg[rd]) + 1;
				reg[rd] &= 0x0000ffff;
				reg[rd] = -reg[rd]; //fix this
			}
		}
		else if (((0x8000 & reg[rd]) == 0x8000) && ((0x8000 & constant) == 0x8000)) {
			reg[rd] += constant + carryBit;
			if ((reg[rd] & 0x8000) == 0) {
				sr |= 0x10;
				reg[rd] = (~reg[rd]) + 1;
			}
		}
		else reg[rd] += constant;
	}
	else {
		if (((0x8000 & reg[rd]) == 0) && ((0x8000 & reg[rs]) == 0)) {
			reg[rd] += reg[rs] + carryBit;
			if ((reg[rd] & 0x8000) == 0x8000) {
				sr |= 0x0010;
				reg[rd] = (~reg[rd]) + 1;
				reg[rd] &= 0x0000ffff;
				reg[rd] = -reg[rd]; //fix this
			}
		}
		else if (((0x8000 & reg[rd]) == 0x8000) && ((0x8000 & reg[rs]) == 0x8000)) {
			reg[rd] += reg[rs] + carryBit;
			if ((reg[rd] & 0x8000) == 0) {
				sr |= 0x10;
				reg[rd] = (~reg[rd]) + 1;
			}
		}
		else reg[rd] += reg[rs];
	}
	//set CARRY
	if (reg[rd] & 0x10000) {
        sr |= 01;
    } else {
        reg[rd] &= 0xffff;
    }
	return;
} 

//similar algorith to the one above, however, checks the 
//rd to see if signed bit is 0 and constant signed bit is 1
//or if the rd signed bit is 1 and the constant is 0. 
//if none of those then add normal.
void VirtualMachine::sub(int &rd, int &i, int &rs, int &constant)
{
	if (i) {
        if(((0x8000&reg[rd] == 0x8000)) && ((0x8000&constant) == 0)) {
            reg[rd] -= constant;
            if((reg[rd]&0x8000) == 0) {
                sr |= 0x0010;
                reg[rd] &= 0x0000ffff; 
                reg[rd] = (~reg[rd]) + 1;
                reg[rd] = -reg[rd];
            }
        } 
        else if(((0x8000 &reg[rd]) == 0) && ((0x8000 & constant) == 0x8000))  {	
            reg[rd]-=constant;
            if(((reg[rd]& 0x8000) == 0x8000)) {
                sr |= 0x10;
                reg[rd] = (~reg[rd]) + 1;
                reg[rd] &= 0x0000ffff;
                reg[rd] = -reg[rd];
            }
        } else {
            reg[rd] -= constant;
        }
    }
    else {
        if(((0x8000&reg[rd]) == 0) && ((0x8000&reg[rs]) == 0)) {
            reg[rd] -= reg[rs];
            if((reg[rd]&0x8000) == 0x8000) {
                sr |= 0x0010;
                reg[rd] = (~reg[rd]) + 1;
                reg[rd] &= 0x0000ffff;
                reg[rd] = -reg[rd];
            }
        }
        else if(((0x8000&reg[rd]) == 0x8000) && ((0x8000&reg[rs]) == 0x8000)) {
            reg[rd] -= reg[rs];
            if((reg[rd]&0x8000) == 0) {
                sr |= 0x10;
                reg[rd] = (~reg[rd]) + 1;
            }
        } 
        else {
            reg[rd] -= reg[rs];
        }
    }
	//set CARRY
	if (reg[rd] & 0x10000) {
        sr |= 01;
    }
    else {
        reg[rd] &= 0xffff;
    }
	return;
}

//will have the same algorithm as the above however it is accounting
//for the carry now. 
void VirtualMachine::subc(int &rd, int &i, int &rs, int &constant)
{
    if(i) {
        if(((0x8000&reg[rd]) == 0x8000) && ((0x8000&constant) == 0)) {
            reg[rd] -= constant;
            if((0x8000&reg[rd] == 0x8000)) {
                sr |= 0x0010;
                reg[rd] &= 0x0000ffff;
                reg[rd] = (~reg[rd]) + 1;
                reg[rd] = -reg[rd];
            }
        } 
        else if(((0x8000&reg[rd]) == 0) && ((0x8000&constant) == 0x8000)) {
            reg[rd] -= constant;
            if((0x8000&reg[rd] == 0x8000)) {
                sr |= 0x10;
                reg[rd] = (~reg[rd]) + 1;
                reg[rd] &= 0x0000ffff;
                reg[rd] = -reg[rd];
            }
        }
        else {
            reg[rd] -= constant;
        }
    }
    else {
        if(((0x8000&reg[rd]) == 0) && ((0x8000&reg[rs]) == 0)) {
            reg[rd] -= reg[rs];
            if((0x8000&reg[rd]) == 0x8000) {
                sr |= 0x0010;
                reg[rd] = (~reg[rd]) + 1;
                reg[rd] &= 0x0000ffff;
                reg[rd] = -reg[rd];
            }
        } 
        else if(((0x8000&reg[rd]) == 0x8000) && ((0x8000&reg[rs]) == 0x8000)) {
            reg[rd] -= reg[rs];
            if((0x8000&reg[rd]) == 0) {
                sr |= 0x10;
                reg[rd] = (~reg[rd]) + 1;
            }
        }
        else {
            reg[rd] -= reg[rs];
        }
    }
    //set carry
    if(reg[rd] & 0x10000) {
        sr |= 01;
    } else {
        reg[rd] &= 0xffff;
    }
	return;
}

//_and will set the rd with constant after i bit is set. Otherwise
//it ands the rd with the rs
void VirtualMachine::_and(int &rd, int &i, int &rs, int &constant)
{
	if (i) {
		reg[rd] = ((reg[rd] & constant)&0x0000ffff);
	}
	else {
		reg[rd] = (reg[rd] & reg[rs]);
	}
	return;
}

//xor will and the rd with the constant if the i bit is set. 
//otherwise it will and the rs and the rd together. 
void VirtualMachine::_xor(int &rd, int &i, int &rs, int &constant)
{
	if (i) {
        reg[rd] = (reg[rd] ^ (constant & 0x0000ffff));
    } else {
         reg[rd] = (reg[rd] ^ reg[rs]);
    }
	return;
}

//complement will take the complement of rd and 
//make shorten the number to a 16 bit number
void VirtualMachine::complement(int &rd)
{
	reg[rd] = (~reg[rd] & 0x0000ffff);
	return;
}

//shl shifts all the bits to the lft. 
//sets overflow if a sign bit is changed,
//then sets carry if register destination has 17th bit.
void VirtualMachine::shl(int &rd)
{
    if((reg[rd]&0x00008000) == 0) {
        reg[rd] = reg[rd]<<1;
        if((reg[rd]&0x00008000) != 0) {
            sr |= 0x10;
        } else {
            reg[rd] &= 0x0000ffff;
        }
    }
    else if((reg[rd]&0x000080000) == 0x00008000) {
        reg[rd] = reg[rd]<<1;
        if((reg[rd]&0x00008000) != 0x00008000) {
            sr |= 0x10;
        } else {
            reg[rd] &= 0x0000ffff;
        }
    }
    //set carry
	if (reg[rd] & 0x00010000) {
		sr |= 01;
	} else {
        reg[rd] &= 0xffff;
    }
	return;
}

//same algorithm as shl and checks sign of number and 
//does not shift sign bit. 
void VirtualMachine::shla(int &rd)
{
    if((reg[rd]&0x000080000) == 0) {
        reg[rd] = reg[rd]<<1;
        if((reg[rd]&0x00008000) != 0) {
            sr |= 0x10;
        } else {
            reg[rd] &= 0x00007fff;
        }
    }
    else if((reg[rd]&0x00008000) == 0x00008000) {
        reg[rd] = reg[rd]<<1;
        if((reg[rd]&0x00008000) != 0x00008000) {
            sr |= 0x10;
        } else {
            reg[rd] &= 0x00007fff;
        }
    }
    //set carry
    if(reg[rd]&0x10000) {
        sr |= 01;
    } else {
        reg[rd] &= 0xffff;
    }
	return;
}

//shr same algorithm as shl 
//but the bits are shifted in the different dirrection. 
void VirtualMachine::shr(int &rd)
{
    if((reg[rd]&0x00008000) == 0) {
        reg[rd] = reg[rd]>>1;
        if((reg[rd]&0x00008000) != 0) {
            sr |= 0x10;
        } else {
            reg[rd] &= 0x0000ffff;
        }
    }
    else if((reg[rd]&0x00008000) == 0x00008000) {
        reg[rd] = reg[rd]>>1;
        if((reg[rd]&0x00008000) != 0x00008000) {
            sr |= 0x10;
        } else {
            reg[rd] &= 0x0000ffff;
        }
    }

    //set carry
    if(reg[rd]&0x10000) {
        sr |= 01;
    } else {
        reg[rd] &= 0xffff;
    }
    
	return;
}

//shra is using the same algorithm as shr 
//but it checks the sign of number and does not shift 
//sign bit
void VirtualMachine::shra(int &rd)
{
    if((reg[rd]&0x00008000) == 0) {
        reg[rd] = reg[rd]>>1;
        if((reg[rd]&0x00008000) != 0) {
            sr |= 0x10;
        } else {
            reg[rd] &= 0x00007fff;
        }
    }
    else if((reg[rd]&0x00008000) == 0x00008000) {
        reg[rd] = reg[rd]>>1;
        if((reg[rd]&0x00008000) != 0x00008000) {
            sr |= 0x10;
        } else {
            reg[rd] &= 0x00007fff;
        }
    }
    //set carry
    if(reg[rd]&0x10000) {
        sr |= 01;
    } else {
        reg[rd] & 0xffff;
    }
	return;
}

//compr will compare the rd if its less than, equal to, or greater
//than the constant and sets sr. If its not i bit, then  it 
//coompares the RD with the SR to check if greater than, 
//less than, or equal to. 
void VirtualMachine::compr(int &rd, int &i, int &rs, int &constant)
{
	if (i) {
		if (reg[rd] < constant) {
			sr |= 0x19;
			sr &= ~(3 << 1);
		} else if (reg[rd] > constant){
			sr |= 0x13;
			sr &= ~(3 << 2);
		} else {
			sr |= 0x15;
			sr &= ~(5 << 1);
		}
	} else {
		if (reg[rd] < reg[rs]) {
			sr |= 0x19;
			sr &= ~(3 << 1);
	    } else if (reg[rd] > reg[rs]) {
			sr |= 0x13;
			sr &= ~(3 << 2);
		} else {
			sr |= 0x15;
			sr &= ~(5 << 1);
		}
	}

	return;
}

//puts sr into rd
void VirtualMachine::get_stat(int &rd)
{
	reg[rd] = sr;
	return;
}

//putrs rd into sr
void VirtualMachine::put_stat(int &rd)
{
	sr = reg[rd];
	return;
}

//the next instrunction is set to addr
void VirtualMachine::jump(int &addr)
{
	pc = addr;
	return;
}

//next instrunction is set to addr
//if less than status set
void VirtualMachine::jumpl(int &addr)
{
	if ((sr & 0x8) == 0x8) { 
        pc = addr;
    }
	return;
}

//next instrunction is set to addr
//if equal status set
void VirtualMachine::jumpe(int &addr)
{
	if ((sr & 0x4) == 0x4) {
        pc = addr;
    }
	return;
}

//next instrunction set to addr
//if greater than status set. 
void VirtualMachine::jumpg(int &addr)
{
	if ((sr & 0x2) == 0x2)  {
        pc = addr;
    }
	return;
}

//call will decrememnt stack pointer and store
//pc, sr, and general registers into memory. 
void VirtualMachine::call(int &addr)
{
	sp--;
	memory[sp] = pc;
	sp--;
	memory[sp] = sr;
	sp--;

	for (int i = 0; i < 4; i++){
		memory[sp] = reg[i];
		sp--;
	}
	pc = addr;
	clock += 3;
	return;
}

//return increments the stack pointer and puts
//pc, sr, and general registers into registers. 
void VirtualMachine::_return()
{
	sp++;
	for (int i = 3; i >= 0; i--) {
		reg[i] = memory[sp];
		sp++;
	}

	sr = memory[sp];
	sp++;
	pc = memory[sp];
	sp++;
	clock += 3;
	return;
}

//read takes the in file and puts it into the rd 
void VirtualMachine::read(int &rd, fstream& in)
{
	in >> reg[rd];
	clock += 27;
	return;
}

//write takes the rd and writes it out to the 
//outfile. 
void VirtualMachine::write(int &rd, fstream& out)
{
	out << reg[rd];
	clock += 27;
	return;
}

//does nothing for memory size. 
void VirtualMachine::noop()
{
	return;
}

//run is the main portion of the program called from os and seperates
//the object code into seperate integers and decides using a switch case
//to decide which function to execute. 
void VirtualMachine::run(fstream& objectCode, fstream& in, fstream& out)
{
	const int debug = false;
	int opcode, rd, i, rs, constant, addr, j;
	base = 0;
	for (limit = 0; objectCode >> memory[limit]; limit++);

	sr = 0;
	sp = memorySize;
	pc = 0;
	while (pc < limit) 
    {
		ir = memory[pc];
		pc++;
		opcode = (ir & 0xf800) >> 11;
		rd = (ir & 0x600) >> 9;
		i = (ir & 0x100) >> 8;
		rs = (ir & 0xc0) >> 6;
		addr = ir & 0xff;
		constant = addr;


		//Negative sign extend.
		if (ir & 0x80) {
			constant |= 0xffffff00;
		}
		clock++;

		//Build switch of functions here, dependent on opcode.
		switch (opcode) 
        {
		    case 0:
			    load(rd, i, addr, constant);
			    break;
		    case 1:
			    store(rd, addr);
			    break;
		    case 2:
			    add(rd, i, rs, constant);
			    break;
		    case 3:
			    addc(rd, i, rs, constant);
			    break;
		    case 4:
		    	sub(rd, i, rs, constant);
			    break;
		    case 5:
		    	subc(rd, i, rs, constant);
			    break;
		    case 6:
			    _and(rd, i, rs, constant);
			    break;
		    case 7:
		    	_xor(rd, i, rs, constant);
			    break;
		    case 8:
		    	complement(rd);
		    	break;
		    case 9:
		    	shl(rd);
			    break;
		    case 10:
			    shla(rd);
		    	break;
		    case 11:
		    	shr(rd);
		    	break;
		    case 12:
			    shra(rd);
			    break;
		    case 13:
			    compr(rd, i, rs, constant);
			    break;
		    case 14:
		    	get_stat(rd);
		    	break;
		    case 15:
		    	put_stat(rd);
		    	break;
		    case 16:
		    	jump(addr);
		    	break;
		    case 17:
		    	jumpl(addr);
		    	break;
		    case 18:
		    	jumpe(addr);
		    	break;
		    case 19:
		    	jumpg(addr);
		    	break;
		    case 20:
		    	call(addr);
		    	break;
		    case 21:
		    	_return();
		    	break;
		    case 22:
		    	read(rd, in);
		    	break;
		    case 23:
		    	write(rd, out);
		    	break;
		    case 25:
		    	noop();
		    	break;
	    	default:
			//Will end the program if hits halt.
            if (debug) {
				printf("ir=%d op=%d rd=%d i=%d rs=%d const=%d addr=%d\n", ir, opcode, rd, i, rs, constant, addr);
			    printf("reg[0]=%d reg[1]=%d reg[2]=%d reg[3]=%d pc=%d sr=%d sp=%d clock=%d\n\n", reg[0], reg[1], reg[2], reg[3], pc, sr, sp, clock);
				//char c;
				//cin>>c;
			}
			if (debug) {
			    printf("ir=%d op=%d rd=%d i=%d rs=%d const=%d addr=%d\n", ir, opcode, rd, i, rs, constant, addr);
			    printf("reg[0]=%d reg[1]=%d reg[2]=%d reg[3]=%d pc=%d sr=%d sp=%d clock=%d\n\n", reg[0], reg[1], reg[2], reg[3], pc, sr, sp, clock);
		    }
		    if (debug) {
				printf("ir=%d op=%d rd=%d i=%d rs=%d const=%d addr=%d\n", ir, opcode, rd, i, rs, constant, addr);
				printf("reg[0]=%d reg[1]=%d reg[2]=%d reg[3]=%d pc=%d sr=%d sp=%d clock=%d\n\n", reg[0], reg[1], reg[2], reg[3], pc, sr, sp, clock);
				//char c;
				//cin>>c;
		    }
        }
    }
    if (debug) {
	    for (j = 0; j < limit; j++) {
	        printf("%8d", memory[j]);
            if ((j % 8) == 7) printf("\n");
	    }
        cout << endl;
    }
}


