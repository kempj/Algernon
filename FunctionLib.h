#ifndef FUNCTIONS_LIB
#define FUNCTIONS_LIB 0

#include <vector>
#include <string>
#include <cmath>

typedef double(*BFPTR)(double,double);
typedef double(*UFPTR)(double);
//IMPLEMENTING FUNCTIONS***************
//Unary
double NEGATE(double r)
{
	return -1*r;
}
//binary
double ADD(double l, double r)
{
	return l + r;
}
double SUBTRACT(double l, double r)
{
	return l + r;
}
double MULTIPLY(double r, double l)
{
	return r*l;
}
double DIVIDE(double r, double l)
{
	return r/l;
}
double MOD(double r, double l)
{
	return (int(r)%int(l));
}
//*************************************
//DEFINING FUNCTIONS LIBRARY***********
union FunctionPtr
{
    double (*uptr) (double r); //pointer to unary funtion
	double (*bptr) (double l,double r); //pointer to binary function
};
struct FunctionDef
{
	std::string name;
	bool isBinary;
	FunctionPtr ptr;
};

class FunctionLib
{
private:
	std::vector<FunctionDef> list;
public:
	FunctionLib();
	int GetListSize();
	std::string GetFunctionName(int index);

	double GetFunction(std::string name, double r);
	double GetFunction(std::string name, double l, double r);
	double GetFunction(int index, double r);
	double GetFunction(int index, double l, double r);

	UFPTR GetFunctionPtr(std::string name);
	BFPTR GetFunctionPtr(std::string name, bool isBinary);
	UFPTR GetFunctionPtr(int index);
	BFPTR GetFunctionPtr(int index, bool isBinary);


	int GetFunctionIndex(std::string name, bool isBinary);
};
double FunctionLib::GetFunction(std::string name, double r)
{
	for(int i=list.size()-1; i>=0; i--)
	{
		if(list[i].name==name && list[i].isBinary==false)
		{
			return list[i].ptr.uptr(r);
		}
	}
}
double FunctionLib::GetFunction(std::string name, double l, double r)
{
	for(int i=list.size()-1; i>=0; i--)
	{
		if(list[i].name==name && list[i].isBinary==true)
		{
			return list[i].ptr.bptr(l,r);
		}
	}
}
double FunctionLib::GetFunction(int index, double r)
{
	return list[index].ptr.uptr(r);
}
double FunctionLib::GetFunction(int index, double l, double r)
{
	return list[index].ptr.bptr(l,r);
}


UFPTR FunctionLib::GetFunctionPtr(std::string name)
{
	for(int i=list.size()-1; i>=0; i--)
	{
		if(list[i].name==name && list[i].isBinary==false)
		{
			return list[i].ptr.uptr;
		}
	}
}
BFPTR FunctionLib::GetFunctionPtr(std::string name, bool isBinary)
{
	for(int i=list.size()-1; i>=0; i--)
	{
		if(list[i].name==name && list[i].isBinary==true)
		{
			return list[i].ptr.bptr;
		}
	}
}
UFPTR FunctionLib::GetFunctionPtr(int index)
{
	return list[index].ptr.uptr;
}
BFPTR FunctionLib::GetFunctionPtr(int index, bool isBinary)
{
	return list[index].ptr.bptr;
}


int FunctionLib::GetListSize()
{
	return list.size();
}
std::string FunctionLib::GetFunctionName(int index)
{
	return list[index].name;
}
int FunctionLib::GetFunctionIndex(std::string name, bool isBinary)
{
	for(int i=list.size()-1; i>=0; i--)
	{
		if(list[i].name==name && list[i].isBinary==isBinary)
		{
			return i;
		}
	}
	return -1;//not found
}
//**********************************
//  ADDING FUNCTIONS TO LIBRARY
//**********************************
FunctionLib::FunctionLib()
{
	FunctionDef tmp;

//unary
	tmp.name = "-";
	tmp.isBinary = false;		//string representation
	tmp.ptr.uptr=NEGATE;		//assign function to apropriate pointer (uptr - unary, bptr - binary)
	list.push_back(tmp);		//add to list


//binary
	tmp.name = "+";				
	tmp.isBinary = true;		//binary or unary
	tmp.ptr.bptr=ADD;			//assign function to apropriate pointer (uptr - unary, bptr - binary)
	list.push_back(tmp);		//add to list

	tmp.name = "-";
	tmp.isBinary = true;
	tmp.ptr.bptr=SUBTRACT;
	list.push_back(tmp);

	tmp.name = "*";
	tmp.isBinary = true;
	tmp.ptr.bptr=MULTIPLY;
	list.push_back(tmp);

	tmp.name = "/";
	tmp.isBinary = true;
	tmp.ptr.bptr=DIVIDE;
	list.push_back(tmp);

	tmp.name = "%";
	tmp.isBinary = true;
	tmp.ptr.bptr=MOD;
	list.push_back(tmp);
}
//********************************************
#endif
