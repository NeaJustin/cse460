#ifndef PAGETABLE_H
#define PAGETABLE_H

/*************************************************************************************
 * Richard Vargas, Justin Pulido
 * PageTable.h
 * PageTable.h will be responsible for creating the space needed for the frames to 
   be entered. When one comes in, it will convert the size to 8 words and it will
   then be placed into its pcb and loaded into the disk from there. it will also
   contain the tlb for saving the pages when it is needing to context switch.  
 * 50% complete
*************************************************************************************/

#include <vector>

using namespace std;

//forward declarations
class OS;
class VirtualMachine;
   
//defining class of row to have a frame, a valid bit
//and a modified bit that is able to share its content with 
//the os and PageTable class.  
class Row
{
private:
    int frame;
    bool valid;
    bool modified;

friend
    class PageTable;

friend 
    class OS;
};

//creating a vector of class row for the page table 
//and creating two constructors for the page table. 
class PageTable
{
private:
    vector<Row> page_table;
    //int r[32];
public:
    PageTable();                        //default
    PageTable(int);                     //constructor
    void operator=(const PageTable&);
    void operator=(PageTable &&);       
    int log_to_phys(int);               //used for grabbing physical address. 
    
friend 
    class OS;

friend 
    class VirtualMachine;
};

//creating the Table look up buffer. 
class TLB
{
private:
    vector<Row> tlb;  
public:
    TLB();
    TLB(int);
friend
    class VirtualMachine;

friend
    class OS;
};

#endif
