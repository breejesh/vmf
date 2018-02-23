#include <iostream>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <iomanip>
#include <stdint.h>
#include <bitset>

using namespace std;

void get_utf_string(ifstream &ifile, char str[], uint32_t addr)
{
    uint8_t c = 1;
    int size = 0;
    ifile.clear();
    ifile.seekg(addr, ios::beg);
    ifile.read(reinterpret_cast<char *>(&c), sizeof(c));
    while(c && size < 255)
    {
    	if(c == ',' || c == 13 || c == '\n')
        	str[size] = ' ';
        else str[size] = c;
        size++;
        ifile.ignore(1);
        ifile.read(reinterpret_cast<char *>(&c), sizeof(c));
    }
    str[size] = 0;
}


uint64_t get_phy_addr(ifstream &ifile, uint64_t vir_addr, uint32_t dtb, uint8_t level)
{
    ifile.clear();
    ifile.seekg(0, ios::beg);
    
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

int main()
{
	ifstream ifile;
	ifstream ikerntxt;
	ofstream okerncsv;
	char s[100], fname[] = "data/win764.vmem";
	char string1[256]="temp1", string2[256]="temp2";
	char vir_address_input[] = "data/kernel_virtual.txt";
	char address_output[] = "data/kern_vir_data.csv";
	uint64_t phy_addr1, phy_addr2;

	uint64_t vir_addr;
	uint32_t dtb = 0x00187000;

	ifile.open(fname, ios::in | ios::binary);
	if(!ifile)
	{
		cout<<"Error in opening file..!!";
		exit(0);
	}

	ikerntxt.open(vir_address_input, ios::in | ios::binary);
	if(!ikerntxt)
	{
		cout<<"Error in opening txt file..!!";
		exit(0);
	}
	okerncsv.open(address_output, ios::out | ios::binary);
	if(!okerncsv)
	{
		cout<<"Error in opening csv file..!!";
		exit(0);
	}


	cout<<"Virtual," << "Physical (3)," << "Physical (4)," << "Virtual (bin)\n";
	okerncsv<<"Virtual," << "Physical (3)," << "Physical (4)," << "Virtual (bin)\n";
	while(ikerntxt.eof() == 0)
	{
		ikerntxt>>hex>>vir_addr;
		phy_addr1 = get_phy_addr(ifile, vir_addr, dtb, 3);
		phy_addr2 = get_phy_addr(ifile, vir_addr, dtb, 4);
		
		string1[0] = 0;
		if(phy_addr1 < 0x80000000 && phy_addr1 > 0)
			get_utf_string(ifile, string1, phy_addr1);

		string2[0] = 0;
		if(phy_addr2 < 0x80000000 && phy_addr2 > 0)
			get_utf_string(ifile, string2, phy_addr2);

		cout<<hex<<vir_addr<<","<<phy_addr1<<","<<phy_addr2<<",0b"<<bitset<48>(vir_addr)<<","<<dec<<string1<<","<<string2<<endl;
		okerncsv<<hex<<vir_addr<<","<<phy_addr1<<","<<phy_addr2<<",0b"<<bitset<48>(vir_addr)<<","<<dec<<string1<<","<<string2<<endl;
	}
	return 0;
}