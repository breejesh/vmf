#include <iostream>
#include <conio.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>
#include <iomanip>

using namespace std;

int main()
{
	ifstream ifile;
	char s[100], fname[] = "data/cridex.vmem";
	ifile.open(fname, ios::in | ios::binary);
    if(!ifile)
	{
		cout<<"Error in opening file..!!";
		exit(0);
	}	
	cout<<"File opened..";
	cout<<"\n";
	int counter = 0;
    while(ifile.eof()==0)
	    {
		int temp = ifile.get();
		if(temp == EOF)
			break;
		cout<< setfill('0') << setw(2) << hex << temp << " ";
		counter++;
		if(counter > 16)
		{
			cout << endl;
			counter = 0;
		}
    }    
	ifile.close();
	cout<<"File closed..";
    return 0;
}