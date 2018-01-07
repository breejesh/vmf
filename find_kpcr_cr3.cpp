#include <iostream>
#include <conio.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <iomanip>
#include<string.h>

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

int main()
{
	ifstream ifile;
	char s[100], fname[] = "data/win764.vmem";
    unsigned long long val, prev_val = 0, lim = 18446462598732840960ul;   
    long long addr_val = 0;
	ifile.open(fname, ios::in | ios::binary);
    if(!ifile)
	{
		cout<<"Error in opening file..!!";
		exit(0);
	}	
	cout<<"File opened.."<<endl;
    cout<<sizeof(val)<<endl;
    while(ifile.eof()==0)
	{
        ifile.read(reinterpret_cast<char *>(&val), sizeof(val));
        if(val > lim  && prev_val > lim && ((val - prev_val) == 384))
        {
            cout<<"KCPR Address: "<<hex<<addr_val<<endl;
            cout<<"DTB Address: "<<hex<<addr_val + 464 <<endl;
        }
        ifile.seekg(-7, std::ios::cur);
        addr_val++;
        if(addr_val % 1000000 == 0)
            cout<<hex<<addr_val<<endl;
        prev_val = val;
    }    
	ifile.close();
	cout<<"File closed..";
    return 0;
}