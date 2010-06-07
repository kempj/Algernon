#include <iostream>
#include <fstream>
#include <string>
#include "graph.h"
#include "graphinvariants.h"
#include "graphalterations.h"

using namespace std;

string itos(int a)
{
    if(a==0)
	return "0";
    string ans;
    while(a>=1)
    {
	ans = char((a%10)+48)+ans;
	a/=10;
    }
    return ans;
}

int main(int args, char* argv[])
{
    int tmpint;
    double tmpdbl;
    string seedg;
    if(args==1)//no arguments
    {
        cout << "Please enter seed graph: ";
        cin >> seedg;          
    }
    else
    {
        seedg = argv[1];
    }
    
    m3sgraph g,g1;
    InvariantLib ilib;
    AlterationLib alib;
    int icount = ilib.GetListSize(); //number of available invariants
    int acount = alib.GetListSize(); //number of available alterations
    g.SetGraph(seedg.c_str());
    long long unsigned int aargs[4]; //alter. arguments
    for(int i=0; i<4; i++)
        aargs[i] = 1;
        
    bool doAlter = true;     
    
    //make graph directory
    string dir, gfolder, tmpStr;
    cout << "Please enter destination directory: ";
    cin >> dir;
    
    //create or append processed graph list
    fstream file;
    string filename;
    int processed_graph_count = 0;
    
    filename = "list00.pgl"; //processed graph list
    gfolder = dir + "" + filename;
    file.open(gfolder.c_str(),ios::in);
    if(!file)//file does not exist
    {
	file.clear();
	file.open(gfolder.c_str(),ios::out);//creates empty file
	file << ilib.GetListSize() << endl;//output invariant count
	for(int icurrent=0; icurrent<icount;icurrent++)//output invariant names
	{
	    file << ilib.GetInvariantName(icurrent) << endl;
	}
	file.close();
	file.clear();
    }
    else
    {
	file >> tmpint;//get magic num
	file.get(); //remove following newline
	for(int i=0; i<tmpint; i++)
	    getline(file,tmpStr);//dump invariant names
        while(!file.eof())
        {
	    for(int i=0; i<tmpint; i++)
		file >> tmpdbl; //dump invariant values

	    getline(file,tmpStr);
	    if(tmpStr==seedg)
	    {
		cout << "Graph processed already" << endl;
		file.close();
		cin.get(); cin.get();
		return 0;
	    }
	    processed_graph_count++;
        }
	if(processed_graph_count>0)
	    processed_graph_count--;

        file.close();
        file.clear();
    }

    string graph_number_name = itos(processed_graph_count);
    gfolder = dir + "" + graph_number_name;
    tmpStr = "mkdir \"" + gfolder + "\"";
    system(tmpStr.c_str());

    double tmpDbl;
    double *ioriginals = new double[icount]; //original invariants
    double *ichanges = new double[icount]; //original invariants

    for(int icurrent=0; icurrent<icount;icurrent++)
    {
         ioriginals[icurrent] = ilib.GetInvariant(icurrent, g, NULL);  
         ichanges[icurrent] = 0;
    }  
   
    file.open(gfolder.c_str(),ios::app|ios::out);
    for(int icurrent=0; icurrent<icount;icurrent++)
    {
         file << ioriginals[icurrent] << '\t';//output invariant values
    }
    file << seedg << endl;//add graph name
    file.close();
    file.clear();

    filename = dir + "" + graph_number_name + ".gap"; //graph alterations profile
    file.open(filename.c_str(), ios::out);
    file << seedg << endl;
    file.close();

    int alteration_count = 0;

    for(int acurrent=0; acurrent<acount; acurrent++) //acurrent: current alteration
    {
        filename = dir + "" + graph_number_name + "/" + alib.GetAlterationName(acurrent)+ ".gar"; //graph alteration record
        file.open(filename.c_str(), ios::out);

        while(doAlter)
        {
            alib.DoAlteration(acurrent,g,g1,aargs[0],aargs[1],aargs[2],aargs[3],NULL);
            if(g==g1)//alteration failed
            {
                if(aargs[3]>1)
                {
                     aargs[3] = 1;
                     aargs[2]++;
                }else if (aargs[2]>1)
                {
                     aargs[2] = 1;
                     aargs[1]++;
                }else if (aargs[1]>1)
                {
                     aargs[1] = 1;
                     aargs[0]++;
                }else
                {
                     doAlter = false;
                } 
            }
            else
            {
                alteration_count++;
                for(int icurrent=0; icurrent<icount;icurrent++)
                {    
                     tmpDbl = ilib.GetInvariant(icurrent, g1, NULL);              
                     file << tmpDbl - ioriginals[icurrent]<< " "; //or
                     //file << tmpDbl << " "; 
                     ichanges[icurrent]+= tmpDbl - ioriginals[icurrent];
                }              
                for(int i=0; i<4; i++)
			file << aargs[i] << " ";

		file << g1.GetG6() << endl;
		aargs[3]++; //increment last
            }
        }
        
        file.close();
        filename = dir + "" + graph_number_name + ".gap";
        file.open(filename.c_str(),ios::app|ios::out);
        file << alib.GetAlterationName(acurrent) << " ";
        for(int icurrent=0; icurrent<icount;icurrent++)
        {
	    file << ichanges[icurrent]/alteration_count << " ";
	    ichanges[icurrent] = 0;
        }
	alteration_count=0;
        file << endl;
        file.close();
        file.clear();
        
        doAlter = true;
        for(int i=0; i<4; i++)
	    aargs[i] = 1;
    }
    
    cin.get(); cin.get();
    return 0;
}
