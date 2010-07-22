#ifndef TD_SUPPORT
#define TD_SUPPORT 0

#include <vector>
#include <iostream>
#include <cmath>

typedef  std::vector<int> int_vec;
typedef  std::vector<int_vec> dble_array;

#define RelSize 2000

struct Graph
{
    int n; //size
    int** adj;//adjacency matrix
};

// this function is called by int totalDomination(Graph g)
void R_totaldomination(dble_array &adj_list, int n, short int *in_set, short int *dominated, int &current_order, int lower, int& upper)
{
    int k=0, i =0, next, list_n=0;
    bool diagnose=false;
    int this_list[RelSize];

    if (current_order >= upper)
	return;
    else
    {	next =0;
	while ((next < n) && (dominated[next]))  //find next candidate
	    next++;

	if (diagnose) std::cout << "in recurse next is " << next << "\n";

	if (next >= n) // if no candidate
	{ if (diagnose) std::cout << "all were dominated\n";
	    upper = current_order; return; }  // all were dominated
	//try next's neighbors
	for(i=1;i<=adj_list[next][0];i++)
	{   if (in_set[adj_list[next][i]]==0) // differ from id &&(!dominated[adj_list[next][i]]))
	    {  this_list[list_n] = adj_list[next][i]; list_n++;
		in_set[adj_list[next][i]] =1;
		current_order++;
		//dominated[adj_list[next][i]]++;
		for (k=1;k<=adj_list[adj_list[next][i]][0];k++)
		    dominated[adj_list[adj_list[next][i]][k]]++;
		if (diagnose)
		{std::cout << "about to recurse for " << next << " next's neighbor =" << adj_list[next][i] << " dom array is \n";
		    std::cout << "lower and upper are " << lower << " " << upper << "\n";
		    for (k=0;k<n;k++)
			std::cout <<  k << " : " << in_set[k] << " | " << k << " : " << dominated[k] << "\n";
		}
		R_totaldomination(adj_list, n, in_set, dominated, current_order, lower, upper);
		if (diagnose) std::cout << "back from recurse and removing next's neighbor \n";
		in_set[adj_list[next][i]] = 2;
		current_order--;
		//dominated[adj_list[next][i]]--;
		for (k=1;k<=adj_list[adj_list[next][i]][0];k++)
		    dominated[adj_list[adj_list[next][i]][k]]--;
	    }
	}

    }// else dominating set

    //clean up
    if (diagnose)
    {	std::cout << "About to clean up and list_n = " << list_n << "\n";
	for (int i=0; i < list_n;i++)
	    std::cout << i << " : " << this_list[i] << " ";
	std::cout << "\n";
    }

    for (int i=0; i < list_n;i++)
    {  if (in_set[this_list[i]]==1)
        { 	dominated[this_list[i]]--;
	    for (k=1;k<=adj_list[this_list[i]][0];k++)
		dominated[adj_list[this_list[i]][k]]--;
        }
	in_set[this_list[i]] = 0;
    }

}

int greedy_domination(Graph &g, short int *starter)
{
    short int dominated[RelSize];
    int i,k, count =0;

    dble_array adj_list;						//adj[i][0]=degree of i and the neigbors follow

    //ADDED  BY HOOMAN
    adj_list.resize(g.n);
    for(int i=0; i<g.n; i++)
        adj_list[i].resize(g.n+1);

    // build adjacency list and initialize dominated
    for ( k=0;k<g.n;k++)
    {	adj_list[k][0] = 0;
	dominated[k] =0;
	for (i=0;i<g.n;i++)
	    if (g.adj[k][i]==1)
	    {	adj_list[k][0]= adj_list[k][0] + 1;
	    adj_list[k][adj_list[k][0]] = i;
	}
    }
    // initialize dominated for starter set
    for ( i =0;i < g.n;i++)
	if (starter[i] ==1)
	{	count++;
	dominated[i]++;
	for ( k=1; k <= adj_list[i][0];k++)
	    dominated[adj_list[i][k]]++;
    }
    do{	//look for next vertex to add
	i=0;
	while ((dominated[i]) && (i<g.n))
	{ i++;
	}
	//mark it and its neighbors dominated
	if (i<g.n)
	{	dominated[i]++;
	    count++;
	    for ( k=1; k <= adj_list[i][0];k++)
		dominated[adj_list[i][k]]++;
	}
    }while(i<g.n);

    return count;
}
// traded some cpu for memory Aug 2009
int TotalDomination(Graph &g)
{
    int mindeg, maxdeg,
    lowerbound =1, upperbound = g.n,
    next, k, i,  current_order=0, list_n=0;
    bool diagnose=false;

    short int dominated[RelSize];
    short int in_set[RelSize];
    dble_array adj_list;						//adj[i][0]=degree of i and the neigbors follow

    //ADDED  BY HOOMAN
    adj_list.resize(g.n);
    for(int i=0; i<g.n; i++)
        adj_list[i].resize(g.n+1);


    // build adjacency list
    for (k=0;k<g.n;k++)
    {	adj_list[k][0] = 0;
	for (i=0;i<g.n;i++)
	    if (g.adj[k][i]==1)
	    {	adj_list[k][0]= adj_list[k][0] + 1;
	    adj_list[k][adj_list[k][0]] = i;
	}
    }
    if (diagnose)
	for (k=0;k<g.n;k++)
	{   std::cout << k << " : ";
	for (i =1; i<=adj_list[k][0];i++)
	    std::cout << adj_list[k][i] << " ";
	std::cout <<std::endl;
    }

    // initialize sets
    for ( k=0;k<g.n;k++)
    {	dominated[k] = 0;
	in_set[k] = 0;      // 0 undecided, 1 in set , 2 not in the set
    }

    maxdeg = 0;
    mindeg = g.n;
    for (k=0;k < g.n; k++)
    { 	if (adj_list[k][0] > maxdeg) maxdeg = adj_list[k][0]; //find max deg
	if (adj_list[k][0] < mindeg) mindeg = adj_list[k][0]; //find min deg
    }

    if (maxdeg == (g.n -1))
	return 2;
    else
    {   	int s, temp;

	lowerbound = std::ceil(double(g.n)/double(maxdeg));
	upperbound = g.n - maxdeg + 1; // dom <=g.n - maxdeg + 1

	for (k=0;k <g.n;k++)
	{ 	if (adj_list[k][0] ==1)
	    {  in_set[k] = 2; 			//leaves are not in set
		s = adj_list[k][1];
		if (in_set[s] != 1)
		{	     in_set[s] = 1; 			//support vertex in set
		    current_order++;
		    //dominated[s]++; 		        //vertices don't td-dominate themselves
		    for (i=1; i <= adj_list[s][0]; i++) //support's neighbors are now dominated
			dominated[adj_list[s][i]]++;
		}
	    }
	}

	temp = greedy_domination(g, in_set);
	if (upperbound > 2*temp)
	    upperbound = 2*temp;

	if (current_order > lowerbound)
	    lowerbound = current_order;
	if (lowerbound == upperbound)
	    return upperbound;
	else
	{  next = 0;
	    //find next undominated
	    while ((dominated[next]) && (next < g.n)) next++;

	    if (next >= g.n)		// all vertices dominated
		return current_order;
	    else
	    {
		for(i=1;i<=adj_list[next][0];i++)
		{   in_set[adj_list[next][i]] =1;		//put neighbor in set
		    current_order++;
		    //dominated[adj_list[next][i]]++;
		    for (k=1;k<=adj_list[adj_list[next][i]][0];k++)
			dominated[adj_list[adj_list[next][i]][k]]++;
		    if (diagnose)
		    {   std::cout << "degree of " << next << " next's neighor " << adj_list[next][i] << " is " << adj_list[adj_list[next][i]][0] << "\n";
			std::cout << "in_set and dominated array \n";
			for (k=0;k<g.n;k++)
			    std::cout <<  k << " : " << in_set[k] << " | " << k << " : " << dominated[k] << "\n";
		    }
		    R_totaldomination(adj_list, g.n, in_set, dominated, current_order, lowerbound, upperbound);
		    in_set[adj_list[next][i]] =2;
		    current_order--;
		    for (k=1;k<=adj_list[adj_list[next][i]][0];k++)
			dominated[adj_list[adj_list[next][i]][k]]--;
		}//for
	    }//else next
	}
	return upperbound;
    }//end of else
}

//~~~~~~~~~~~~~~~~Added By Jeremy~~~~~~~~~~~~~~~~
// this function is called by int k_Domination(Graph g)
// compared to dominatation, 2-domination contains leaves,
// domination by a element in the set is still ++!, but
// when a vetex dominates itself increment dominated by 2

//dble_array &adj_list
//int k_dom
//int n
//short int *in_set
//short int *dominated
//int &current_order
//int lower
//int& upper

void R_k_domination(dble_array &adj_list, int k_dom, int n, short int *in_set, short int *dominated, int &current_order, int lower, int& upper)
{
    int k=0, i =0, next, list_n=0;
    bool diagnose=false;
    int this_list[RelSize];

    if (current_order >= upper)
	return;
    else
    {       next =0;
	while ((next < n) && (dominated[next]>=k_dom))  //find next candidate
	    next++;

	if (diagnose)std::cout << "in recurse next is " << next << "\n";

	if (next >= n) // if no candidate
	{
	    if (diagnose)std::cout << "all were dominated\n";
	    upper = current_order; return; }  // all were dominated
	if (in_set[next]==0)
	{
	    this_list[list_n] = next;
	    list_n++;
	    //try next
	    in_set[next] =1;
	    dominated[next]+=k_dom;
	    current_order ++;
	    for (k=1;k<=adj_list[next][0];k++)
		dominated[adj_list[next][k]]++;

	    if (diagnose)
	    {
		std::cout << "in recurse for next=" << next << " in_set and dom array \n";
		for (k=0;k<n;k++)
		    std::cout <<  k << " : " << in_set[k] << " | " << k << " : " << dominated[k] << "\n";
		std::cin.get();
	    }

	    R_k_domination(adj_list, k_dom, n, in_set, dominated, current_order, lower, upper);
	    if (diagnose)std::cout << "back from recurse and removing next \n";

	    //remove next
	    in_set[next] = 2;
	    dominated[next]-=k_dom;
	    current_order--;
	    for (k=1;k<=adj_list[next][0];k++)
		dominated[adj_list[next][k]]--;
	}
	//now try next's neighbors
	for(i=1;i<=adj_list[next][0];i++)
	{
	    if (in_set[adj_list[next][i]]==0) // differ from id &&(!dominated[adj_list[next][i]]))
	    {
		this_list[list_n] = adj_list[next][i]; list_n++;
		in_set[adj_list[next][i]] =1;
		current_order++;
		dominated[adj_list[next][i]]+=k_dom;
		for (k=1;k<=adj_list[adj_list[next][i]][0];k++)
		    dominated[adj_list[adj_list[next][i]][k]]++;

		if (diagnose)
		{
		    std::cout << "about to recurse for " << next << " next's neighbor =" << adj_list[next][i] << " dom array is \n";
		    std::cout << "lower and upper are " << lower << " " << upper << "\n";
		    for (k=0;k<n;k++)
			std::cout <<  k << " : " << in_set[k] << " | " << k << " : " << dominated[k] << "\n";
		}

		R_k_domination(adj_list, k_dom, n, in_set, dominated, current_order, lower, upper);

		if (diagnose)std::cout << "back from recurse and removing next's neighbor \n";

		in_set[adj_list[next][i]] = 2;
		current_order--;
		dominated[adj_list[next][i]]-=k_dom;
		for (k=1;k<=adj_list[adj_list[next][i]][0];k++)
		    dominated[adj_list[adj_list[next][i]][k]]--;
	    }
	}

    }// else dominating set

    //clean up
    if (diagnose)
    {      std::cout << "About to clean up and list_n = " << list_n << "\n";
	for (int i=0; i < list_n;i++)
	    std::cout << i << " : " << this_list[i] << " ";
	std::cout << "\n";
    }

    for (int i=0; i < list_n;i++)
    {  if (in_set[this_list[i]]==1)
	{       dominated[this_list[i]]-=k_dom;
	    for (k=1;k<=adj_list[this_list[i]][0];k++)
		dominated[adj_list[this_list[i]][k]]--;
	}
	in_set[this_list[i]] = 0;
    }

}


// working on trading some cpu for memory Nov 2009
int k_Domination(int k_dom,Graph &g)
{
    int mindeg, maxdeg,
    lowerbound =1, upperbound = g.n,
    next, k, i,  current_order=0, list_n=0;
    bool diagnose=false;

    short int dominated[RelSize];
    short int in_set[RelSize];
    dble_array adj_list;                                            //adj[i][0]=degree of i and the neigbors follow

    //ADDED  BY HOOMAN
    adj_list.resize(g.n);
    for(int i=0; i<g.n; i++)
	adj_list[i].resize(g.n+1);

    // build adjacency list
    for (k=0;k<g.n;k++)
    {       adj_list[k][0] = 0;
	for (i=0;i<g.n;i++)
	    if (g.adj[k][i]==1)
	    {       adj_list[k][0]= adj_list[k][0] + 1;
	    adj_list[k][adj_list[k][0]] = i;
	}
    }
    if (diagnose)
	for (k=0;k<g.n;k++)
	{  std::cout << k << " : ";
	for (i =1; i<=adj_list[k][0];i++)
	    std::cout << adj_list[k][i] << " ";
	std::cout <<std::endl;
    }
    // initialize sets
    for ( k=0;k<g.n;k++)
    {       dominated[k] = 0;
	in_set[k] = 0;      // 0 undecided, 1 in set , 2 not in the set
    }

    maxdeg = 0;
    mindeg = g.n;
    for (k=0;k < g.n; k++)
    {       if (adj_list[k][0] > maxdeg) maxdeg = adj_list[k][0]; //find max deg
	if (adj_list[k][0] < mindeg) mindeg = adj_list[k][0]; //find min deg
    }

    {       int s;
	lowerbound = ceil(g.n/(1 + maxdeg));
	upperbound = g.n; // dom <=g.n
	for (k=0;k <g.n;k++)
	{       if (adj_list[k][0] <k_dom)
	    {  in_set[k] = 1;                     //leaves are in set & dominated
		dominated[k]+=k_dom;
		for (i=1; i <= adj_list[k][0]; i++) //support are now dominated at least once
		    dominated[adj_list[k][i]]++;
		current_order++;
	    }
	}

	if (current_order > lowerbound)
	    lowerbound = current_order;

	if (lowerbound == upperbound)
	    return upperbound;
	else
	{  next = 0;
	    //find next undominated
	    while (dominated[next]>=k_dom) next++;

	    if (next >= g.n)             // all vertices dominated
		return current_order;
	    else
	    {
		//try next    in_set[next] =1;
				dominated[next]+=k_dom;
				current_order++;
				for (k=1;k<=adj_list[next][0];k++)
				    dominated[adj_list[next][k]]++;
				if (diagnose)
				{  std::cout << "in MAIN k_Dom degree of next= " << next  << " is " << adj_list[next][0] << "\n";
				    std::cout << "Initial in_set and k_dominated array \n";
				    for (k=0;k<g.n;k++)
					std::cout <<  k << " : " << in_set[k] << " | " << k << " : " << dominated[k] << "\n";
				}

				//R_k_domination(adj_list, g.n, in_set, dominated, current_order, lowerbound, upperbound);
				R_k_domination(adj_list, k_dom, g.n, in_set, dominated, current_order, lowerbound, upperbound);

				if (diagnose)std::cout << "IN MAIN back from recurse and removing next \n";
				//remove next   in_set[next] = 2;
						  dominated[next]-=k_dom;
						  current_order--;
						  for (k=1;k<=adj_list[next][0];k++)
						      dominated[adj_list[next][k]]--;

						  //now try next's neighbors
						  for(i=1;i<=adj_list[next][0];i++)
						  {   in_set[adj_list[next][i]] =1;
						      current_order++;
						      dominated[adj_list[next][i]]+=k_dom;
						      for (k=1;k<=adj_list[adj_list[next][i]][0];k++)
							  dominated[adj_list[adj_list[next][i]][k]]++;
						      if (diagnose)
						      {  std::cout << "degree of " << next << " next's neighor " << adj_list[next][i] << " is " << adj_list[adj_list[next][i]][0] << "\n";
							  std::cout << "in_set and dominated array \n";
							  for (k=0;k<g.n;k++)
							      std::cout <<  k << " : " << in_set[k] << " | " << k << " : " << dominated[k] << "\n";
						      }

						      R_k_domination(adj_list,k_dom, g.n, in_set, dominated, current_order, lowerbound, upperbound);
						      if (diagnose)std::cout << "back from recurse and removing next's neighbor \n";
						      in_set[adj_list[next][i]] =2;
						      current_order--;
						      dominated[adj_list[next][i]]-=k_dom;
						      for (k=1;k<=adj_list[adj_list[next][i]][0];k++)
							  dominated[adj_list[adj_list[next][i]][k]]--;
						  }//for
					      }//else next
	}
	if (diagnose)
	{std::cout << "new k-dom = " << upperbound << "\n";
	    std::cin.get();
	}
	return upperbound;
    }//end of else
}




#endif
