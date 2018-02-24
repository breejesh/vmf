#include <iostream>
#include <conio.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
#include <stdint.h>

#define mainfunc

#include "AddrTranslationProto.cpp"

using namespace std;

void get_utf_string(ifstream &ifile, char str[], uint64_t addr)
{
    uint8_t c = 1;
    int size = 0;
    ifile.clear();
    ifile.seekg(addr, ios::beg);
    ifile.read(reinterpret_cast<char *>(&c), sizeof(c));
    while(c)
    {
        str[size] = c;
        size++;
        ifile.ignore(1);
        ifile.read(reinterpret_cast<char *>(&c), sizeof(c));
        if(c > 127)
            break;
    }
    str[size] = 0;
}

int scan_tag(char arr1[], char arr2[], int n)
{
	int i;
	for(i = 4; i < n; i++)
		if(arr1[i] != arr2[i])
			return 0;
	return 1;
}

int compare_array(char arr1[], char arr2[], int n)
{
    int i;
    for(i = 0; i < n; i++)
        if(arr1[i] != arr2[i])
            return 0;
    return 1;
}


int main()
{
    ifstream ifile;
    ofstream ofile;
    char fname[] = "data/Win764.vmem";
    char hive_signature[] = {char(224), char(190), char(224), char(190)};
    char found_pattern[8];
    char file_path[100];
    uint64_t vir_file_addr, phy_file_addr;
    long long addr_val = 0;
    char pool_tag[] = "0000CM10";

    ifile.open(fname, ios :: in | ios :: binary);
    if(!ifile)
	{
		cout<<"Error in opening file..!!";
		exit(0);
	}
    cout<<"File opened..";
	cout<<"\n";
    cout<<setw(16)<<"Physical Address"<<setw(70)<<"Name"<<endl;
    while(ifile.eof() == 0)
    {
        ifile.read(found_pattern, 8);
        addr_val += 8;
        if(scan_tag(pool_tag, found_pattern, 8))
        {
            ifile.ignore(8);
            addr_val += 8;
            cout<<hex<<setw(16)<<addr_val<<setw(70);
            ifile.ignore(0x6f0);
            addr_val += 0x6d8 + 24;
            ifile.read(reinterpret_cast<char *>(&vir_file_addr), 8);
            addr_val += 8;
            if(vir_file_addr == 0){
                ifile.ignore(8);
                addr_val += 8;
                ifile.read(reinterpret_cast<char *>(&vir_file_addr), 8);
                addr_val += 8;
            }
            if(vir_file_addr == 0)
                cout<<"[no name]"<<endl;
            else
            {
                phy_file_addr = translateAddr(ifile, vir_file_addr, 0x00187000);
                get_utf_string(ifile, file_path, phy_file_addr);
                cout<<file_path<<endl;
            }
            ifile.clear();
            ifile.seekg(addr_val, ios::beg);
        }
    }
    
    ifile.close();
    cout<<"Done"<<endl;
    return 0;
}
