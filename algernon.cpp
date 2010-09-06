#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>
#include <time.h>
#include "graph.h"
#include "AlgeronExpressions.h"
#include "graphalterations.h"
#include "graphinvariants.h"

#define DOUBLE_ERROR 0.0000001
#define SHIFT_SIZE 0.00001
#define BEST_MATCH_TOLERANCE 0.90 //90%
#define BEST_CHANGE_TOLERANCE 0.1 //10% more/less is acceptable
#define BEST_ARG_TOLERANCE 0.10 //90% more/less is acceptable

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

bool VerifyVarList(vector<Variable*>& varList, InvariantLib &ilib);
void PopulateVarList(vector<Variable*>& varList, InvariantLib &ilib, m3sgraph &g);
void GetExpression(vector<ExpressionClass>& expr_list,InvariantLib& ilib, istream& in);
void BuildShiftData(vector<vector<vector<vector<vector<double> > > > > &shift_data,
		    vector<ExpressionClass> &expr_list,
		    vector<Variable*> &var_list);
int ExpressionsTruthValue(vector<vector<vector<vector<vector<double> > > > > &shift_data,
			  vector<ExpressionClass> &expr_list,vector<Variable*> &var_list,
			  vector<double>& change);

#ifdef _WIN32
void ClearScreen()
{
    system("cls");
}
#else
void ClearScreen()
{
    system("clear");
}
#endif

int main()
{
    AlterationLib alib;/*!< The library containing all of the Alterations*/
    InvariantLib ilib; /*!< The library containing all of the Invariants*/
    vector<ExpressionClass> expr_list;/*!< A vector containing all of the expressions the user inputs*/
    m3sgraph coreg, newg, currentg, lastg;
    int tmpInt = 0;
    string tmpStr;
    vector<vector<vector<vector<vector<double> > > > > shift_data;
    vector<Variable*> var_list;
    
    //This doWhile loop Obtains the Expressions to be used
    do
    {
	ClearScreen();
	
	GetExpression(expr_list,ilib, cin);
	cout << "\n\nEntered expressions:\n\n";
	for(int i=0; i<expr_list.size(); i++)
	{
	    cout << '\t' << expr_list[i].GetRaw() << endl;
	}
	cout << "\n\nAnother expression? Enter 1 for YES & 0 for NO: ";
	cin >> tmpInt;
	cin.get();//cleans remaining newline character in stream
    }while(tmpInt == 1);
    
    //create a vector for variables
    for(int i=0; i<expr_list.size(); i++)
    {
	expr_list[i].GetVarList(var_list); //get & link variables
    }
    
    //Inputting Seed Grap and creating graph object from it.
    cout << "\nPlease enter the G6 name for starting graph: ";
    coreg.SetGraph(cin);//get graph from stdin
    cout << endl;

    string lprefix = "";
    cout << "\nPlease enter the prefix used as the directory name in garden: ";
    getline(cin,lprefix);
    
    if(!VerifyVarList(var_list, ilib))
    {
	cout << "Unknown invariants\n";
	return 0;
    }
    PopulateVarList(var_list, ilib, coreg);
    
    cout << "Base invariants for " << coreg.GetG6()<< ":\n";
    for(int i=0; i<var_list.size(); i++)
    {
	cout << var_list[i]->name << " : " << var_list[i]->Value << "\n";
    }
    cout << "\n\n";
    
    //This loop does the initial evaluation of the expression
    for(int i=0; i<expr_list.size(); i++)
    {
	cout << "Expression #" << i <<":\n";
	//expr.EvaluateLHS(var_list);

	cout << "~LHS: " <<  expr_list[i].EvaluateLHS() << "\n";
	//use this form only if expr.Getvar_list was used

	cout << "~RHS: " <<  expr_list[i].EvaluateRHS() << "\n";
	//use this form only if expr.Getvar_list was used
	cout << endl;
    }
    
    newg = coreg;//coreg is never used again, why have newg and coreg?*/
    currentg = newg;
    BuildShiftData(shift_data,expr_list,var_list);
    vector<double> required_changes;


    string garden = "garden/";
    string list = "list00.pgl";

    string tmpStr1, tmpStr2, tmpStr3;
    fstream file;
    int garden_var_count;
    vector<vector<double> > data_table;
    
    //PREP DATA SOURCE
    //get invariant names and create matchup table
    vector<int> invariant_map;
    vector<string> alteration_names;
    tmpStr1 = garden + lprefix + list;
    file.open(tmpStr1.c_str(), ios::in);
    file >> garden_var_count;//The number of invariants, stored in the top line of the .pgl file
    file.get();// file.get() remove residual newline marker
    int tmpInt1;
    if(var_list.size()>garden_var_count)//not enough varaibles in garden
    {
	cout << "Not enough invariant data avialable\n";
	return 1;
    }
    tmpInt = 0;

    //This loop compares every element in the garden data with every element of the invariant lib, storing
    // the index
    // Any that are in garden, but not in the lib have a -1 put in their place in the invariant map.
    for(int i=0; i<garden_var_count; i++)
    {
	getline(file,tmpStr1);//tmpStr1 is resued for a different purpose here(Invariant names).
	for(tmpInt1=0; tmpInt1<var_list.size(); tmpInt1++)
	{
	    if(var_list[tmpInt1]->name==tmpStr1)
	    {
		invariant_map.push_back(tmpInt1);
		tmpInt1 = var_list.size()+1;
		tmpInt++;//count mapped invariants
	    }
	}
	if(tmpInt1==var_list.size())//no match = unknow variable
	{
	    invariant_map.push_back(-1);
	    cout << "Unused invariant: " << tmpStr1 << "\n";
	}
    }

    if(var_list.size()>tmpInt)//not enough varaibles in garden
    {
	cout << "Not enough relevant invariant data avialable\n";
	return 1;
    }
    
    cout << endl;
    
    //get graph data
    double tmpDbl1;
    file >> tmpDbl1;

    //This loop is not entered.
    //This loop cycles through the pgl file creating a table of references to map
    //changes, keeping track of how a change of one invariant effects the others?

    /*while(file)
    {
	data_table.resize(data_table.size()+1);
	data_table[data_table.size()-1].resize(garden_var_count);
	data_table[data_table.size()-1][0] = tmpDbl1;
	for(int i=1; i<garden_var_count; i++)
	{
	    file >> data_table[data_table.size()-1][i];
	}
	//The following 2 lines in Garden are not written or being read.
	file >> tmpStr2; //graph name (currently unused)
	file >> tmpDbl1;
    }*/
    file.close();
    file.clear();

    int args[4];
    int hi_match = 0;
    double hi_score = 0;
    double tmp_score = 0;
    int first_loop = 0;

    //main while loop, alterations are done in here until an result is found
    while(ExpressionsTruthValue(shift_data,expr_list,var_list,required_changes)>0)
    {
	for(int i=0; i<expr_list.size(); i++)
	{
	    cout << "LHS: " <<  expr_list[i].EvaluateLHS();
	    cout << "\nRHS: " <<  expr_list[i].EvaluateRHS() << endl;
	}

	/* Removed because it is not called.
	//compare graph to exisitng profiles and pick best match
	for(int i=0; i<data_table.size(); i++)
	{
	    tmp_score=0;
	    for(int j=0; j<invariant_map.size(); j++)
	    {
		if(invariant_map[j]==-1)
		    continue;
		//check for BEST_MATCH_TOLERANCE
		if((data_table[i][j]> BEST_MATCH_TOLERANCE*var_list[invariant_map[j]]->Value)&&
		   (data_table[i][j]< (2-BEST_MATCH_TOLERANCE)*var_list[invariant_map[j]]->Value))
		{
		    tmp_score++;
		}

	    }
	    if(tmp_score>hi_score)
	    {
		hi_match = i;
		hi_score = tmp_score;
	    }
	}*/

	//check profile for best matching change in required invariants
	tmpStr1 = itos(hi_match);
	string graph_number = tmpStr1;
	//~~~~~~~~Reading in the GAP file~~~~~~~~~~~~
	tmpStr3 = garden + lprefix + tmpStr1 +".gap";
	file.open(tmpStr3.c_str(), ios::in);
	hi_match = 0;
	hi_score = 0;
	tmp_score = 0;
	int count = 0;
	file >> tmpStr1; //consumes graph name
	file >> tmpStr1; //gets first alteration method name

	//This loop Scores the alterations for each invariant based on how it impacts the total expression
	while(file)
	{
	    tmp_score=0;
	    alteration_names.push_back(tmpStr1);
	    for(int i=0; i<invariant_map.size(); i++)
	    {
		file >> tmpDbl1;
		if(invariant_map[i]==-1)
		    continue;
		if(required_changes[invariant_map[i]]*tmpDbl1 < 0)//different signs
		{
		    tmp_score-= (fabs(required_changes[invariant_map[i]])+fabs(tmpDbl1));
		}
		else if(fabs(required_changes[invariant_map[i]]-tmpDbl1)/required_changes[invariant_map[i]] >= BEST_CHANGE_TOLERANCE)
		{
		    tmp_score+=tmpDbl1;
		}
		else if(required_changes[invariant_map[i]]<tmpDbl1)
		{
		    tmp_score+=required_changes[invariant_map[i]];
		}
		else
		{
		    tmp_score+=tmpDbl1;
		}
	    }
	    if(tmp_score>hi_score && count>0)
	    {
		hi_match = alteration_names.size()-1;
		hi_score = tmp_score;
	    }else if(count ==0)//prone to error if file is empty
	    {
		hi_score = tmp_score;
		hi_match = alteration_names.size()-1;
	    }
	    file >> tmpStr1; //Consumes Alteration Name
	    count++;
	}
	file.close();
	file.clear();
	
	string alteration = alteration_names[hi_match];
	//~~~~~~~~~~~~~Reading in the gar file~~~~~~~~~~~~
	tmpStr1 = garden + lprefix + graph_number + "/" + alteration + ".gar";
	file.open(tmpStr1.c_str(), ios::in);

	//NOTE: This is a temporary fix
	for(int i=0; i<4; i++)//assigns 1 to all arguments
	    args[i]=1;
	count = 0;
	file >> tmpDbl1;
	
	//This loop determines the arguments for the Alteration?
	while(file)
	{
	    tmp_score=0;

	    if(invariant_map[0]!=-1)
	    {
		if(required_changes[invariant_map[0]]*tmpDbl1 < 0)//different signs
		{
		    tmp_score-= (fabs(required_changes[invariant_map[0]])+fabs(tmpDbl1));
		}
		else if(fabs(required_changes[invariant_map[0]]-tmpDbl1)/required_changes[invariant_map[0]] >= BEST_ARG_TOLERANCE)
		{
		    tmp_score+=tmpDbl1;
		}
		else if(required_changes[invariant_map[0]]<tmpDbl1)
		{
		    tmp_score+=required_changes[invariant_map[0]];
		}
		else
		{
		    tmp_score+=tmpDbl1;
		}
	    }

	    for(int i=1; i<invariant_map.size(); i++)
	    {
		file >> tmpDbl1;
		if(invariant_map[i]==-1)
		    continue;
		if(required_changes[invariant_map[i]]*tmpDbl1 < 0)//different signs
		{
		    tmp_score-= (fabs(required_changes[invariant_map[i]])+fabs(tmpDbl1));
		}
		else if(fabs(required_changes[invariant_map[i]]-tmpDbl1)/required_changes[invariant_map[i]] >= BEST_ARG_TOLERANCE)
		{
		    tmp_score+=tmpDbl1;
		}
		else if(required_changes[invariant_map[i]]<tmpDbl1)
		{
		    tmp_score+=required_changes[invariant_map[i]];
		}
		else
		{
		    tmp_score+=tmpDbl1;
		}
	    }
	    if(tmp_score>hi_score && count>0) //>= so that the array is initialized to a valid arg set
	    {
		for(int i=0; i<4; i++)
		{
		    file >> args[i];
		}
		hi_score = tmp_score;
	    }
	    else if (count==0)
	    {
		for(int i=0; i<4; i++)
		{
		    file >> args[i];
		}
		hi_score = tmp_score;
	    }
	    else
	    {
		for(int i=0; i<4; i++)
		{
		    file >> tmpInt; //discard args
		}
	    }
	    file >> tmpStr1; //discard graph name
	    file >> tmpDbl1; //first number on next line
	}
	file.close();
	file.clear();//JK - shouldn't this be use only if the file fails to open?
	
	newg = alib.DoAlteration(alteration,currentg,args[0],args[1],args[2],args[3]);

	//~~~~~~ Actual Alteration:
	cout << "Altered " << currentg.GetG6() << " using " << alteration << " obtaining " << newg.GetG6() << "\n";

	//Termination Condition - Failure
	if(first_loop != 0 && lastg == currentg && currentg == newg)
	{
	    cout << "\n\nGraphs Not Changing, Program Terminated\n";
	    return 0;
	}

	first_loop = 1;
	lastg = currentg;
	currentg = newg;

	PopulateVarList(var_list,ilib,currentg);

	cout << "Base invariants for " << currentg.GetG6()<< ":\n";
	for(int i=0; i<var_list.size(); i++)
	{
	    cout << var_list[i]->name << " : " << var_list[i]->Value << "\n";
	}
	cout << "\n\n";
	

    }

    cout << "Result: " << newg.GetG6() << endl;
    return 0;
}

void GetExpression(vector<ExpressionClass>& expr_list,InvariantLib& ilib, istream& in)
{
    ExpressionClass expr;
    if(in==cin)
    {
	cout << "Acceptable invariants are: \n";
	for(int i=ilib.GetListSize()-1; i>=0;i--)
	{
	    cout << ilib.GetInvariantName(i) << "\n";
	}
	cout << "\nPlease enter desired expression: ";
    }
    expr.Set(in);
    expr_list.push_back(expr);
}

bool VerifyVarList(vector<Variable*>& varList, InvariantLib &ilib)
{
    for(int i=0; i<varList.size(); i++)
    {
	cout << varList[i]->name << endl;
	if(ilib.GetIndex(varList[i]->name)==-1)
	    return false;
    }
    return true;
}

//populates the the values of invariants for a given graph?
void PopulateVarList(vector<Variable*>& varList, InvariantLib &ilib, m3sgraph &g)
{
    for(int i=0; i<varList.size(); i++)
    {
	varList[i]->Value = ilib.GetInvariant(varList[i]->name,g);
	varList[i]->isKnown = true;
    }
}

//This builds a data structure that stores the stats built from garden
void BuildShiftData(vector<vector<vector<vector<vector<double> > > > > &shift_data,
		    vector<ExpressionClass> &expr_list,
		    vector<Variable*> &var_list)
{
    //Shift Study
    shift_data.resize(var_list.size());//variables
    for(int i=0; i< shift_data.size(); i++)
    {
	shift_data[i].resize(expr_list.size());//expression
	for(int j=0; j< shift_data[i].size(); j++)
	{
	    shift_data[i][j].resize(2);//LHS & RHS
	    for(int k=0; k< shift_data[i][j].size(); k++)
	    {
		shift_data[i][j][k].resize(2);//positive & negative change
		for(int l=0; l< shift_data[i][j][k].size(); l++)
		{
		    shift_data[i][j][k][l].resize(2);//change in value & change in side
		}
	    }
	}
    }
    
    //Preload starting data
    for(int i=0; i<var_list.size(); i++)
    {
	for(int j=0; j<expr_list.size(); j++)
	{
	    shift_data[i][j][0][0][1] = expr_list[j].GetLHS(); //LHS - positive
	    shift_data[i][j][0][1][1] = expr_list[j].GetLHS(); //LHS - negative
	    shift_data[i][j][1][0][1]= expr_list[j].GetRHS(); //RHS
	    shift_data[i][j][1][1][1]= expr_list[j].GetRHS(); //RHS
	}
    }
    //do study
    for(int i=0; i<var_list.size(); i++)
    {
	var_list[i]->Value+=SHIFT_SIZE; //positive change
	for(int j=0; j<expr_list.size(); j++)
	{
	    shift_data[i][j][0][0][1] = expr_list[j].EvaluateLHS() - shift_data[i][j][0][0][1]; //LHS
	    shift_data[i][j][1][0][1] = expr_list[j].EvaluateRHS() - shift_data[i][j][1][0][1]; //RHS
	    shift_data[i][j][0][0][0] = SHIFT_SIZE; //RHS-extent of change
	    shift_data[i][j][1][0][0] = SHIFT_SIZE; //RHS
	}
	var_list[i]->Value-=2*SHIFT_SIZE; //negative change
	for(int j=0; j<expr_list.size(); j++)
	{
	    shift_data[i][j][0][1][1] = expr_list[j].EvaluateLHS() - shift_data[i][j][0][1][1]; //LHS
	    shift_data[i][j][1][1][1] = expr_list[j].EvaluateRHS() - shift_data[i][j][1][1][1]; //RHS
	    shift_data[i][j][0][1][0] = -1*SHIFT_SIZE; //RHS
	    shift_data[i][j][1][1][0] = -1*SHIFT_SIZE; //RHS
	}
	var_list[i]->Value+=SHIFT_SIZE; //return to original value
    }
    //print study
    cout << "\n\n";
    for(int i=0; i<expr_list.size(); i++)
    {
	cout << "For : \n" << expr_list[i].GetRaw() << "\n\n";
	for(int j=0; j<var_list.size(); j++)
	{
	    cout << "- if " << var_list[j]->name << " increases by "
		    << shift_data[j][i][0][0][0] << ", the LHS ";

	    if(fabs(shift_data[j][i][0][0][1])>DOUBLE_ERROR)
	    {
		if(shift_data[j][i][0][0][1]>0)
		    cout << "increases.\n";
		else
		    cout << "decreases.\n";
	    }
	    else
		cout << "does not change.\n";
	    
	    cout << "- if " << var_list[j]->name << " decreases by "
		    << shift_data[j][i][0][1][0] << ", the LHS ";
	    if(fabs(shift_data[j][i][0][1][1])>DOUBLE_ERROR)
	    {
		if(shift_data[j][i][0][1][1]>0)
		    cout << "increases.\n";
		else
		    cout << "decreases.\n";
	    }
	    else
		cout << "does not change.\n";
	    
	    cout << "- if " << var_list[j]->name << " increases by "
		    << shift_data[j][i][1][0][0] << ", the RHS ";
	    if(fabs(shift_data[j][i][1][0][1])>DOUBLE_ERROR)
	    {
		if(shift_data[j][i][1][0][1]>0)
		    cout << "increases.\n";
		else
		    cout << "decreases.\n";
	    }
	    else
		cout << "does not change.\n";
	    
	    cout << "- if " << var_list[j]->name << " decreases by "
		    << shift_data[j][i][1][1][0] << ", the RHS ";
	    if(fabs(shift_data[j][i][1][1][1])>DOUBLE_ERROR)
	    {
		if(shift_data[j][i][1][1][1]>0)
		    cout << "increases.\n";
		else
		    cout << "decreases.\n";
	    }
	    else
		cout << "does not change.\n";
	}
	cout << endl;
    }
    
    //revert expr values to original
    for(int j=0; j<expr_list.size(); j++)
    {
	expr_list[j].EvaluateLHS(); //LHS - positive
	expr_list[j].EvaluateRHS(); //LHS - positive
    }
    
}

int ExpressionsTruthValue(vector<vector<vector<vector<vector<double> > > > > &shift_data,
			  vector<ExpressionClass> &expr_list,vector<Variable*> &var_list,
			  vector<double>& change)
{
    //shift_data<VARIABLE<EXPRESSION<LHS|RHS<+|-<AMOUNT & EFFECT> > > > >
    //assesses truth value for all expressions:
    //	returns - # of unsatisfied expressions
    //	recommend: recommended varaible to change
    //	change: minimal recommended intesity of change
    
    vector<double> change_list;
    vector<Variable*> mini_list;
    double tmpdbl;
    double var_change;
    string tmpstr;
    int ans = 0;
    int side = 0;

    for(int i=0; i<var_list.size(); i++)
    {
	change_list.push_back(0);
    }
    
    for(int i=0; i<expr_list.size(); i++)
    {
	tmpstr = expr_list[i].GetComparisson();
	//which side and by how much
	if(!expr_list[i].GetSideComparison(tmpdbl))
	{
	    ans++;
	    if(tmpstr==">")
	    {
		tmpdbl = (fabs(tmpdbl) + 10*DOUBLE_ERROR)/2;//half for each side
		side = 1; //left side positive
	    }
	    if(tmpstr=="<")
	    {
		tmpdbl = (fabs(tmpdbl) + 10*DOUBLE_ERROR)/2;;
		side = 2; //right side positive
	    }
	    if(tmpstr==">=")
	    {
		tmpdbl = fabs(tmpdbl);
		side = 1; //left side positive
	    }
	    if(tmpstr=="<=")
	    {
		tmpdbl = fabs(tmpdbl);
		side = 2; //right side positive
	    }
	    if(tmpstr=="==")//increases to match
	    {
		
		if(tmpdbl < 0)
		{
		    side = 1;//left side positive
		}
		else
		{
		    side = 2;//right side positive
		}
		tmpdbl = fabs(tmpdbl)/2;
	    }
	    if(tmpstr=="!=")//increases for mis-match
	    {
		tmpdbl = 10 * DOUBLE_ERROR;
		if(time(NULL)%2)
		    side = 1;
		else
		    side = 2;
	    }
	}
	else continue;//THIS CAN BE ADDED SO THAT EXPRESSIONS THAT ARE MARGINALLY TRUE ARE AVOIDED

	expr_list[i].GetVarLinks(mini_list,true);//LHS_LISTING | mini_list does not have repeating elements
	if(side==2)
	    tmpdbl = -1*tmpdbl;
	for(int j=mini_list.size()-1;j>=0;j--)
	{
	    for(int a=0; a<var_list.size();a++)
	    {
		if(var_list[a]==mini_list[j])
		{
		    if(side == 2)
		    {
			if(fabs(shift_data[a][i][0][1][1])>DOUBLE_ERROR &&  //nonzero
			   shift_data[a][i][0][1][1] < 0)			//negative
			{
			    var_change = (shift_data[a][i][0][1][0]*tmpdbl)/shift_data[a][i][0][1][1];
			}
			else if(fabs(shift_data[a][i][0][0][1])>DOUBLE_ERROR &&  //nonzero
				shift_data[a][i][0][0][1] < 0)			//negative
			{
			    var_change = (shift_data[a][i][0][0][0]*tmpdbl)/shift_data[a][i][0][0][1];
			}
			else
			{var_change = 0;}
		    }else
		    {
			if(fabs(shift_data[a][i][0][0][1])>DOUBLE_ERROR &&  //nonzero
			   shift_data[a][i][0][0][1] > 0)			//negative
			{
			    var_change = (shift_data[a][i][0][0][0]*tmpdbl)/shift_data[a][i][0][0][1];
			}
			else if(fabs(shift_data[a][i][0][1][1])>DOUBLE_ERROR &&  //nonzero
				shift_data[a][i][0][1][1] > 0)			//negative
			{
			    var_change = (shift_data[a][i][0][1][0]*tmpdbl)/shift_data[a][i][0][1][1];
			}
			else
			{var_change = 0;}
		    }
		    if(fabs(change_list[a])>DOUBLE_ERROR && fabs(var_change)>DOUBLE_ERROR)
		    {
			if(change_list[a]>0 && var_change>0)
			{
			    if(change_list[a]<var_change)
				change_list[a]=var_change;
			}else if (change_list[a]<0 && var_change<0)
			{
			    if(change_list[a]>var_change)
				change_list[a]=var_change;
			}
			else
			    change_list[a]+=var_change;
		    }
		    else
			change_list[a]+=var_change;
		}
	    }
	}
	
	expr_list[i].GetVarLinks(mini_list,false);//RHS_LISTING | mini_list does not have repeating elements
	if(side==1)
	    tmpdbl = -1*tmpdbl;
	for(int j=mini_list.size()-1;j>=0;j--)
	{
	    for(int a=0; a<var_list.size();a++)
	    {
		if(var_list[a]==mini_list[j])
		{
		    if(side == 1)
		    {
			if(fabs(shift_data[a][i][1][1][1])>DOUBLE_ERROR &&  //nonzero
			   shift_data[a][i][1][1][1] < 0)			//negative
			{
			    var_change = (shift_data[a][i][1][1][0]*tmpdbl)/shift_data[a][i][1][1][1];
			}
			else if(fabs(shift_data[a][i][1][0][1])>DOUBLE_ERROR &&  //nonzero
				shift_data[a][i][1][0][1] < 0)			//negative
			{
			    var_change = (shift_data[a][i][1][0][0]*tmpdbl)/shift_data[a][i][1][0][1];
			}
			else
			{var_change = 0;}
		    }else
		    {
			if(fabs(shift_data[a][i][1][0][1])>DOUBLE_ERROR &&  //nonzero
			   fabs(shift_data[a][i][1][0][1]) > 0)			//negative
			{
			    var_change = (shift_data[a][i][1][0][0]*tmpdbl)/shift_data[a][i][1][0][1];
			}
			else if(fabs(shift_data[a][i][1][1][1])>DOUBLE_ERROR &&  //nonzero
				fabs(shift_data[a][i][1][1][1]) > 0)			//negative
			{
			    var_change = (shift_data[a][i][1][1][0]*tmpdbl)/shift_data[a][i][1][1][1];
			}
			else
			{var_change = 0;}
		    }
		    if(fabs(change_list[a])>DOUBLE_ERROR && fabs(var_change)>DOUBLE_ERROR)
		    {
			if(change_list[a]>0 && var_change>0)
			{
			    if(change_list[a]<var_change)
				change_list[a]=var_change;
			}else if (change_list[a]<0 && var_change<0)
			{
			    if(change_list[a]>var_change)
				change_list[a]=var_change;
			}
			else
			    change_list[a]+=var_change;
		    }
		    else
			change_list[a]+=var_change;
		}
	    }
	}
	
    }
    
    change = change_list;
    return ans;
}
