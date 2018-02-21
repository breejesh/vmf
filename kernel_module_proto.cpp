#include <iostream>
#include <conio.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <iomanip>

/*#define mainfunc

#include "AddrTranslationProto.cpp"
*/
using namespace std;


struct kernel_module
{
    uint64_t address;
    uint64_t block_size;
    uint16_t name_size;
    uint64_t name_addr;
    char *name;
    kernel_module *next;
};

int compare_array(char arr1[], char arr2[], int n)
{
    int i;
    for(i = 0; i < n; i++)
        if(arr1[i] != arr2[i])
            return 0;
    return 1;
}

int scan_tag(char arr1[], char arr2[], int n)
{
    int i;
    for(i = 4; i < n; i++)
        if(arr1[i] != arr2[i])
            return 0;
    return 1;
}

char* getUnicodeStr(char uniStr[], int n)
{
    char *str = new char[n/2 + 1];
    for(int i = 0; i < n; i+=2)
    {
        str[i / 2] = uniStr[i];
    }
    str[n/2] = '0';
    return str;
}

int main()
{
	ifstream ifile;
	char s[100], fname[] = "data/win764.vmem";

    char pool_tag[] = "0000MmLd";

	int counter = 0;
    char found_pattern[8], magic_process_pattern[8] = {3, 0, 88, 0, 0, 0, 0, 0};
    char k_name[16];
    char process_billed[8];

    unsigned int pid, ppid;
    int end_flag = 0;
    int i, temp;
    long long addr_val = 0;
	ifile.open(fname, ios::in | ios::binary);
    if(!ifile)
	{
		cout<<"Error in opening file..!!";
		exit(0);
	}
	cout<<"File opened..";
	cout<<"\n";
    
    kernel_module *modules = new kernel_module;
    modules->next = NULL;
    kernel_module *last_module = modules;

    uint64_t prev_size, pool_index, block_size, pool_type;
    uint64_t name_addr, phy_name_addr;
    uint16_t name_size = 0x14;

    while(ifile.eof()==0)
	{
        ifile.read(found_pattern, 8);
        addr_val += 8;
        if(scan_tag(pool_tag, found_pattern, 8))
        {
            prev_size = found_pattern[0] * 16;
            pool_index = found_pattern[1];
            block_size = found_pattern[2] * 16;     //16 for 64bits and 8 for 32bits OS
            pool_type = found_pattern[3];

            if(block_size <= 32)
            {
                ifile.ignore(8);
                addr_val += 8;
                continue;
            }

            kernel_module *curr_module = new kernel_module;
            
            curr_module->next = NULL;
            curr_module->address = addr_val - 8;
            curr_module->block_size = block_size;

            cout<<hex<<curr_module->address<<endl;

            ifile.ignore(96);                       //0x68 for size of name
            addr_val += 96;
            ifile.read(reinterpret_cast<char *>(&name_size), sizeof(name_size));
            addr_val += 2;
            ifile.ignore(6);
            addr_val += 6;

            curr_module->name_size = name_size;

            ifile.read(reinterpret_cast<char *>(&name_addr), sizeof(name_addr));
            addr_val += 8;
            phy_name_addr = (addr_val & 0xfffffffffffff000) | (name_addr & 0x0fff);
            
            curr_module->name_addr = phy_name_addr;
            
            /*ifile.read(k_name, 32);
            cout<<setw(24)<<getUnicodeStr(k_name, 32);
            addr_val += 32;
            */
            ifile.ignore(8);
            addr_val += 8;

            last_module->next = curr_module;
            last_module = curr_module;
        }
        else
        {
            ifile.ignore(8);
            addr_val += 8;
        }
    }

    cout<<setw(10)<<"Address"<<setw(8)<<"Size"<<setw(8)<<"nameS"<<setw(18)<<"NameAddr"<<setw(20)<<"Name"<<endl;
    
    kernel_module *curr_module = modules;
    cout<<endl;
    while(curr_module->next != NULL)
    {
        curr_module = curr_module->next;

        ifile.clear();
        ifile.seekg(curr_module->name_addr, ios::beg);

        char name[curr_module->name_size * 2];

        ifile.read(name, sizeof(name));

        curr_module->name = getUnicodeStr(name, sizeof(name));

        cout<<setw(10)<<hex<<curr_module->address;
        cout<<setw(8)<<hex<<curr_module->block_size;
        cout<<setw(8)<<hex<<curr_module->name_size;
        cout<<setw(18)<<hex<<curr_module->name_addr;
        cout<<setw(20)<<curr_module->name;
        cout<<endl;
    }

    ifile.close();
    cout<<"File closed..";

    return 0;
}