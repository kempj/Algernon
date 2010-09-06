#ifndef GRAPH_ALTERATIONS
#define GRAPH_ALTERATIONS 0

#include <vector>
#include <string>
#include <algorithm>
#include "graph.h"

#define SIZE_LIMIT 75

/*

NOTE: The 'void* arg'  is used to pass custom arguments to functions. It
can be either hardcoded into parts of the algorithm or used when library
is used in conjuction with programs other than algernon. Algernon ALWAYS
passes a NULL to 'arg' if it is auto generating arguments.

For values of a,b,c, or d that must be boolean, if any number other than
0 or 1 is used, the function should leave the graph unchanged. This will
for better pattern recognition.

If any value is unreasonable, the graph should reamin unchanged. This will
allow the program to discard bad choices.
*/


void RemoveSpike (m3sgraph &g, m3sgraph &ng,
		  long long unsigned int a,
		  long long unsigned int b,
		  long long unsigned int c,
		  long long unsigned int d, void* arg)
{
    /*
	a - Length of Spikes removed
	b - Remove all Spikes 1 for no, 2 for yes
	c - Number of Spikes To remove(if b is 1)
	d - Unused
	*/

    ng = g;

    //Testing to make sure the inputs are sane
    if(b > 2)
	return;
    if(c > 5)
	return;
    if(a > 3)
	return;
    if(b == 2 && c > 1)
	return;
    if(d > 1)
	return;

    ng.PopulateNeighborlist();
    std::vector<NeighborData*>* neighbors = ng.GetNeighborListing();
    int spikeArray[a];
    int indexOfLastVertex, j;
    bool completeSpike = false;

    for(int i = 0; i < ng.GetSize(); i++)
    {
	//this loop goes through all the vertices and the if statement
	// selects all of the pendants.
	if((*neighbors)[i]->neighbors == 1)
	{
	    indexOfLastVertex = i;
	    //This loop traverses the graph as long as the degree is two, until
	    // it reaches the spike length a.
	    for(j = 0; j < a - 1; j++)
	    {
		spikeArray[j] = indexOfLastVertex;
		//This if, elseif is needed so the program doesn't go back and forth between 2 vertices
		// if the order of the elements in the list isn't consistent.
		if((*neighbors)[indexOfLastVertex]->list[0] != indexOfLastVertex && (*neighbors)[(*neighbors)[indexOfLastVertex]->list[0]]->neighbors == 2)
		{
		    indexOfLastVertex = (*neighbors)[indexOfLastVertex]->list[0];
		}
		else if((*neighbors)[(*neighbors)[indexOfLastVertex]->list[1]]->neighbors == 2)
		{
		    indexOfLastVertex = (*neighbors)[indexOfLastVertex]->list[1];
		}
		else
		{
		    continue;
		}
		//if this if statement is entered, then a spike has been found
		// and is going to be deleted
		if(j == a-2)
		{
		    for(int k = 0; k < a-1; k++)
		    {
			ng.RemoveVertex(spikeArray[k]);
		    }
		}
	    }
	}
    }

//for 1:n, where n is spikelength
    //check if degree of neighbor is 2
//if vertex was a spike of length n, then delete all of the nodes traversed
    // store the vertices in an array length n
//
//do this for every vertex.

    return;
}

void SpikeHighest (m3sgraph &g, m3sgraph &ng,
		   long long unsigned int a,
		   long long unsigned int b,
		   long long unsigned int c,
		   long long unsigned int d, void* arg)
{
    /*
	a - rank (i.e. 1st highest, 2nd highest,...)
	b - number of such vertices to spike (next highest if no more a)
	c - number of spikes per target - capped at size of graph or 5 wichever is higher
	d - length of spike (i.e. p1, p2,p3) - capped at 5 i.e.
	*/

    ng = g;

    //Testing to make sure the inputs are sane
    //and limiting the length of spikes to 5
    //JK - replacing all of the 5s with 3s
    if(a>g.GetSize()||b>g.GetSize()||d>3)
	return;
    if(g.GetSize()>=3 && c > g.GetSize())
	return;
    if(g.GetSize()<3 && c > 3)
	return;

    std::vector<long long unsigned int> seq = g.GetDegSeq();

    if(seq.size()==0)
	return;

    std::vector<long long unsigned int> seq2 = seq;
    std::sort(seq2.begin(), seq2.end()); //lowest value on lowest index

    long long unsigned int targetdeg = seq2[seq2.size()-1];
    int count = 0; //vertices spiked - MAX b

    int dchanges = 1;
    int index;
    for(index = seq2.size()-1; index>=0 && dchanges<a; index--)
    {
	if(seq2[index]!=targetdeg)
	{
	    targetdeg = seq2[index];
	    dchanges++;
	}
    }
    if(index!=seq2.size()-1)//index was changed
	index++; //this compensates for the last index-- done before exiting

    /*NOTE: The following are not adjusted so that
	identical graphs are not created. If any argument
	values are adjusted inside to function we risk 
	generating identical graphs and afffecting data.
	i.e. if instead of exactly b, what ever is available 
	is used, there will be successful runs with highers b
	values resulting in the same graphs.*/
    if(dchanges<a)
	return; //a is too high for this graph
    if(b-1 > index)
	return; //b is too high for this graph


    int spikeindex;

    while(count<b)
    {
	for(int i=0; i<seq.size()&&count<b; i++)
	{
	    if(seq[i]==targetdeg)//snce new vertices are padded indices are still valid
	    {
		for(long long unsigned int spikes = 0; spikes < c; spikes++)
		{
		    spikeindex = g.GetSize() + (count*c*d) + spikes*d;
		    ng.AddVertex();
		    //std::cout << "-" << ng.GetG6() <<std::endl;
		    ng.SetEdge(spikeindex,i,true);
		    //std::cout << "--" << ng.GetG6() <<std::endl;
		    for(unsigned int len = 1; len < d; len++)
		    {
			ng.AddVertex();
			ng.SetEdge(spikeindex+len,spikeindex+len-1,true);
		    }
		}
		count++;
	    }
	}
	targetdeg--; //go to next highest degree
    }

    if(ng.GetSize()>SIZE_LIMIT)
	ng = g;

    return;
}

void ClonesToHighest (m3sgraph &g, m3sgraph &ng,
		      long long unsigned int a,
		      long long unsigned int b,
		      long long unsigned int c,
		      long long unsigned int d, void* arg)
{
    //NOTE: It is simple and faster to copy SpikeHighest and just
    //add to it. This implmentation is only a test.

    //Similar to spiking except attaches clones of the orginal graph
    //to the end of the spikes, on the vertex that corolates to the
    //current spiked vertex
    /*
	a - rank (i.e. 1st highest, 2nd highest,...)
	b - number of such vertices to spike (next highest if no more a)
	c - number of spikes per target - capped at size of graph or 5 wichever is higher
	d - length of spike (i.e. p1, p2,p3) - capped at 5 i.e.
	*/

    ng = g;

    //JK - replacing all of the 5s with 3s
    if(a>g.GetSize()||b>g.GetSize()||d>3)
	return;
    if(g.GetSize()>=3 && c > g.GetSize())
	return;
    if(g.GetSize()<3 && c > 3)
	return;

    std::vector<long long unsigned int> seq = g.GetDegSeq();

    if(seq.size()==0)
	return;

    std::vector<long long unsigned int> seq2 = seq;
    std::sort(seq2.begin(), seq2.end()); //lowest value on lowest index

    long long unsigned int targetdeg = seq2[seq2.size()-1];
    int count = 0; //vertices spiked - MAX b

    int dchanges = 1;
    int index;
    for(index=seq2.size()-1; index>=0&&dchanges<a;index--)
    {
	if(seq2[index]!=targetdeg)
	{
	    targetdeg = seq2[index];
	    dchanges++;
	}
    }
    if(index!=seq2.size()-1)//index was changed
	index++; //this compensates for the last index-- done before exiting

    if(dchanges<a)
	return; //a is too high for this graph
    if(b-1 > index)
	return; //b is too high for this graph


    int spikeindex;
    long long unsigned int gsize = g.GetSize();

    while(count<b)
    {
	for(int i=0; i<seq.size()&&count<b; i++)
	{
	    if(seq[i]==targetdeg)//since new vertices are padded indices are still valid
	    {
		for(long long unsigned int spikes = 0; spikes < c; spikes++)
		{
		    spikeindex = g.GetSize() + (count*c*(d+gsize-1)) + spikes*(d+gsize-1);
		    ng.AddVertex();
		    //std::cout << "-" << ng.GetG6() <<std::endl;
		    ng.SetEdge(spikeindex,i,true);
		    //std::cout << "--" << ng.GetG6() <<std::endl;
		    for(unsigned int len = 1; len < d; len++)
		    {
			ng.AddVertex();
			ng.SetEdge(spikeindex+len,spikeindex+len-1,true);
		    }
		    long long unsigned int tip = ng.GetSize()-1; //tip of spike
		    //////////////////////////////
		    bool isShort = false; //spike is short
		    if(tip == g.GetSize()+count*(d+g.GetSize()-1))
		    {
			ng.SetEdge(i,tip,false);
			isShort = true;
		    }
		    else
		    {
			ng.SetEdge(tip-1,tip,false);
		    }
		    /////////////////////////////
		    for(long long unsigned int v = 0; v < gsize-1; v++)
		    {
			ng.AddVertex();
		    }
		    /////////////////////////////
		    if(isShort)
		    {
			ng.SetEdge(i,i+g.GetSize()+count*(d+g.GetSize()-1),true);
		    }
		    else
		    {
			ng.SetEdge(tip-1,tip+i,true);
		    }
		    /////////////////////////////
		    for(long long unsigned int v = 0; v < gsize; v++)
		    {
			for(long long unsigned int u = 0; u < v; u++)
			{
			    if(g.GetEdge(v,u))
			    {
				//ng.SetEdge(tip+((v+i)%gsize),tip+((i+u)%gsize),true);
				ng.SetEdge(tip+v,tip+u,true);
			    }
			}
		    }
		}
		count++;
	    }
	}
	targetdeg--; //go to next highest degree
    }

    if(ng.GetSize()>SIZE_LIMIT)
	ng = g;

    return;
}

void CliqueHighest (m3sgraph &g, m3sgraph &ng,
		    long long unsigned int a,
		    long long unsigned int b,
		    long long unsigned int c,
		    long long unsigned int d, void* arg)
{
    /*
	a - rank (i.e. 1st highest, 2nd highest,...)
	b - number of such vertices (in addition to first) to form a clique with (next highest if no more a)
	c - maximum number of cliques to form (currently unused)
	d - ?????.
	*/

    ng = g;

    if(a>g.GetSize()||b>g.GetSize()-1||d>1)
	return; //imposible rank or impossible # of vertices to include or
    //d is invalid so anything other than default value 1 is avoided
    if(c>1)//currently unused
	return;

    std::vector<long long unsigned int> seq = g.GetDegSeq();

    if(seq.size()==0)
	return;

    std::vector<long long unsigned int> seq2 = seq;
    std::sort(seq2.begin(), seq2.end()); //lowest value on lowest index

    long long unsigned int targetdeg = seq2[seq2.size()-1];
    int count = 0; //vertices spiked - MAX b

    int rank = 1; //first highest
    int index;
    for(index=seq2.size()-1; index>=0 && rank<a; index--)
    {
	if(seq2[index]!=targetdeg)
	{
	    targetdeg = seq2[index];
	    rank++;
	}
    }
    if(index!=seq2.size()-1)//index was changed
	index++; //this compensates for the last index-- done before exiting

    /*NOTE: The following are not adjusted so that
	identical graphs are not created. If any argument
	values are adjusted inside to function we risk 
	generating identical graphs and afffecting data.
	i.e. if instead of exactly b, what ever is available 
	is used, there will be successful runs with highers b
	values resulting in the same graphs.*/
    if(rank<a)
	return; //a is too high for this graph
    if(b > index)
	return; //b is too high for this graph

    //index is the index of target deg in seq2
    bool exists = false;
    std::vector<int> targetlist;

    for(int j=seq.size()-1; j>=0; j--) //adding first vertex
    {
	if(seq[j]==seq2[index])
	{
	    targetlist.push_back(j);
	    index--;
	    j=-1;
	}
    }

    while(targetlist.size()<=b)
    {
	for(int j=seq.size()-1; j>=0; j--)
	{
	    if((seq[j]==seq2[index]))
	    {
		for(int k=targetlist.size()-1; k>=0 && !exists; k--)
		{
		    if(targetlist[k]==j)
			exists = true;
		}
		if(!exists)
		{
		    targetlist.push_back(j);
		    index--;
		    if(targetlist.size()==b+1)
			j=-1;
		}
		exists = false;
	    }
	}
    }
    for(int i=targetlist.size()-1; i>0; i--)
    {
	for(int j=i-1;j>=0;j--)
	{
	    ng.SetEdge(i,j,true);
	}
    }

    if(ng.GetSize()>SIZE_LIMIT)
	ng = g;

    return;
}

//********************************************
struct AlterationDef
{
    std::string name;
    void (*ptr) (m3sgraph &g, m3sgraph &ng,
		 long long unsigned int a,
		 long long unsigned int b,
		 long long unsigned int c,
		 long long unsigned int d, void* arg);
};

class AlterationLib
{
private:
    std::vector<AlterationDef> list;
public:
    AlterationLib();
    int GetListSize();
    std::string GetAlterationName(int index);

    void DoAlteration(std::string name,
		      m3sgraph &graph,m3sgraph &ng,
		      long long unsigned int a, long long unsigned int b,
		      long long unsigned int c, long long unsigned int d,
		      void* ptr = NULL);
    void DoAlteration(int s,
		      m3sgraph &graph,m3sgraph &ng,
		      long long unsigned int a, long long unsigned int b,
		      long long unsigned int c, long long unsigned int d,
		      void* ptr = NULL);

    m3sgraph DoAlteration(std::string name,
			  m3sgraph &graph,
			  long long unsigned int a, long long unsigned int b,
			  long long unsigned int c, long long unsigned int d,
			  void* ptr = NULL);
    m3sgraph DoAlteration(int s,
			  m3sgraph &graph,
			  long long unsigned int a, long long unsigned int b,
			  long long unsigned int c, long long unsigned int d,
			  void* ptr = NULL);
    std::string DoAlteration(std::string name,
			     std::string g6,
			     long long unsigned int a, long long unsigned int b,
			     long long unsigned int c, long long unsigned int d,
			     void* ptr = NULL);
    std::string DoAlteration(int s,
			     std::string g6,
			     long long unsigned int a, long long unsigned int b,
			     long long unsigned int c, long long unsigned int d,
			     void* ptr = NULL);
};

void AlterationLib::DoAlteration(std::string name, 
				 m3sgraph &graph,m3sgraph &ng,
				 long long unsigned int a, long long unsigned int b,
				 long long unsigned int c, long long unsigned int d,
				 void* ptr)
{
    for(int i=list.size()-1; i>=0; i--)
    {
	if(list[i].name==name)
	{
	    list[i].ptr(graph,ng,a,b,c,d,ptr);
	    return;
	}
    }
}

void AlterationLib::DoAlteration(int s, 
				 m3sgraph &graph,m3sgraph &ng,
				 long long unsigned int a, long long unsigned int b,
				 long long unsigned int c, long long unsigned int d,
				 void* ptr)
{
    list[s].ptr(graph,ng,a,b,c,d,ptr);
}

m3sgraph AlterationLib::DoAlteration(std::string name, 
				     m3sgraph &graph,
				     long long unsigned int a, long long unsigned int b,
				     long long unsigned int c, long long unsigned int d,
				     void* ptr)
{
    m3sgraph ng;
    DoAlteration(name,graph,ng,a,b,c,d,ptr);
    return ng;
}

m3sgraph AlterationLib::DoAlteration(int s, 
				     m3sgraph &graph,
				     long long unsigned int a, long long unsigned int b,
				     long long unsigned int c, long long unsigned int d,
				     void* ptr)
{
    m3sgraph ng;
    DoAlteration(s,graph,ng,a,b,c,d,ptr);
    return ng;
}

std::string AlterationLib::DoAlteration(std::string name, 
					std::string g6,
					long long unsigned int a, long long unsigned int b,
					long long unsigned int c, long long unsigned int d,
					void* ptr )
{
    m3sgraph graph,ng;
    graph.SetGraph(g6.c_str());
    DoAlteration(name,graph,ng,a,b,c,d,ptr);
    return ng.GetG6();
}

std::string AlterationLib::DoAlteration(int s, 
					std::string g6,
					long long unsigned int a, long long unsigned int b,
					long long unsigned int c, long long unsigned int d,
					void* ptr)
{
    m3sgraph graph,ng;
    graph.SetGraph(g6.c_str());
    DoAlteration(s,graph,ng,a,b,c,d,ptr);
    return ng.GetG6();
}
int AlterationLib::GetListSize()
{
    return list.size();
}

std::string AlterationLib::GetAlterationName(int index)
{
    return list[index].name;
}

//********************************************
// Add pointer to the library
// at the end of the constructor below
//********************************************
AlterationLib::AlterationLib()
{
    AlterationDef tmp;

    tmp.name = "SpikeHighest";
    tmp.ptr = SpikeHighest;
    list.push_back(tmp);

    tmp.name = "ClonesToHighest";
    tmp.ptr = ClonesToHighest;
    list.push_back(tmp);

    tmp.name = "CliqueHighest";
    tmp.ptr = CliqueHighest;
    list.push_back(tmp);

    tmp.name = "RemoveSpike";
    tmp.ptr = RemoveSpike;
    list.push_back(tmp);
}

#endif
