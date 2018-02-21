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

    temp1 = (dtb & 0xFFFFF000) + p[0] * 8;
    ifile.ignore(temp1);
    ifile.read(reinterpret_cast<char *>(&temp2), sizeof(temp2));
    ifile.clear();
    ifile.seekg(0, ios::beg);

    temp1 = (temp2 & 0xFFFFF000) + p[1] * 8;
    ifile.ignore(temp1);
    ifile.read(reinterpret_cast<char *>(&temp2), sizeof(temp2));
    ifile.clear();
    ifile.seekg(0, ios::beg);

    temp1 = (temp2 & 0xFFFFF000) + p[2] * 8;
    ifile.ignore(temp1);
    ifile.read(reinterpret_cast<char *>(&temp2), sizeof(temp2));
    ifile.clear();
    ifile.seekg(0, ios::beg);

    if(level == 3)
    {
        return ((temp2 & 0b11111111111000000000000000000000) + offset);    
    }

    temp1 = (temp2 & 0xFFFFF000) + p[3] * 8;
    ifile.ignore(temp1);
    ifile.read(reinterpret_cast<char *>(&temp2), sizeof(temp2));
    ifile.clear();
    ifile.seekg(0, ios::beg);

    return ((temp2 & 0xFFFFF000) + offset);
}

uint64_t translateAddr(char* file_path, uint64_t vir_addr, uint32_t dtb)
{
    ifstream ifile;
    uint64_t phy_addr;

    int level;
    if((vir_addr&0b1000000000000000000000000000) == 0) //untested
         level = 4;
    else level = 3; 

    ifile.open(file_path, ios::in | ios::binary);
    if(!ifile)
    {
        cout<<"Error in opening file..!!";
        exit(0);
    }
    phy_addr = get_phy_addr(ifile, vir_addr, dtb, level);
    return phy_addr;
}

#ifndef mainfunc
int main()
{
    cout<<"\nPhysical Address: "<<hex<<translateAddr("data/win764.vmem", 0xfffffa8019ec5fe0, 0x00187000)<<endl;
    return 0;
}
#endif