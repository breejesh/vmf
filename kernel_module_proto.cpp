#include <iostream>
#include <conio.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <iomanip>

#define mainfunc

#include "AddrTranslationProto.cpp"

using namespace std;


struct custom_eprocesses
{
    long long address;
    int pid;
    int ppid;
    char name[16];
    int active_threads;
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
    char str[n/2 + 1];
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
    
    cout<<setw(16)<<"Address"<<setw(32)<<"Name"<<endl;
    while(ifile.eof()==0)
	{
        ifile.read(found_pattern, 8);
        addr_val+=8;
        if(scan_tag(pool_tag, found_pattern, 8))
        {
            cout<<setw(16)<<hex<<addr_val - 8;
            ifile.ignore(200 - 8);
            addr_val += 200 - 8;
            ifile.read(k_name, 32);
            cout<<setw(32)<<getUnicodeStr(k_name, 32)<<endl;
            addr_val += 32;
            //ifile.ignore(480);
            ifile.ignore(8);
            addr_val += 8;
        }
        else
        {
            ifile.ignore(8);
            addr_val += 8;
        }
    }    
	ifile.close();
	cout<<"File closed..";
    return 0;
}