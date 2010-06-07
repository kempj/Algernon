#ifndef ALGERNON_EXPRESSIONS
#define ALGERNON_EXPRESSIONS 0

#include<iostream>
#include<string>
#include<vector>
#include <sstream> //look into pro/con of strstream
#include <cmath>
#include"FunctionLib.h"

#define DOUBLE_ERROR 0.0000001

enum ExprType {VALUE=0,VARIABLE,BINARY,UNARY}; //type of expression building block

double stod(std::string str)
{
	double ans = 0;
	int i;
	double multiplier = 10;
	for(i=0; i<str.length() && str[i]!='.'; i++)
	{
		ans = ans*10 + str[i]-48;
	}
	if(str[i]=='.')
	{
		i++;
		while(i<str.length())
		{
			ans = ans + double(str[i]-48)/multiplier;
			multiplier*10;
			i++;
		}
	}

	return ans;
}

struct Variable //variable description
{
	std::string name;	//name
	bool isKnown;		//value is determined
	double Value;		//value
};

union ExprStepData //data pertaining to stepwise calculation of expressions 
{
	double val;		//explicit value
	Variable* var;	//variable pointer
	double (*uptr) (double r);	//unary function pointer
	double (*bptr) (double l, double r);	//binary function pointer
};

struct ExpressionStep	//description of calculation step in expressions
{
	ExprType type;		//type of computation
	ExprStepData data;	//data used to carry out computation
};
bool EvaluateExpressionVector (std::vector<ExpressionStep> &itemizedExpression, double ans, int index)
{
	if(itemizedExpression[index].type != VALUE)
	{
		//if variable
		if(itemizedExpression[index].type == VARIABLE)
		{
			if(!itemizedExpression[index].data.var->isKnown)
				return false; //error??
			else
			{
				itemizedExpression[index].data.val = itemizedExpression[index].data.var->Value;
				itemizedExpression[index].type = VALUE;
				return true;
			}
		}
		//if unary function
		if(itemizedExpression[index].type==UNARY)
		{
			double tmp;
			if(!EvaluateExpressionVector(itemizedExpression,tmp,index+1))
				return false;

			itemizedExpression[index].data.val = itemizedExpression[index].data.uptr(tmp);
			itemizedExpression[index].type = VALUE;
			return true;
		}
		//if unary function
		if(itemizedExpression[index].type==UNARY)
		{
			double tmp1,tmp2;
			if(!EvaluateExpressionVector(itemizedExpression,tmp1,index+1))
				return false;
			if(!EvaluateExpressionVector(itemizedExpression,tmp2,index+2))
				return false;
			itemizedExpression[index].data.val = itemizedExpression[index].data.bptr(tmp1,tmp2);
			itemizedExpression[index].type = VALUE;
			return true;
		}
	}
	
	ans = itemizedExpression[index].data.val;
	return true;
}




//PARSING EXPRESSIONS

struct Token
{
     std::string token;
     char type; //n = number | i = identifier | o = operator | e = empty | 0 = null 
                //l = left divider (i.e. parantheses or brackets) | r = right divider| c = comparison (i.e. ==, =>,...)
				//d = dividing comma
};

Token GetToken(std::istream& in); 
bool IsLetter(char ch){return ((ch >= 65 && ch <= 90)||(ch >= 97 && ch <= 122))?true:false;} 
bool TokenizeExpression (std::istream& in, std::vector<Token> &tokenList) //returns a std::vector steps used to calculate expression
{
	Token t; 
	tokenList.clear();
	t = GetToken(in);
	while(t.token!="")
	{
		tokenList.push_back(t);
		 t = GetToken(in);
	}
    return true;
}
Token GetToken(std::istream& in)
{
    Token t;
    char ch;
    in.get(ch);
    if(ch == ' ' || ch == '\t' || ch == '\r')//white space 
          return GetToken(in);
    else if(ch >=48 && ch <= 57)//starts with digit: is a number
    {
          t.type = 'n';
          t.token += ch;
          ch = in.peek();
          while(ch >=48 && ch <= 57)
          {
                in.get(ch);
                t.token += ch;
                ch = in.peek();
          }
          if(ch == '.')
          {
                in.get(ch);
                ch = in.peek();
                if(ch >=48 && ch <= 57)
                {
                      t.token += '.';
                      t.token += ch;
                      in.get(ch);
                      ch = in.peek();
                      while(ch >=48 && ch <= 57)
                      {
                             in.get(ch);
                             t.token += ch;
                             ch = in.peek();   
                      }
                      
                }
                else
                      in.putback('.');    
          }
    }
    else if(IsLetter(ch)||(ch == '_'))
    {//this token is an identifier
         t.type = 'i';
         t.token += ch;
         ch = in.peek();
         while(IsLetter(ch)||(ch == '_')||(ch >=48 && ch <= 57))
         {
              in.get(ch);
              t.token += ch;
              ch = in.peek();
         }
    }
    else if(!in)
    {//no data
         in.clear();
         t.token = "";
         t.type = '0';
    }
    else if(ch == '\n')
    {//end of data
         t.token = "";
         t.type = '0'; 
    }
    else if(ch == '-' || ch == '+' || ch == '\\' ||
            ch == '/' || ch == '*' || ch == '%' )
    {
         t.token = ch;
         t.type = 'o';
    } 
	else if(ch == ',' )
    {
         t.token = ch;
         t.type = 'd';
    }
    else if(ch == '(' || ch == '[' || ch == '{')
    {
         t.token = ch;
         t.type = 'l';
    }
	else if( ch == ')' || ch == ']' || ch == '}')
    {
         t.token = ch;
         t.type = 'r';
    }
	else if(ch == '>' || ch == '<')
	{
		t.token = ch;
		t.type = 'c';
		ch = in.peek();
		if(ch == '=') // >= & <=
		{
			t.token+=ch;
			in.get(ch);
		}

	}
	else if(ch == '=' || ch == '!')
	{
		/*original code bottom of file*/
		t.token += ch;
		ch = in.peek();
		if(ch=='=')
		{
			t.token+=ch;
			t.type = 'c';
			in.get(ch);
		}
		else
		{
			 t.token = "0xERROR"; //error token: not valid comparison
			 t.type = 'e';
		}				
	}
    else
    {
         t.token = "0xERROR"; //error token: not valid number or identifier
         t.type = 'e';
    }   
    return t;
}



std::vector<ExpressionStep> ExpressionToPrescript(FunctionLib& flib,std::vector<Token>& list,int start,int end);


class ExpressionClass
{
private:
	FunctionLib flib;
	std::string raw;
	std::vector<Token> tokenized;
	std::vector<ExpressionStep> LHS;
	std::vector<ExpressionStep> RHS;
	Token comparison;
	double LHS_val;
	double RHS_val;
	bool varsLinked;



	bool Evaluate(std::vector<Variable>& varList, std::vector<ExpressionStep>& side, double& ans );
	bool Evaluate(std::vector<ExpressionStep>& side, double& ans );

public:
	ExpressionClass();
	bool Set(std::istream& in);
	bool Set(std::string str);
	double EvaluateLHS(std::vector<Variable>& varList);
	double EvaluateRHS(std::vector<Variable>& varList);
	double EvaluateLHS();
	double EvaluateRHS();
	double GetLHS(){return LHS_val;} //most recently computed value
	double GetRHS(){return RHS_val;} //most recently computed value
	std::string GetRaw(){return raw;}
	std::string GetComparisson(){return comparison.token;}
	void GetVarList(std::vector<Variable*>& varList);
	bool GetSideComparison(double& LHS_minus_RHS);
	void GetVarLinks(std::vector<Variable*> &list, bool LHS_listing);

};
ExpressionClass::ExpressionClass(){varsLinked = false;}
void ExpressionClass::GetVarLinks(std::vector<Variable*> &list, bool LHS_listing)
{
	list.clear();//empty list
	std::vector<ExpressionStep>* side;
	if(LHS_listing)
		side = &LHS;
	else
		side = &RHS;

	int j;

	for(int i=0; i<side->size(); i++)
	{
		if((*side)[i].type==VARIABLE)
		{
			for(j=list.size()-1; j>=0; j--)
			{
				if(list[j]==(*side)[i].data.var)
					j=-2;
			}//will exit with j = -1 or j = -3
			if(j>-2)//esstentially j=-1, this statement is cautionary
			{
				list.push_back((*side)[i].data.var);
			}
		}
	}

}
bool ExpressionClass::GetSideComparison(double& LHS_minus_RHS)
{
	LHS_minus_RHS = LHS_val - RHS_val;
	double tmpdbl = std::fabs(LHS_minus_RHS);
	if(comparison.token==">")
	{
		if(tmpdbl > DOUBLE_ERROR && LHS_minus_RHS > 0)
			return true;
		return false;
	}
	if(comparison.token=="<")
	{
		if(tmpdbl > DOUBLE_ERROR && LHS_minus_RHS < 0)
			return true;
		return false;
	}
	if(comparison.token==">=")
	{
		if(tmpdbl <= DOUBLE_ERROR || LHS_minus_RHS > 0)
			return true;
		return false;
	}
	if(comparison.token=="<=")
	{
		if(tmpdbl <= DOUBLE_ERROR || LHS_minus_RHS < 0)
			return true;
		return false;
	}
	if(comparison.token=="==")
	{
		if(tmpdbl < DOUBLE_ERROR)
			return true;
		return false;
	}
	if(comparison.token=="!=")
	{
		if(tmpdbl > DOUBLE_ERROR)
			return true;
		return false;
	}

	return false;
}
void ExpressionClass::GetVarList(std::vector<Variable*>& varList)
{
	int j = 0;
	for(int i=LHS.size()-1; i>=0; i--)
	{
		if(LHS[i].type == VARIABLE)
		{
			for(j=varList.size()-1; j>=0; j--)
			{
				if(varList[j]->name==LHS[i].data.var->name)
				{
					LHS[i].data.var = varList[varList.size()-1]; //link to vriable
					j=-2;
				}
			}
			if(j!=-3)
			{
				varList.resize(varList.size()+1);
				varList[varList.size()-1]=new Variable();
				varList[varList.size()-1]->name = LHS[i].data.var->name;
				varList[varList.size()-1]->isKnown = LHS[i].data.var->isKnown;
				varList[varList.size()-1]->Value = LHS[i].data.var->Value;
				LHS[i].data.var = varList[varList.size()-1];
			}
			
		}
	}

	for(int i=RHS.size()-1; i>=0; i--)
	{
		if(RHS[i].type == VARIABLE)
		{
			for(j=varList.size()-1; j>=0; j--)
			{
				if(varList[j]->name==RHS[i].data.var->name)
				{
					RHS[i].data.var = varList[varList.size()-1]; //link to vriable
					j=-2;
				}
			}
			if(j!=-3)
			{
				varList.resize(varList.size()+1);
				varList[varList.size()-1]=new Variable();
				varList[varList.size()-1]->name = RHS[i].data.var->name;
				varList[varList.size()-1]->isKnown = RHS[i].data.var->isKnown;
				varList[varList.size()-1]->Value = RHS[i].data.var->Value;
				RHS[i].data.var = varList[varList.size()-1];
			}
			
		}
	}

	varsLinked = true;
}
double ExpressionClass::EvaluateLHS(std::vector<Variable>& varList)
{
	Evaluate(varList, LHS, LHS_val);
	return LHS_val;
}
double ExpressionClass::EvaluateRHS(std::vector<Variable>& varList)
{
	Evaluate(varList, LHS, LHS_val);
	return LHS_val;
}
double ExpressionClass::EvaluateLHS()
{
	Evaluate(LHS, LHS_val);
	return LHS_val;
}
double ExpressionClass::EvaluateRHS()
{
	Evaluate(RHS, RHS_val);
	return RHS_val;
}


bool ExpressionClass::Evaluate(std::vector<Variable>& varList, std::vector<ExpressionStep>& side, double& ans)
{
	std::vector<ExpressionStep> temp = side;

	//replace all varaibles with values
	for(int i=temp.size()-1; i>=0; i--)
	{
		if(temp[i].type == VARIABLE)
		{
			for(int j=varList.size()-1; j>=0; j--)
			{
				if(varList[j].name == temp[i].data.var->name)
				{
					if(!varList[j].isKnown)
						return false;
					temp[i].type = VALUE;
					temp[i].data.val=varList[j].Value;
					j=-1;
				}
			}
			if(temp[i].type==VARIABLE) //no match found
				return false;
		}
	}

	while(temp.size()>1)
	{
		//evaluate unaries and binaries that are possible
		for(int i=0; i<temp.size(); i++)
		{
			if(temp[i].type == BINARY)
			{
				if(temp[i+1].type== VALUE && temp[i+2].type==VALUE)
				{
					temp[i].type=VALUE;
					temp[i].data.val= temp[i].data.bptr(temp[i+1].data.val,temp[i+1].data.val);
					temp.erase(temp.begin()+i+1,temp.begin()+i+2);
					i=0;
				}
			}
			else if(temp[i].type == UNARY)
			{
				if(temp[i+1].type== VALUE)
				{
					temp[i].type=VALUE;
					temp[i].data.val= temp[i].data.uptr(temp[i+1].data.val);
					temp.erase(temp.begin()+i+1);
					i=0;
				}
			}
		}
	}

	ans = temp[0].data.val;
	return true;
}
bool ExpressionClass::Evaluate(std::vector<ExpressionStep>& side, double& ans)//use only if variable list was acquired thru class
{
	std::vector<ExpressionStep> temp = side;

	//replace all varaibles with values
	for(int i=temp.size()-1; i>=0; i--)
	{
		if(temp[i].type == VARIABLE)
		{
			temp[i].type = VALUE;
			temp[i].data.val=temp[i].data.var->Value;
		}
	}

	while(temp.size()>1)
	{
		//evaluate unaries and binaries that are possible
		for(int i=0; i<temp.size(); i++)
		{
			if(temp[i].type == BINARY)
			{
				if(temp[i+1].type== VALUE && temp[i+2].type==VALUE)
				{
					temp[i].type=VALUE;
					temp[i].data.val= temp[i].data.bptr(temp[i+1].data.val,temp[i+2].data.val);
					temp.erase(temp.begin()+i+1,temp.begin()+i+3);
					i=-1;
				}
			}
			else if(temp[i].type == UNARY)
			{
				if(temp[i+1].type== VALUE)
				{
					temp[i].type=VALUE;
					temp[i].data.val= temp[i].data.uptr(temp[i+1].data.val);
					temp.erase(temp.begin()+i+1);
					i=-1;
				}
			}
		}
	}

	ans = temp[0].data.val;
	return true;
}

bool ExpressionClass::Set(std::istream& in)
{
	std::string str;
	getline(in,str);

	return Set(str);
}
bool ExpressionClass::Set(std::string str)
{
	std::stringstream ss(std::stringstream::in);
	raw = str;
	ss.str(raw);
	if(TokenizeExpression(ss, tokenized))
	{
		//find comparison op
		for(int i=0; i<tokenized.size(); i++)
		{
			if(tokenized[i].type == 'c')
			{
				if(i==0||i==tokenized.size()-1) //empty LHS or RHS
					return false;
				comparison = tokenized[i];
				LHS = ExpressionToPrescript(flib,tokenized,0,i);
				RHS = ExpressionToPrescript(flib,tokenized,i+1,tokenized.size());	

				for(int j=0; j<LHS.size(); j++)
				{
					switch(LHS[j].type)
					{//VALUE=0,VARIABLE,BINARY,UNARY
					case 0:
						std::cout << LHS[j].data.val<<" ";break;
					case 1:
						std::cout << LHS[j].data.var->name << " ";break;
					case 2:
						std::cout << "BINARY" << " "; break;
					case 3:
						std::cout << "UNARY" << " "; break;
					}
				}
				std::cout<<std::endl;
				for(int j=0; j<RHS.size(); j++)
				{
					switch(RHS[j].type)
					{//VALUE=0,VARIABLE,BINARY,UNARY
					case 0:
						std::cout << RHS[j].data.val<<" ";break;
					case 1:
						std::cout << RHS[j].data.var->name << " ";break;
					case 2:
						std::cout << "BINARY" << " "; break;
					case 3:
						std::cout << "UNARY" << " "; break;
					}
				}
				std::cout<<std::endl;
			}
		}
		if(comparison.type = '0')//no comparison found
		{
			return false;
		}
		return true;
	}
	return false;
}










//std::vector<ExpressionStep> ExpressionToPrescript(std::vector<Token>& list,int start,int end);
struct IntermediateComponent
{
	int start;
	int end;
};
std::vector<ExpressionStep> ExpressionToPrescript(FunctionLib& flib, std::vector<Token>& list,int start,int end)
{
	std::vector<ExpressionStep> prescript;
	ExpressionStep tmp;
	std::vector<std::vector<Token> > statements;
	//TOKEN TYPES
	//n = number | i = identifier | o = operator | e = empty | 0 = null 
    //l = left divider (i.e. parantheses or brackets) | r = right divider| c = comparison (i.e. ==, =>,...)
	//d = dividing comma
	
	int counter = 0;
	int lastop = 0;
	for(int i=0; start+i<end; i++)
	{
		if(list[start+i].type == 'l')
		{
			counter++;
		}
		if(list[start+i].type == 'r')
		{
			counter--;
		}
		if(counter==0 && list[start+i].type == 'o')
		{
			if(i!=0)
			{
				//not unary
				//everything before is a statement
				statements.resize(statements.size()+1);
				if(lastop!=0)
				{
					for(int j=lastop - start + 1; j<i; j++)
					{
						statements[statements.size()-1].push_back(list[start+j]);
					}
				}
				else
				{
					for(int j=0; j<i; j++)
					{
						statements[statements.size()-1].push_back(list[start+j]);
					}
				}
				statements.resize(statements.size()+1);
					statements[statements.size()-1].push_back(list[start+i]);
				lastop=start + i;
			}			
		}
	}
	statements.resize(statements.size()+1);
	if(lastop>0)
	{
		for(int j=lastop+1; j<end; j++)
		{
			statements[statements.size()-1].push_back(list[j]);
		}
	}else
	{
		for(int j=start; j<end; j++)
		{
			statements[statements.size()-1].push_back(list[j]);
		}
	}

	Token tmpToken;
	//move all high precedence ops up by 1
	for(int i=statements.size()-1; i>=0; i--)
	{
		if(statements[i].size()==1&&statements[i][0].type=='o')
		{
			if(statements[i][0].token=="*" || statements[i][0].token=="/" || statements[i][0].token=="%")
			{
				statements.insert(statements.begin()+i-1,statements[i]);
				statements.erase(statements.begin()+i+1);
				i--;
			}
		}
	}
	//move all precedence up by 1 (over and high precedence ops)
	for(int i=statements.size()-1; i>=0; i--)
	{
		if(statements[i].size()==1&&statements[i][0].type=='o')
		{
			if(statements[i][0].token=="+" || statements[i][0].token=="-")
			{
				int k=i-1;
				while(k>=0)
				{
					if(k==0 || (statements[k].size()==1&&statements[k][0].type=='o'))
					{
						statements.insert(statements.begin()+k,statements[i]);
						statements.erase(statements.begin()+i+1);
						i=k-1;
						k=-1;
					}
					k--;
				}
				
			}
		}
	}
	
	//start turning into ExpressionStep types
	for(int i=0; i<statements.size(); i++)
	{
		if(statements[i].size()==1&&statements[i][0].type=='o')//binary operator
		{
			prescript.resize(prescript.size()+1);
			prescript[prescript.size()-1].type=BINARY;
			prescript[prescript.size()-1].data.bptr = flib.GetFunctionPtr(statements[i][0].token, true);
		}else
		{
			std::vector<ExpressionStep> tmpES;
			for(int j=0; j<statements[i].size(); j++)
			{
				if(statements[i][j].type=='o')//unary op
				{
					prescript.resize(prescript.size()+1);
					prescript[prescript.size()-1].type=UNARY;
					prescript[prescript.size()-1].data.uptr = flib.GetFunctionPtr(statements[i][j].token);
				}
				else if(statements[i][j].type=='n')
				{
					prescript.resize(prescript.size()+1);
					prescript[prescript.size()-1].type=VALUE;
					prescript[prescript.size()-1].data.val = stod(statements[i][j].token);//<--- string to double needed
				}
				else if(statements[i][j].type=='i')
				{
					if(j==statements[i].size()-1 || statements[i][j+1].type!='l') //variable
					{
						prescript.resize(prescript.size()+1);
						prescript[prescript.size()-1].type=VARIABLE;
						prescript[prescript.size()-1].data.var =new Variable;
						prescript[prescript.size()-1].data.var->isKnown=false;
						prescript[prescript.size()-1].data.var->name=statements[i][j].token;
					}
					else//function
					{
						prescript.resize(prescript.size()+1);
						int pcount = 1;
						for(int k=j+2; k<statements[i].size()&&pcount>0; k++)
						{
							if(statements[i][k].type=='l')
								pcount++;
							if(statements[i][k].type=='l')
								pcount--;
							if(pcount==1&&statements[i][k].type=='d')
							{
								prescript[prescript.size()-1].type=BINARY;
								prescript[prescript.size()-1].data.bptr = flib.GetFunctionPtr(statements[i][j].token,true);
								
								tmpES = ExpressionToPrescript(flib,statements[i],j+2,k);
								prescript.insert(prescript.end(),tmpES.begin(),tmpES.end());

								int comma = k;
 
								while(pcount>0)
								{
									k++;
									if(statements[i][k].type=='l')
										pcount++;
									if(statements[i][k].type=='l')
										pcount--;
								}
								tmpES = ExpressionToPrescript(flib,statements[i],comma+1,k);
								prescript.insert(prescript.end(),tmpES.begin(),tmpES.end());
								j=k+1;
								
							}else if (pcount==0)
							{
								prescript[prescript.size()-1].type=UNARY;
								prescript[prescript.size()-1].data.uptr = flib.GetFunctionPtr(statements[i][j].token);
								
								tmpES = ExpressionToPrescript(flib,statements[i],j+2,k);
								prescript.insert(prescript.end(),tmpES.begin(),tmpES.end());
								j=k+1;
							}
							
								
						}
					}
				}
				else if(statements[i][j].type=='l')
				{
					int startingpoint = j+1;
					int pcount = 1; 
					while(pcount>0)
					{
						j++;
						if(statements[i][j].type=='l')
							pcount++;
						if(statements[i][j].type=='r')
							pcount--;
					}
					tmpES = ExpressionToPrescript(flib,statements[i],startingpoint,j);
					prescript.insert(prescript.end(),tmpES.begin(), tmpES.end());
					j++;
				}
			}
		}

	}

	return prescript;
} 

#endif


/*

t.token += ch;
		ch = in.peek();
		if(t.token=="=")
		{
			if(ch=='>' || ch=='<' || ch=='=')
			{
				t.token+=ch;
				t.type = 'c';
				in.get(ch);
			}
			else
			{
				 t.token = "0xERROR"; //error token: not valid number or identifier
				 t.type = 'e';
			}

		}
		else
		{
			if(ch=='=')
			{
				t.token+=ch;
				t.type = 'c';
				in.get(ch);
			}
			else
			{
				 t.token = "0xERROR"; //error token: not valid number or identifier
				 t.type = 'e';
			}
		}	



*/
