#include <iostream>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <stdint.h>

using namespace std;

uint64_t get_phy_addr(ifstream &ifile, uint64_t vir_addr, uint32_t dtb, uint8_t level)
{
    uint64_t phy_addr;
    uint32_t temp1;
    uint32_t temp2;
    uint16_t p[4];
    uint32_t offset;

    p[0] = vir_addr>>39 & 0b111111111;
    p[1] = vir_addr>>30 & 0b111111111;
    p[2] = vir_addr>>21 & 0b111111111;
    p[3] = vir_addr>>12 & 0b111111111;
    offset = vir_addr & 0b111111111111;
    if(level == 3)
    {
        offset = vir_addr & 0b0111111111111111111111;
    }
    cout<<endl<<"Dtb: 0x"<<hex<<dtb<<endl;
    cout<<"p[0]: 0x"<<p[0]<<endl;
    cout<<"p[1]: 0x"<<p[1]<<endl;
    cout<<"p[2]: 0x"<<p[2]<<endl;
    cout<<"p[3]: 0x"<<p[3]<<endl;
    cout<<"Offset: 0x"<<offset<<endl;

    cout<<endl;  
    temp1 = (dtb & 0xFFFFF000) + p[0] * 8;
    ifile.ignore(temp1);
    ifile.read(reinterpret_cast<char *>(&temp2), sizeof(temp2));
    cout<<"PML4 Addr     : 0x"<<temp1<<endl;
    cout<<"PML4 Content  : 0x"<<temp2<<endl;
    ifile.clear();
    ifile.seekg(0, ios::beg);

    temp1 = (temp2 & 0xFFFFF000) + p[1] * 8;
    ifile.ignore(temp1);
    ifile.read(reinterpret_cast<char *>(&temp2), sizeof(temp2));
    cout<<"PDPTE Addr    : 0x"<<temp1<<endl;
    cout<<"PDPTE Content : 0x"<<temp2<<endl;
    ifile.clear();
    ifile.seekg(0, ios::beg);

    temp1 = (temp2 & 0xFFFFF000) + p[2] * 8;
    ifile.ignore(temp1);
    ifile.read(reinterpret_cast<char *>(&temp2), sizeof(temp2));
    cout<<"PDE Addr      : 0x"<<temp1<<endl;
    cout<<"PDE Content   : 0x"<<temp2<<endl;
    ifile.clear();
    ifile.seekg(0, ios::beg);

    if(level == 3)
    {
        return ((temp2 & 0b11111111111000000000000000000000) + offset);    
    }

    temp1 = (temp2 & 0xFFFFF000) + p[3] * 8;
    ifile.ignore(temp1);
    ifile.read(reinterpret_cast<char *>(&temp2), sizeof(temp2));
    cout<<"Table Addr    : 0x"<<temp1<<endl;
    cout<<"Table Content : 0x"<<temp2<<endl;
    ifile.clear();
    ifile.seekg(0, ios::beg);

    return ((temp2 & 0xFFFFF000) + offset);
}

int main()
{
    ifstream ifile;
	char s[100], fname[] = "data/win764.vmem";
    uint64_t phy_addr;
    /*
    uint64_t vir_addr = 0xFA801AB93360;
    uint32_t dtb = 0x00187000;
    uint8_t level = 3;
    */
    /*uint64_t vir_addr = 0x00942760;
    uint32_t dtb = 0x0baae000;
    uint8_t level = 4;
    */

    uint64_t vir_addr = 0xf8a00ba16b80;
    uint32_t dtb = 0x00187000;
    int level;
    if((vir_addr&0b1000000000000000000000000000) == 0) //untested
         level = 4;
    else level = 3;
    
    cout<<"Lvl: "<<level<<endl;
	ifile.open(fname, ios::in | ios::binary);
    if(!ifile)
	{
		cout<<"Error in opening file..!!";
		exit(0);
	}	
	cout<<"File opened.. "<<fname<<" "<<endl;
    phy_addr = get_phy_addr(ifile, vir_addr, dtb, level);
    cout<<endl<<"Virtual Address: 0x"<<hex<<vir_addr<<endl<<"Physical Address: 0x"<<phy_addr<<endl;
    return 0;
}