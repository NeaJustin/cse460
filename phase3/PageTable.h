#ifndef PAGETABLE_H
#define PAGETABLE_H

/*************************************************************************************
 *
 *
 *
 *
 *
*************************************************************************************/

#include <vector>

using namespace std;

//forward declarations
class OS;
class VirtualMachine;
    
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

class PageTable
{
private:
    vector<Row> page_table;
public:
    PageTable();
    PageTable(int);
    void operator=(const PageTable&);
    void operator=(PageTable &&);
    int log_to_phys(int);
    
friend 
    class OS;

friend 
    class VirtalMachine;
};

#endif
