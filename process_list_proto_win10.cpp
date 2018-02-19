#include <iostream>
#include <conio.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <iomanip>

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
	char s[100], fname[] = "data/win1064vir.vmem";
	int counter = 0;
    char found_pattern[8], magic_process_pattern[8] = {3, 0, 182, 0, 0, 0, 0, 0};
    char p_name[16];
    uint32_t pid, ppid;
    int end_flag = 0;
    int i, temp;
    long long addr_val = 0;

    cout<<sizeof(pid)<<endl;

	ifile.open(fname, ios::in | ios::binary);
    if(!ifile)
	{
		cout<<"Error in opening file..!!";
		exit(0);
	}	
	cout<<"File opened..";
	cout<<"\n";
    //cout<<"Address | Name"<<endl;
    cout<<setw(16)<<"Address"<<setw(16)<<"PID"<<setw(16)<<"PPID"<<setw(16)<<"Name"<<endl;
    while(ifile.eof()==0)
	{
        ifile.read(found_pattern, 8);
        addr_val+=8;
        if(compare_array(magic_process_pattern, found_pattern, 8))
        {

            cout<<setw(16)<<hex<<addr_val - 8;
            /*
            ifile.ignore(736 - 8);
            ifile.read(p_name, 16);
            cout<<p_name<<" "<<endl;
            */
            ifile.ignore(736 - 8);
            ifile.read(reinterpret_cast<char *>(&pid), sizeof(pid));
            cout<<setw(16)<<dec<<pid;
            ifile.ignore(256 - 4);
            ifile.read(reinterpret_cast<char *>(&ppid), sizeof(ppid));
            cout<<setw(16)<<ppid;
            ifile.ignore(1104 - 992 - 4);
            ifile.read(p_name, 16);
            cout<<setw(16)<<p_name<<endl;
            ifile.ignore(2072 - 1104 - 16);
            addr_val += 2072 - 8;
        }
        /*else
        {
            ifile.ignore(8);
            addr_val += 8;
        }*/
    }    
	ifile.close();
	cout<<"File closed..";
    return 0;
}