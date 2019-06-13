/************************************************************************************
 * Richard Vargas, Justin Pulido
 * PageTable.cpp
 * The page table will create a vector of row size and be responsible for changing
   the valid and modified bits of the page that is being brought it. It will also
   have an associated page table that will contain it. 
***********************************************************************************/

#include "PageTable.h"

//default constructor
PageTable::PageTable() { }

/**********************************************************************************
 * setting the page table to be a vector of type row and grabbing the size of the
   page length. Then setting each indecy in the page table row to be false 
**********************************************************************************/
PageTable::PageTable(int size) 
{
    page_table = vector<Row>(size);
    
    //changing the valid and invalid bits to false 
    for(int i = 0; i < size; i++) {
        page_table[i].valid = false;
        page_table[i].modified = false;
    }
}

/**********************************************************************************
 * Setting the page table to have the page that is coming in throgh the vector
   by calling the page table which is set to be a vector in the .h file. 
*********************************************************************************/ 
void PageTable::operator=(const PageTable & p)
{
    page_table = p.page_table;
}

/********************************************************************************
 * swapping the page table with the page table that is being brought in. 
********************************************************************************/
void PageTable::operator=(PageTable && p )
{
    swap(page_table, p.page_table);
}

/*********************************************************************************
 * We are grabbing the logical address of the page, then we are grabbing the 
   frame number and the address of it and setting it to a variable to 
   get the phsyical address of it in memory. And if it does not find it, then
   it causes a page fault and needs to be handled by the OS and brings in the 
   page from the disk. 
*********************************************************************************/
int PageTable::log_to_phys(int addr)
{
    //assuming the pages are 8 words
    int page_num = addr/8;
	//checking to see if the page is valid
    if(page_table[page_num].valid) {
		//grabbing the frame number of the current page
        int frame = page_table[page_num].frame;
		//grabbing the frame number and the offset
		//then adding them together to obtain the phsycal
        //address. 
        int phys_addr = (frame*9) + (addr%8);
        return phys_addr;
    }
    //causing a page fault 
    else {
        return -1;
    }
}
