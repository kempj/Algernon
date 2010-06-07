#ifndef GRAPH_INVARIANTS
#define GRAPH_INVARIANTS 0

#include <vector>
#include <string>
#include <algorithm>
#include "graph.h"
#include "TD.h"

//********************************************
// Add additional Invariants here
//********************************************

double Size(m3sgraph &g, void* arg)
{       
	return double(g.GetSize());
}


double EdgeCount(m3sgraph &g, void* arg)
{       
	g.PopulateNeighborlist();
    std::vector<NeighborData*>* neighbors = g.GetNeighborListing();
    int sum = 0;
    for(int i=g.GetSize()-1; i>=0; i--)
    {
         sum+=(*neighbors)[i]->neighbors;
    }
    sum /=2; //number of edges

	return double(sum);
}


double HavelHakimiResidue(m3sgraph &g, void* arg)
{
       std::vector<long long unsigned int> seq = g.GetDegSeq();
       std::sort(seq.begin(), seq.end());
	   std::reverse(seq.begin(), seq.end());

       while(seq[0]!=0)
       { 
            for(int i=1; i<=seq[0]; i++)
            {
                seq[i]--;
            }
            seq.erase(seq.begin());
            std::sort(seq.begin(), seq.end());
			std::reverse(seq.begin(), seq.end());
        }
 
       return double(seq.size());
}
double TotalDomination(m3sgraph &g, void* arg)
{
       Graph tmpg;
       tmpg.n = g.GetSize();
       tmpg.adj = new int*[tmpg.n]; 
       for(int i=0; i<tmpg.n;i++)
       {
           tmpg.adj[i] = new int[tmpg.n];
           for(int j=0; j<tmpg.n; j++)
           {
                if(g.GetEdge(i,j))
                {
                     tmpg.adj[i][j] = 1;
                }
                else
                {
                    tmpg.adj[i][j] = 0;
                }
           }
       }
       return double(TotalDomination(tmpg));
}
double CaroWei (m3sgraph &g, void* arg)
{
    g.PopulateNeighborlist();
    std::vector<NeighborData*>* neighbors = g.GetNeighborListing();
    double ans = 0;
    for(int i=g.GetSize()-1; i>=0; i--)
    {
            ans+=1.0/((*neighbors)[i]->neighbors+1);
    }
	return ans;
}
double Tree (m3sgraph &g, void* arg)
{
    g.PopulateNeighborlist();
    std::vector<NeighborData*>* neighbors = g.GetNeighborListing();
    int sum = 0;
    for(int i=g.GetSize()-1; i>=0; i--)
    {
            if((*neighbors)[i]->neighbors>0)
            {
                sum+=(*neighbors)[i]->neighbors;
            }
            else
            {
                return 2; //is not a tree, actually its not even connected
            }
    }
    if(sum/2 > g.GetSize()-1)
        return 2; //is not tree
	return 1;
}
double EdgeDensity (m3sgraph &g, void* arg)
{
    g.PopulateNeighborlist();
    std::vector<NeighborData*>* neighbors = g.GetNeighborListing();
    int sum = 0;
    for(int i=g.GetSize()-1; i>=0; i--)
    {
         sum+=(*neighbors)[i]->neighbors;
    }
    sum /=2; //number of edges
    
    //return number of edges/total possible edges
	return double(sum)/double((g.GetSize()*(g.GetSize()-1))/2); 
}
//********************************************
struct InvariantDef
{
	std::string name;
	double(*ptr) (m3sgraph &g, void* arg);
};

class InvariantLib
{
private:
	std::vector<InvariantDef> list;
public:
	InvariantLib();
	int GetListSize();
	std::string GetInvariantName(int index);
	int GetIndex(std::string name);

	double GetInvariant(std::string name, m3sgraph &g, void* arg);
	double GetInvariant(int index, m3sgraph &g, void* arg);

	
};
double InvariantLib::GetInvariant(std::string name, m3sgraph &g, void* arg = NULL)
{
	for(int i=list.size()-1; i>=0; i--)
	{
		if(list[i].name==name)
		{
			return list[i].ptr(g,arg);
		}
	}
}
double InvariantLib::GetInvariant(int index, m3sgraph &g, void* arg = NULL)
{
	return list[index].ptr(g,arg);
}

int InvariantLib::GetListSize()
{
	return list.size();
}
std::string InvariantLib::GetInvariantName(int index)
{
	if(index>=list.size()||index < 0)
		return "";
	return list[index].name;
}
int InvariantLib::GetIndex(std::string name)
{
	for(int i=0; i<list.size(); i++)
	{
		if(list[i].name==name)
			return i;
	}
	return -1; //not found
}
//********************************************
//Add pointers to Variables to the bottom of
//  the constructor below
//********************************************
InvariantLib::InvariantLib()
{
	InvariantDef tmp;
	
	tmp.name = "Size";
	tmp.ptr = Size;
	list.push_back(tmp);

	tmp.name = "EdgeCount";
	tmp.ptr = EdgeCount;
	list.push_back(tmp);
	
	tmp.name = "Tree";
	tmp.ptr = Tree;
	list.push_back(tmp);
	
	tmp.name = "EdgeDensity";
	tmp.ptr = EdgeDensity;
	list.push_back(tmp);

	tmp.name = "CaroWei";
	tmp.ptr = CaroWei;
	list.push_back(tmp);
	
	tmp.name = "TotalDomination";
	tmp.ptr = TotalDomination;
	list.push_back(tmp);
	
	tmp.name = "HavelHakimiResidue";
	tmp.ptr = HavelHakimiResidue;
	list.push_back(tmp);
	
}

#endif
