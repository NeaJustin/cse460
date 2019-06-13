/**************************************************************************
 * Richard Vargas, Justin Pulido 
 * October 17, 2018
 * Assembler.cpp
 * The program will create a map with the constructor and be able to check
   the input with the map and see if it is in the instrunction set. From 
   the input, it will grab the rs, rd, or constant and perform a set of 
   operations to obtain the opcode that is needed for the virtual machine
   to properly execute the commands in the .s file. 
 * 100% complete 
**************************************************************************/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>

#include "Assembler.h"

//creating a map within the constructor to be called in main
//it will be executded to have the strings to get the opcodes 
Assembler::Assembler()
{
    jumpTable["load"] = &Assembler::load;
    jumpTable["loadi"] = &Assembler::loadi;
    jumpTable["store"] = &Assembler::store;
    jumpTable["add"] = &Assembler::add;
    jumpTable["addi"] = &Assembler::addi;
    jumpTable["addc"] = &Assembler::addc;
    jumpTable["addci"] = &Assembler::addci;
    jumpTable["sub"] = &Assembler::sub;
    jumpTable["subi"] = &Assembler::subi;
    jumpTable["subc"] = &Assembler::subc;
    jumpTable["subci"] = &Assembler::subci;
    jumpTable["and"] = &Assembler::and_instr;
    jumpTable["andi"] = &Assembler::andi;
    jumpTable["xor"] = &Assembler::xor_instr;
    jumpTable["xori"] = &Assembler::xori;
    jumpTable["compl"] = &Assembler::compl_instr;
    jumpTable["shl"] = &Assembler::shl;
    jumpTable["shla"] = &Assembler::shla;
    jumpTable["shr"] = &Assembler::shr;
    jumpTable["shra"] = &Assembler::shra;
    jumpTable["compr"] = &Assembler::compr;
    jumpTable["compri"] = &Assembler::compri;
    jumpTable["getstat"] = &Assembler::getstat;
    jumpTable["putstat"] = &Assembler::putstat;
    jumpTable["jump"] = &Assembler::jump;
    jumpTable["jumpl"] = &Assembler::jumpl;
    jumpTable["jumpe"] = &Assembler::jumpe;
    jumpTable["jumpg"] = &Assembler::jumpg;
    jumpTable["call"] = &Assembler::call;
    jumpTable["return"] = &Assembler::return_instr;
    jumpTable["read"] = &Assembler::read_instr;
    jumpTable["write"] = &Assembler::write_instr;
    jumpTable["halt"] = &Assembler::halt;
    jumpTable["noop"] = &Assembler::noop;
}

//creating an constructor that has both file stream
//in and output that will take in a file and write out
//the file as well. 
int Assembler::assemble(fstream& in, fstream& out)
{
    string line;
    string opcode;
    const int success = false;
    const int error = true;
    //const int debug = false;
    int instruction;

	//grabbing the line of the date being put in. 
    getline(in, line);
    while (!in.eof()) {
		//converting to string of characters
        istringstream str(line.c_str());
		//putting opcode into str
        str >> opcode;
		//checking to see if the opcode is ! and if it is
		//then grab the next character and continue by bypassing
		//the rest of the exceptions and going back to the beginning
		//of the loop
        if (opcode[0] == '!') {
            getline(in, line);
            continue;
        }
		
		//using a try to catch any runtime errors within the program. 
        try {
			//if the opcode is not within the map, then throw an
			//exception to the user. Else we derefference the map
			//and set this->towards and pass string to the instrunction
			//set and set it equal to instrunctions. 
            if (not jumpTable[opcode] )
                throw NullPointerException();
            else instruction = (this->*jumpTable[opcode])(str);
		//will catch the exception from the previous catch and then print out
		//the error to the useer. 
        } catch (NullPointerException e) {
            cerr << e.what() << endl;
            return error;
        }

		//set instrunction into out and display it.
        if (instruction == -1)
            return error;
        out << instruction << endl;
		//then grab the next line to repeat the process again 
		//until the end of file. 
        getline(in, line);
    }
    return success;
} // assemble

/*********************************************************
 *load will will set the rd and addr to str and then 
  OR them together and bit shift them to the appropiate
  positions to obtain the object code.  
*********************************************************/
int Assembler::load(istringstream & str)
{
    int rd, addr;
    str >> rd >> addr;
    if (rd < 0 || rd > 3)
        return -1;
    if (addr < 0 || addr > 255)
        return -1;
    int inst=0;
    inst = inst<<11 | rd<<9 | addr;
    return inst;
}

/******************************************************
 *creates a destination register and a constant 
 *then passes a str to read the contents of both rd and
 *constant. If rd is not 0 and 3, or between those two
 *numbers, then exit. Checking constant as well to 
 *make sure its a valid number. 
******************************************************/
int Assembler::loadi(istringstream & str)
{
    int rd, constant;
    str >> rd >> constant;
    if (rd < 0 || rd > 3)
        return -1;
    if (constant < -128 || constant > 127)
        return -1;
    int inst=0;
    //anding constant with 0x000000ff to change it to be 
	//8 bits and the keeping the constant the same through
	//out by making sure to not disturb it if its a negative. 
	//then bit shift 1 to the left 8 times for 
	//I bit, rd to left and inst to the left 11. 
    inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
    return inst;
}

/*******************************************************
 *Store will be taking care of saving it to memory by 
 *having rd and addr passed into str. Then shift rd to
 *the left 9 times and inst to the left 11 times. Then
 *OR each one and set to inst. 
*******************************************************/
int Assembler::store(istringstream & str)
{
    int rd, addr;
    str >> rd >> addr;
    if (rd < 0 || rd > 3)
        return -1;
    if (addr < 0 || addr > 255)
        return -1;
    int inst=1;
    //oring with i bit shifting it 8 times over. 
    inst = inst<<11 | rd<<9 | 1<<8 | addr;
    return inst;
}

/*******************************************************
 *will check to make sure the registers are within 
 *range and it will then shift rs 6 times to the left
 *and shift rd to the left 9 times then OR each with
 *inst that is shifted 11 times to get the opcode.  
*******************************************************/
int Assembler::add(istringstream & str)
{
    int rd, rs;
    str >> rd >> rs;
    if (rd < 0 || rd > 3)
        return -1;
    if (rs < 0 || rs > 3)
        return -1;
    int inst=2;
    //shift rs, rd and inst to grab their binary
    //numbers and then Or them together to grab 
    //the opcode 
    inst = inst<<11 | rd<<9 | rs<<6;
    return inst;
}

/*******************************************************
 *add imediate will check the constants and the
 *registers to make sure they are within range of their
 *boundaries. Then we will grab the constant and 
 *OR it with the I bit and rd and set it to inst 
 *to return the opcode. 
*******************************************************/
int Assembler::addi(istringstream & str)
{
    int rd, constant;
    str >> rd >> constant;
    if (rd < 0 || rd > 3)
        return -1;
    if (constant < -128 || constant > 127)
        return -1;
    int inst=2;
    inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
    return inst;
}


/*******************************************************
 *add carry will check the boundaries of the registers
 *to see if they match up. Then we will grab the carry
 *and add it to the source register and OR it with the 
 *rs and rd registers and move the inst over 11 times 
 *to receive the opcode from the instant. 
*******************************************************/
int Assembler::addc(istringstream & str)
{
    int rd, rs;
    str >> rd >> rs;
    //if destination register is less than 0 or greater than 
    //3, then dont run.
    if (rd < 0 || rd > 3)
        return -1;
    //if source register is less than 0 but greater than 3 
    //dont run and exit. 
    if (rs < 0 || rs > 3)
        return -1;
    int inst=3;
    inst = inst<<11 | rd<<9 | rs<<6;
    return inst;
}

/***************************************************************
 *add carry imediate will grab the carry and the constant 
  and add it to the destination register to then grab the
  opcode for the instrunction. 
***************************************************************/
int Assembler::addci(istringstream & str)
{
    int rd, constant;
    str >> rd >> constant;
    if (rd < 0 || rd > 3)
        return -1;
    if (constant < -128 || constant > 127)
        return -1;
    int inst=3;
    //adding 16 1's to the constant and the carry to get both into the correct
    //position when bitshifting. 
    //then shifting the inst by 11 to get the op code that is in the bits of 
    //15-11. Using I bit for the imidiate
    inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
    return inst;
}

/************************************************************
 *subtract will take the source register and the destination
  register and OR them together with the instant to grab
  the opcode for the operation.
************************************************************/
int Assembler::sub(istringstream & str)
{
    int rd, rs;
    str >> rd >> rs;
    if (rd < 0 || rd > 3)
        return -1;
    if (rs < 0 || rs > 3)
        return -1;
    int inst=4;
    //bitshifiting the source register and the destination register
    //to obtain the values in the ocrrect spot. Then move the 
    //inst over 11 bits to recieve the opcode 
    inst = inst<<11 | rd<<9 | rs<<6;
    return inst;
}

/************************************************************
 *subi will set the i bit to 1 and then OR it with the 
  constant, and rd with the instant that has a set number
  then shift inst, rd, and i and anding the constant with
  0x000000ff to return the opcode 
************************************************************/
int Assembler::subi(istringstream & str)
{
    int rd, constant;
    str >> rd >> constant;
    if (rd < 0 || rd > 3)
        return -1;
    if (constant < -128 || constant > 127)
        return -1;
    int inst=4;
    inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
    return inst;
}

/***************************************************************************
 *subc will take the rd and rs and set them to the str to be read. then 
  it will bitshift them both and or them to an instant with a set number
  that is also shifted to obtain the opcode. 
***************************************************************************/
int Assembler::subc(istringstream & str)
{
    int rd, rs;
    str >> rd >> rs;
    if (rd < 0 || rd > 3)
        return -1;
    if (rs < 0 || rs > 3)
        return -1;
    int inst=5;
    inst = inst<<11 | rd<<9 | rs<<6;
    return inst;
}

/************************************************************
 *subtract carry imediate will get from the destination 
  register, the constant and the carry and it will then 
  shift each bit over and add it to inst where that will
  be shifted over to grab the opcode.
************************************************************/
int Assembler::subci(istringstream & str)
{
    int rd, constant;
    str >> rd >> constant;
    if (rd < 0 || rd > 3)
        return -1;
    if (constant < -128 || constant > 127)
        return -1;
    int inst=5;
    //shifting the inst, rd, i bit, and grabbing the first 16 bits 
    //of constant then OR them each. 
    inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
    return inst;
}

/*************************************************************
 *_and grabs the rd and rs sets them to the str. Then
  bitshifts them each and sets an inst that is also shifted
  then OR each bit to grab the opcode. 
*************************************************************/
int Assembler::and_instr(istringstream & str)
{
    int rd, rs;
    str >> rd >> rs;
    if (rd < 0 || rd > 3)
        return -1;
    if (rs < 0 || rs > 3)
        return -1;
    int inst=6;
    //having inst set equal to rd anded with rs 
    //then bitshift 11 times to obtain the opcode from the
    //process
    inst = inst<<11 | rd<<9 | rs<<6;
    return inst;
}

/************************************************************
 *andi takes in a i bit and a constant instead of the rs 
  and shifts the i bit and grabs the first 16 bits of the 
  constant to return the opcode.  
************************************************************/
int Assembler::andi(istringstream & str)
{
    int rd, constant;
    str >> rd >> constant;
    if (rd < 0 || rd > 3)
        return -1;
    if (constant < -128 || constant > 127)
        return -1;
    int inst=6;
    inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
    return inst;
}

/**********************************************************
 *_xor grabs the rd and rs and set it to str. Then sets
  inst to 7 and shifts each rs and rd and inst. Then 
  ORs them together and sets back to instant to grab 
  the opcode.  
**********************************************************/
int Assembler::xor_instr(istringstream & str)
{
    int rd, rs;
    str >> rd >> rs;
    if (rd < 0 || rd > 3)
        return -1;
    if (rs < 0 || rs > 3)
        return -1;
    int inst=7;
    inst = inst<<11 | rd<<9 | rs<<6;
    return inst;
}

/***********************************************************************
 *xori takes the i bit and the constant instead of the rs register. 
  Then does the same procedure with bitshifting to grab the correct
  values and or them together to obtain the opcode needed for the 
  operation.  
***********************************************************************/
int Assembler::xori(istringstream & str)
{
    int rd, constant;
    str >> rd >> constant;
    if (rd < 0 || rd > 3)
        return -1;
    if (constant < -128 || constant > 127)
        return -1;
    int inst=7;
    inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
    return inst;
}

/***********************************************************************
 *complement will take the destination register and set it to the str
  then it set an inst that is then shifted along with the rd and they
  are ORed together to grab the opcode. 
***********************************************************************/
int Assembler::compl_instr(istringstream & str)
{
    int rd;
    str >> rd;
    if (rd < 0 || rd > 3)
        return -1;
    int inst=8;
    inst = inst<<11 | rd<<9;
    return inst;
}

/***********************************************************************
 *shl grabls the rd and set it to str. Then sets inst to 9 and 
  shifts both the rd and inst the appropiate amount of times and then 
  OR them together to grab the correct opcode.   
***********************************************************************/
int Assembler::shl(istringstream & str)
{
    int rd;
    str >> rd;
    if (rd < 0 || rd > 3)
        return -1;
    int inst=9;
    inst = inst<<11 | rd<<9;
    return inst;
}

/*********************************************************************
 *shla grabs the rd and does the same procedure as the shl except 
  with a different number set for the instant.    
*********************************************************************/
int Assembler::shla(istringstream & str)
{
    int rd;
    str >> rd;
    if (rd < 0 || rd > 3)
        return -1;
    int inst=10;
    inst = inst<<11 | rd<<9;
    return inst;
}

/********************************************************************
 *shr will do the same procedure as shl except it has a different 
  number set for the inst and still returns the opcode for 
  further instrunctions.  
********************************************************************/
int Assembler::shr(istringstream & str)
{
    int rd;
    str >> rd;
    if (rd < 0 || rd > 3)
        return -1;
    int inst=11;
    inst = inst<<11 | rd<<9;
    return inst;
}

/*******************************************************************
 *shra will grab the rd and set it to str and then set inst to 12 
  this then shifts both rd and inst and ORs them together to obtain
  the opcode.  
*******************************************************************/
int Assembler::shra(istringstream & str)
{
    int rd;
    str >> rd;
    if (rd < 0 || rd > 3)
        return -1;
    int inst=12;
    inst = inst<<11 | rd<<9;
    return inst;
}

/*******************************************************************
 *compr will grab the rd and rs and check their boundaries. Then
  sets an inst that is shifted along with the rd and rs and ORed 
  together to grab the opcode after being set to inst.  
*******************************************************************/
int Assembler::compr(istringstream & str)
{
    int rd, rs;
    str >> rd >> rs;
    if (rd < 0 || rd > 3)
        return -1;
    if (rs < 0 || rs > 3)
        return -1;
    int inst=13;
    inst = inst<<11 | rd<<9 | rs<<6;
    return inst;
}

/*****************************************************************
 *compri will take the rd and constant and set them to the str
  then it will check to make sure they are within the range 
  they should be in and OR them with the I bit, and set it to 
  the inst to retrieve the opcode. 
*****************************************************************/
int Assembler::compri(istringstream & str)
{
    int rd, constant;
    str >> rd >> constant;
    if (rd < 0 || rd > 3)
        return -1;
    if (constant < -128 || constant > 127)
        return -1;
    int inst=13;
    inst = inst<<11 | rd<<9 | 1<<8 | (0x000000ff & constant);
    return inst;
}

/****************************************************************
 *get_status will grab the rd and set it to the rd and then 
  set inst to 14 and shift both rd and inst and OR them together
  to obtain the opcode.  
****************************************************************/
int Assembler::getstat(istringstream & str)
{
    int rd;
    str >> rd;
    if (rd < 0 || rd > 3)
        return -1;
    int inst=14;
    inst = inst<<11 | rd<<9;
    return inst;
}

/****************************************************************
 *the put_stat takes the rd and sets it to string and checks it. 
  then it sets inst to 15 and shifts both rd and inst and sets 
  it back to inst to get the opcode. 
****************************************************************/
int Assembler::putstat(istringstream & str)
{
    int rd;
    str >> rd;
    if (rd < 0 || rd > 3)
        return -1;
    int inst=15;
    inst = inst<<11 | rd<<9;
    return inst;
}

/***************************************************************
 *jump takes the address and checks to see if it is within 
  range. Then sets an inst and shifts the inst and ORs it with
  the address to obtain the opcode. 
***************************************************************/
int Assembler::jump(istringstream & str)
{
    int addr;
    str >> addr;
    if (addr < 0 || addr > 255)
        return -1;
    int inst=16;
    inst = inst<<11 | 1<<8 | addr;
    return inst;
}

/**************************************************************
 *jumpl grabs the address and checks for the i bit. Then shifts
  the I bit and the inst that is set. Each are then ORed 
  toether with the address and set to inst to grab the opcode 
**************************************************************/
int Assembler::jumpl(istringstream & str)
{
    int addr;
    str >> addr;
    if (addr < 0 || addr > 255)
        return -1;
    int inst=17;
    inst = inst<<11 | 1<<8 |addr;
    return inst;
}

/*************************************************************
 *jumpe grabs the address and checks it and sets an instant.
  then the inst, I bit, is shifted and ORed with the address
  and set to the instant. 
*************************************************************/
int Assembler::jumpe(istringstream & str)
{
    int addr;
    str >> addr;
    if (addr < 0 || addr > 255)
        return -1;
    int inst=18;
    inst = inst<<11 | 1<<8 |addr;
    return inst;
}

/*************************************************************
 *jumpg will do the same operations as the jumpe except with
  a higher inst value.  
*************************************************************/
int Assembler::jumpg(istringstream & str)
{
    int addr;
    str >> addr;
    if (addr < 0 || addr > 255)
        return -1;
    int inst=19;
    inst = inst<<11 | 1<<8 |addr;
    return inst;
}

/***********************************************************
 *call grabs address and sets the inst. Then shifts both 
  I bit and the instant and ORs it with the address 
  to obtain the opcode. 
***********************************************************/
int Assembler::call(istringstream & str)
{
    int addr;
    str >> addr;
    if (addr < 0 || addr > 255)
        return -1;
    int inst=20;
    inst = inst<<11 | 1<<8 |addr;
    return inst;
}

/**********************************************************
 *return just returns the value of the opcode after 
  bitshifting the inst.  
**********************************************************/
int Assembler::return_instr(istringstream & str)
{
    int inst=21;
    inst = inst<<11;
    return inst;
}

/**********************************************************
 *read grabs the rd and checks it. Then sets an inst and
  shifts both the rd and the inst and will set it back to 
  the instant to grab the opcode. 
**********************************************************/
int Assembler::read_instr(istringstream & str)
{
    int rd;
    str >> rd;
    if (rd < 0 || rd > 3)
        return -1;
    int inst=22;
    inst = inst<<11 | rd<<9;
    return inst;
}

/*********************************************************
 *write grabs rd and set an inst. Then shifts both and 
  ORs them together and it is then set back to instant to 
  obtain the opcode. 
*********************************************************/
int Assembler::write_instr(istringstream & str)
{
    int rd;
    str >> rd;
    if (rd < 0 || rd > 3)
        return -1;
    int inst=23;
    inst = inst<<11 | rd<<9;
    return inst;
}

/*********************************************************
 *halt sets instant and shifts the bits and sets it to 
  the inst to get the opcode.  
*********************************************************/
int Assembler::halt(istringstream & str)
{
    int inst=24;
    inst = inst<<11;
    return inst;
}

/********************************************************
 *noop will grab the inst and shift it and set it back
  to inst to grab the opcode.  
********************************************************/
int Assembler::noop(istringstream & str)
{
    int inst=25;
    inst = inst<<11;
    return inst;
}

