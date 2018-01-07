#include <iostream>
#include <conio.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
#include <string.h>
#include <stdint.h>

using namespace std;

void get_utf_string(ifstream &ifile, char str[], uint32_t addr)
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
    }
    str[size] = 0;
}

uint64_t get_phy_addr(ifstream &ifile, uint64_t vir_addr, uint32_t dtb, uint8_t level)
{
    uint64_t phy_addr;
    uint32_t temp1;
    uint32_t temp2;
    uint16_t p[4];
    uint32_t offset;
    
    ifile.clear();
    ifile.seekg(0, ios::beg);
    
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
	char s[100], fname[] = "data/win764.vmem";
	int counter = 0;
    char found_pattern_p[8], magic_process_pattern_p[8] = {3, 0, 88, 0, 0, 0, 0, 0};
    char found_pattern_s[8], magic_process_pattern_s1[8] = {115, 101, 114, 72, 0, 0, 0, 0}, magic_process_pattern_s2[8] = {115, 101, 114, 72, 4, 0, 0, 0};
    char p_name[16];
    uint32_t srv_dtb = 0xbaae000;
    uint64_t srv_record_addr[100], temp_addr, str_addr;
    char s_name[256] = {0}, d_name[256] = {0};
    int num_srv = 0;
    int end_flag = 0;
    long long addr_val = 0;
	ifile.open(fname, ios::in | ios::binary);
    if(!ifile)
	{
		cout<<"Error in opening file..!!";
		exit(0);
	}	
	cout<<"File opened..";
	cout<<"\n";
    //cout<<"Address | Name"<<endl;
    /*
    while(ifile.eof()==0)
	{
        ifile.read(found_pattern_p, 8);
        addr_val+=8;
        if(compare_array(magic_process_pattern_p, found_pattern_p, 8))
        {
            ifile.ignore(736 - 8);
            ifile.read(p_name, 16);
            if(strcmp(p_name, "services.exe")==0)
	        {
		        cout<<"services.exe found at "<<hex<<addr_val-8<<endl;
                ifile.seekg(-(736 + 16), std::ios::cur);
                ifile.seekg(40, std::ios::cur);
                unsigned long temp;
                ifile.read(reinterpret_cast<char *>(&temp), sizeof(temp));
		        cout<<"DTB value for services.exe "<<hex<<temp<<endl;
                srv_dtb = temp;
                break;
	        }
            ifile.ignore(480);
            addr_val += 1232 - 8;
        }
        else
        {
            ifile.ignore(8);
            addr_val += 8;
        }
    }
    ifile.clear();
    ifile.seekg(0, ios::beg);
    addr_val = 0;
    */
    while(ifile.eof()==0)
	{
        ifile.read(found_pattern_s, 8);
        addr_val+=8;
        if(compare_array(magic_process_pattern_s1, found_pattern_s, 8) || compare_array(magic_process_pattern_s2, found_pattern_s, 8))
        {
            ifile.ignore(8);
            addr_val += 8;
            ifile.read(reinterpret_cast<char *>(&temp_addr), sizeof(temp_addr));
            if(temp_addr > 0)
            {
                //cout<<"Service found at: 0x"<<hex<<addr_val - 16<<" with pointer: 0x"<<temp_addr<<" (V)"<<endl;
                srv_record_addr[num_srv] = temp_addr;
                num_srv++;
            }
            ifile.ignore(8);
            addr_val += 16;
        }
        else
        {
            ifile.ignore(8);
            addr_val += 8;
        }

    }
    
    cout<<endl;
    cout<<setw(20)<<"Address";
    cout<<setw(60)<<"Service Name";
    cout<<setw(60)<<"Display Name";    
    cout<<endl;
    
    for(int i = 0; i < num_srv; i++)
    {
        temp_addr = get_phy_addr(ifile, srv_record_addr[i], srv_dtb, 4);
        cout<<setw(20)<<hex<<temp_addr;

        ifile.clear();
        ifile.seekg(temp_addr + 8, ios::beg);        
        ifile.read(reinterpret_cast<char *>(&str_addr), sizeof(str_addr));
        str_addr = get_phy_addr(ifile, str_addr, srv_dtb, 4);
        get_utf_string(ifile, s_name, str_addr);
        cout<<setw(60)<<hex<<s_name;

        ifile.clear();
        ifile.seekg(temp_addr + 16, ios::beg);        
        ifile.read(reinterpret_cast<char *>(&str_addr), sizeof(str_addr));
        str_addr = get_phy_addr(ifile, str_addr, srv_dtb, 4);
        get_utf_string(ifile, d_name, str_addr);
        cout<<setw(60)<<hex<<d_name;

        cout<<endl;
    }
    
	ifile.close();
	cout<<"File closed..";
    return 0;
}