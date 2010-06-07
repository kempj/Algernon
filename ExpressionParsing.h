#ifndef EXPRESSIONS
#define EXPRESSIONS 0

#include<iostream>
#include<string>
#include<vector>

struct Token
{
     std::string token;
     char type; //n = number | i = identifier | o = operator | e = empty | 0 = null 
                //d = divider (i.e. parantheses or brackets)
};

Token GetToken(std::istream& in); 
bool IsLetter(char ch){return ((ch >= 65 && ch <= 90)||(ch >= 97 && ch <= 122))?true:false;} 
ExprStep ParseExpression (std::istream& in, Token& t,ExprStep &s,std::vector<ExprStep> &v) //returns a std::vector steps used to calculate expression
{
    t = GetToken(in);
    if(t.type == 'o' && (t.token=="-" || t.token=="+"))
    {
         s.type = 'u';
         s.data.ufptr = NULL; //this is temporary
         v.insert(v.end,s);
         v.insert(v.end,ParseExpression(in,t,s,v));
    }
    
    
    //after expression stuff
    t=GetToken(in);
    /*
    if op....
    else if anything else, end of expression
    */
}

Token GetToken(std::istream& in)
{
    Token t;
    char ch;
    cin.get(ch);
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
              cin.get(ch);
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
            ch == '/' || ch == '*' || ch == '%')
    {
         t.token = ch;
         t.type = 'o';
    }
    else if((ch == '(' || ch == ')' || ch == '[' ||
            ch == ']' || ch == '{' || ch == '}')
    {
         t.token = ch;
         t.type = 'd';
    }
    else
    {
         t.token = "0xERROR"; //error token: not valid number or identifier
         t.type = 'e';
    }   
    return t;
}



#endif
