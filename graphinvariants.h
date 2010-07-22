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


double MaxDegree(m3sgraph &g, void* arg)
{
    g.PopulateNeighborlist();
    std::vector<NeighborData*>* neighbors = g.GetNeighborListing();
    double max = 0;
    double tmp = 0;
    for(int i=g.GetSize()-1; i>=0; i--)
    {
	 tmp =(*neighbors)[i]->neighbors;
	 if(tmp >= max)
	 {
	     max = tmp;
	 }
    }
	return double(max);
    //return double(0);
}

double MinDegree(m3sgraph &g, void* arg)
{
    g.PopulateNeighborlist();
    std::vector<NeighborData*>* neighbors = g.GetNeighborListing();
    double min = 0;
    double tmp = 0;
    tmp =(*neighbors)[0]->neighbors;
    min = tmp;
    for(int i=1; i < g.GetSize(); i++)
    {
	 if(tmp <= min)
	 {
	     min = tmp;
	 }
	 tmp =(*neighbors)[i]->neighbors;
    }
	return double(min);
    //return double(0);
}

double Size(m3sgraph &g, void* arg)
{
	return double(g.GetSize());
}

double UpperMedian(m3sgraph &g, void* arg)
{
    //This gives me a list(vector) of the degrees:
    g.PopulateNeighborlist();
    std::vector<NeighborData*>* neighbors = g.GetNeighborListing();
    //This is the upper median:
    return double((*neighbors)[ceil(double(g.GetSize() / 2.0))]->neighbors);

}

double NumMaxDegree(m3sgraph &g, void* arg)
{
    g.PopulateNeighborlist();
    std::vector<NeighborData*>* neighbors = g.GetNeighborListing();

    double MaxVal = 0;
    int NumMax = 0;
    //MaxVal = InvariantLib::GetInvariant("MaxDegree",g);

    for(int i=0; i < g.GetSize(); i++)
    {
	if(MaxVal < (*neighbors)[i]->neighbors)
	{
	    MaxVal = (*neighbors)[i]->neighbors;
	    NumMax = 1;
	}
	else if(MaxVal == (*neighbors)[i]->neighbors)
	{
	    NumMax++;
	}
    }
    return double(NumMax);
}

double NumMinDegree(m3sgraph &g, void* arg)
{
    g.PopulateNeighborlist();
    std::vector<NeighborData*>* neighbors = g.GetNeighborListing();

    double MinVal = 0;
    int NumMin = 0;
    MinVal = (*neighbors)[0]->neighbors;

    for(int i=1; i < g.GetSize(); i++)
    {
	if(MinVal > (*neighbors)[i]->neighbors)
	{
	    MinVal = (*neighbors)[i]->neighbors;
	    NumMin = 1;
	}
	else if(MinVal == (*neighbors)[i]->neighbors)
	{
	    NumMin++;
	}
    }
    return double(NumMin);
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

double Dom1(m3sgraph &g, void* arg)
{
    //int k_dom;// = *(int*)arg;
    //k should be passed, but if it is not, defaulting to 2_domination
    //if(k_dom == NULL)
    int k_dom = 1;

    //copied from the TotalDomination
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
    return double(k_Domination(k_dom,tmpg));
}

double Dom2(m3sgraph &g, void* arg)
{
    int k_dom = 2;
    //copied from the TotalDomination
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
    return double(k_Domination(k_dom,tmpg));
}

double Dom3(m3sgraph &g, void* arg)
{
    int k_dom = 3;
    //copied from the TotalDomination
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
    return double(k_Domination(k_dom,tmpg));
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
	
	tmp.name = "Degree";
	tmp.ptr = Size;
	list.push_back(tmp);

	tmp.name = "Size";
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

	tmp.name = "MaxDegree";
	tmp.ptr = MaxDegree;
	list.push_back(tmp);

	tmp.name = "MinDegree";
	tmp.ptr = MinDegree;
	list.push_back(tmp);

	tmp.name = "NumMaxDegree";
	tmp.ptr = NumMaxDegree;
	list.push_back(tmp);

	tmp.name = "NumMinDegree";
	tmp.ptr = NumMinDegree;
	list.push_back(tmp);
	
	tmp.name = "UpperMedian";
	tmp.ptr = UpperMedian;
	list.push_back(tmp);

	tmp.name = "Dom1";
	tmp.ptr = Dom1;
	list.push_back(tmp);

	tmp.name = "Dom2";
	tmp.ptr = Dom2;
	list.push_back(tmp);

	tmp.name = "Dom3";
	tmp.ptr = Dom3;
	list.push_back(tmp);
}

#endif
