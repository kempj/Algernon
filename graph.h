#ifndef GRAPH_LIB
#define GRAPH_LIB 0

#include <iostream>
#include <vector>
#include <string>

#include "g6tools.h"
class m3sgraph;
struct NeighborData
{
    int index; //vertex index in graph
    int neighbors; //number of neighbors
    int listsize;
    int* list;

    NeighborData()
    {
      index = -1; //undefined
      neighbors = 0;
      listsize = -1; //uninitialized
      list = NULL;
    }

    bool GetInfo (int i, m3sgraph& g);               
};

class m3sgraph 
{
	/*
	m3sgraph  is a minimal memory matrix simple graph. This graph
	is slow to access but quite small on memory consumption. For 
	example:
	a graph of size 100 would require about: 
	-10000 bytes if represented with a boolean matrix
	-4950 bytes if representated by only an upper triangle
	-825 (cieling of 4950/6)bytes using g6 type notation
	-619 (cieling of 4950/8)bytes using a m3sgraph

	Since the graph is represented on the bit level, accessing info
	can be slower than a full matrix, however if multiple graphs are
	needed simultaneously, it is preferable.
	*/
private:
	//BitByteTools bbt;
	long long unsigned int size;
	unsigned int length; // int is 32bit
	unsigned char* graph;
	std::vector<NeighborData*> neighborlist;
	bool neghborlistIsUpdated;
public:
	m3sgraph();
	~m3sgraph();
	m3sgraph(const m3sgraph& g); //copy constructor
	m3sgraph& operator=(const m3sgraph& g); //assignment operator overload
	bool operator==(const m3sgraph &other);
	bool operator!=(const m3sgraph &other);


	void SetEdge(long long unsigned int a,long long unsigned int b, bool v);
	bool GetEdge(long long unsigned int a,long long unsigned int b);

	void AddVertex();
	void RemoveVertex(long long unsigned int v);

	bool SetGraph(const char* g6); //convert from Graph6 format
	bool SetGraph(std::istream& in); //convert from Graph6 format
	long long unsigned int GetDeg(long long unsigned int a);
	std::vector<long long unsigned int> GetDegSeq(); //works only with small graphs (size -> 32bit int)
	std::string GetG6();

	long long unsigned int GetSize();

	void Draw(std::ostream& out);
	void PopulateNeighborlist();
	std::vector<NeighborData*>* GetNeighborListing() {return &neighborlist;} 
};
m3sgraph::m3sgraph()
{
	size = 0;
	length = 0;
	graph = NULL;
	neghborlistIsUpdated = false;
}
m3sgraph::~m3sgraph()
{return;//***************************************************//what?
	if(graph != NULL)
		delete[] graph;
}
m3sgraph::m3sgraph(const m3sgraph &g)
{
	size = g.size;
	length = g.length;
	graph = new unsigned char[length];
	for(unsigned int i=0; i<length;i++)
	{
		graph[i]=g.graph[i];
	}
}
m3sgraph& m3sgraph::operator=(const m3sgraph& g)
{
	if(this != &g) //check to see if its the same object or not (circumventing self assignment?)
	{
		delete[] graph; //remove old graph data
		size = g.size;
		length = g.length;
		graph = new unsigned char[length];
		for(unsigned int i=0; i<length;i++)
		{
			graph[i]=g.graph[i];
		}
	}
	return *this;
}
bool m3sgraph::operator ==(const m3sgraph &other)
{
	if(size != other.size)
		return false;
	/*if(length != length.size)
		return false*/ //unnecessary since for a particular size length must be the same
	for(unsigned int i=0; i<length;i++)
	{
		if(int(graph[i])!=int(other.graph[i]))
			return false;
	}

	return true;
}
bool m3sgraph::operator !=(const m3sgraph &other)
{
	return !(*this == other);
}

void m3sgraph::SetEdge(long long unsigned int a,long long unsigned int b, bool v)
{
	long long unsigned int tmp;
	unsigned char bit = 0;
	if(a==b)
		return;
	if(a>b) 
	{//use 'a' as column
		tmp = a;
		a=b;
		b=tmp;
	}
	tmp = ((b-1)*(b))/2; //previous coloumns summed up
	tmp += a; //elements from this column added
	bit = bit = 128 >> (tmp%8);
	tmp = tmp/8;

	graph[tmp]|= bit;

	if(!v)
	{
		graph[tmp]^= bit;
	}

	return;
}

bool m3sgraph::GetEdge(long long unsigned int a,long long unsigned int b)
{
	long long unsigned int tmp;
	unsigned char bit = 0;
	if(a==b)
		return false;
	if(a>b) //use 'a' as column
	{
		tmp = a;
		a=b;
		b=tmp;
	}
	tmp = ((b-1)*(b))/2; //previous coloumns summed up
	tmp += a+1; //elements from this column added
	if(tmp%8==0)
	{
		bit = 1;
		tmp = tmp/8 - 1; //byte index
	}
	else
	{
		bit = 128 >> ((tmp%8)-1);
		tmp = tmp/8; //byte index
	}

	if(((graph[tmp])&bit)==bit)
		return true;

	return false;
}

void m3sgraph::AddVertex()
{
	if(size == 0)
	{
		size+=1;
		return; //no edges in a 1 vertex graph
	}
	if(size == 1)
	{
		graph = new unsigned char[1];
		graph[0]=0;
		size++;
		length++;
		return;
	}
	long long unsigned int tmp = (size*(size-1))/2; //current # of used bits

	//number of actual bits to add (max # = size, since the current size is 1 less
	//than the intended size:
	if(tmp%8==0)
	{
		tmp = size; //bits to add 
	}
	else
	{// tmp%8 is number of bits used in last byte
		if(size > (8-(tmp%8))) //if the remaining bits in last byte 
		{	//are less than the maximum required bits(size)
			tmp = size - (8-(tmp%8)); //reduce by existing # of unused bits 
		}
		else
			tmp = 0; //no additional bits required
	}
	if(tmp!=0)
	{
		//number of bytes to add
		if(tmp%8==0)
		{
			tmp = tmp/8;
		}
		else
		{
			tmp = (tmp/8)+1;
		}

		unsigned char* tmpg = graph;
		graph = new unsigned char[length + tmp];

		for(long long unsigned int i = 0; i< length; i++)
		{
			graph[i]=tmpg[i];
			//std::cout << "m3u byte - " << i << " of " << length + tmp << ":" <<int(graph[i]) << std::endl;
		}
		for(long long unsigned int i = 0; i< tmp; i++)
		{
			graph[length+i]=0;
			//std::cout << "new m3u byte - " << length + i<< " of " << length + tmp << ":" <<int(graph[length+i]) << std::endl;
		}
		length = length + tmp;
		//delete[] tmpg;****************************************
	}
	size++;

}
void m3sgraph::RemoveVertex(long long unsigned int v)
{
	unsigned char* old = graph;
	long long unsigned int bitcount = ((size-2)*(size-1))/2;
	long long unsigned int tmpInt64;
	unsigned int newlength;
	unsigned int cbyte; //byte column starts in
	unsigned int cbit;  //first bit of column
	
	if(bitcount%8==0)
		newlength = bitcount/8;
	else
		newlength = bitcount/8 + 1;

	graph = new unsigned char[newlength];

	tmpInt64 = (v*(v-1))/2;//sum of previous columns
	cbyte = tmpInt64/8;//correct index: if evenly divided we start at next byte so
					   //there is no need to reduce by 1, else if it does not
					   //divide evenly, there is no need to reduce by 1 since the
					   //remainder and begining of column are in the next byte
	cbit = tmpInt64%8; //bit of byte to start ignoring
	//number of bits to remove from column = v
	
	for(unsigned int i=0; i<cbyte; i++)//copy whole bytes until begining of
	{								 //column to remove
		graph[i]=old[i];
	}
	unsigned char tmpUchar = 128;
	graph[cbyte] = 0;//is this necessary?
	for(int i=0; i<cbit; i++)//copy remainin g bits before beginning
	{
		//std::cout << int(old[cbyte]) << " " << int(tmpUchar) <<" " << int(tmpUchar&old[cbyte])<< std::endl; 
		//if(tmpUchar == tmpUchar&old[cbyte]) //NOTE:Unknown anomaly
		if(int(tmpUchar) == int(tmpUchar&old[cbyte]))
		{
			graph[cbyte] |=tmpUchar;
		}
		tmpUchar >>= 1;//shift right by 1
	}
	if(v == size - 1)//last vertex
	{
		size--;
		delete[] old;
		return;
	}

	unsigned int cbit2;
	unsigned int cbyte2;

	tmpInt64 = (((v+1)*v)/2); //bits including the column for v
	cbit2 = tmpInt64%8; //bit to start copying from
	cbyte2 = tmpInt64/8; //byte to start copying from

	
	unsigned char tmpUchar2 = 128 >> cbit2; 
	tmpUchar = 128 >> cbit;
	while(cbyte2 < length)
	{
		while(tmpUchar2 > 0)
		{
			if(int(tmpUchar2) == int(old[cbyte2]&tmpUchar2))
			{
				graph[cbyte] |= tmpUchar;
			}
			tmpUchar >>= 1;
			tmpUchar2 >>=
				1;
			if(tmpUchar == 0)
			{
				tmpUchar = 128;
				cbyte++;
			}
		}
		tmpUchar2 = 128;
		cbyte2++;
	}

	delete[] old;
	size--;
	return;
}



bool m3sgraph::SetGraph(std::istream& in)
{
	std::string str;
	std::getline(in,str);

	return SetGraph(str.c_str());
}
bool m3sgraph::SetGraph(const char* g6)
{
	size = g6::GetSize(g6);
	if(size==0)
		return false;
	
	length = size - 1;
	length = (length*(size))/2; //number of elements in upper matrix
	if(length%8 > 0)
		length = (length/8)+1;
	else
		length = (length/8);

	graph = new unsigned char[length];
	long long unsigned int byte = 0;
	unsigned char bit = 128;
	bool** matrix = g6::GetMatrix(g6);

	graph[byte]=0;
	for(long long unsigned int i=0; i<size; i++)
	{
		for(long long unsigned int j=0; j<i; j++)
		{
			if(bit==0)
			{
				bit = 128;
				byte++;
				graph[byte]=0;
			}
			if(matrix[j][i])
			{
				graph[byte] |= bit;
			}
			
			bit >>= 1;
		}
	}
	return true;
}
long long unsigned int m3sgraph::GetDeg(long long unsigned int a)
{
	long long unsigned int count = 0;
	for(long long unsigned int i=0; i<size; i++)
	{
		if(GetEdge(a,i))
		{
			count++;
		}
	}
	return count;
}
std::vector<long long unsigned int> m3sgraph::GetDegSeq()
{
	std::vector<long long unsigned int> seq;
	if(size==0)
		return seq;

	for(long long unsigned int i =0; i< size; i++)
	{
		seq.push_back(GetDeg(i));
	}
	

	return seq;
}
std::string m3sgraph::GetG6()
{
	if(size == 0)
		return "";
	
	long long unsigned int count = ((size*(size-1))/2);
	if(count%6==0)
		count = count/6;
	else
		count = (count/6)+1;

	long long unsigned int byte = 0;
	unsigned char bit;
	std::string g6;

	if(size < 63)
    {
        g6 += (char)(size+63);
    }else if (size < 258047)
    {
        g6 += (char)126;

        for(int i = 12; i>=0; i-=6)
        {
        bit = size >> i;
        g6 += (char)((bit & '?')+63);
        }
    }
    else
    {
        g6 += (char)126;
        g6 += (char)126;
        for(int i = 30; i>=0; i-=6)
        {
        bit = size >> i;
        g6 += (char)((bit & '?')+63);
        }
    }



	for(int i=0; i<count; i++)
	{
		switch(i%4)
		{
		case 0:
			bit = graph[byte];
			bit >>=2;
			g6+=char(bit+63);
			break;
		case 1:
			bit = graph[byte];
			bit <<= 4;
			byte++;
			bit |= graph[byte] >> 4;
			g6 +=char((bit&63)+63);
			break;
		case 2:
			bit = graph[byte];
			bit <<= 2;
			byte++;
			bit |= graph[byte] >> 6;
			g6 +=char((bit&63)+63);
			break;
		case 3:
			bit = graph[byte];
			g6 +=char((bit&63)+63);
			byte++;
			break;
		}
	}
return g6;	
}
long long unsigned int m3sgraph::GetSize()
{
	return size;
}

void m3sgraph::Draw(std::ostream& out)
{
	for(long long unsigned int j=0; j<size; j++)
	{
		for(long long unsigned int k=0; k<size; k++)
		{
			if(GetEdge(j,k))
				out << 1;
			else
				out <<0;
		}
		out << std::endl;
	}
	out <<"======================"<< std::endl;
}
void m3sgraph::PopulateNeighborlist()
{
     if(!neghborlistIsUpdated)
     {
     neighborlist.clear();
     neighborlist.resize(size,NULL);
     for(int i=0; i<size; i++)
     {
             neighborlist[i]=new NeighborData;
             neighborlist[i]->GetInfo(i,*this);
     }
     //neghborlistIsUpdated = true;
     }
}


bool NeighborData::GetInfo (int i, m3sgraph& g)
    {
      index = i;
      int size = g.GetSize();

      int* tmp = new int [size-1];
      neighbors=0; //neighbor count
      for(int n=0; n<size; n++)
      {
			if(g.GetEdge(i,n))
			{
			  tmp[neighbors] = n;
			  neighbors++;
			}
      }
      if(list!=NULL)
			delete[] list;
      list = new int[neighbors];
      for(int n=0; n<neighbors; n++)
      {
			list[n]=tmp[n];
      }
      delete[] tmp;
	return true;
    } 






#endif
