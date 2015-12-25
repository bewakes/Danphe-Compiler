#include "stdinc.h"
#include "lex.h"
#include "parserMisc.h"
#include "parser.h"

// Get next token while parsing
#define NextToken       if (tk<Tokens.size()-1) tk++
// The current token being analysed while parsing
#define CToken           Tokens[tk]
// Throw out an error if we run out of tokens in the middle of parsing a statement
#define ErrOutOfTokens if (tk>=Tokens.size()-1 && tk!=(unsigned)-1) Error("Program ended unexpectedly", CToken.LineStart)

long unsigned tk;
Node * ParseTree;
int CurrentFunction=-1;

void ParseFunction(Node*& nd, int FuncIndex)
{
    NextToken;
    if (CToken.Str!="(") Error("Expected list of function parameters",CToken.LineStart);

    NextToken;
    CreateTree(nd);
    nd->Value="NULL_PARAMETER";
    Node** nd_=&nd;
    unsigned PCnt=0;
    if (CToken.Type==EOL)Error("Expected closing bracket ')'",CToken.LineStart);
    while (CToken.Str!=")")
    {
        PCnt++;
        if (PCnt>Funcs[FuncIndex].Parameters.size()) Error("Invalid number of parameters",CToken.LineStart);
        (*nd_)->Value="PARAMETER";
        int Type=ParseExpression((*nd_)->Left, MaxOprPd);
        if (!CheckTypes(Type,Funcs[FuncIndex].Parameters[PCnt-1].Type))
            Error("Different type of expression expected",CToken.LineStart);
        (*nd_)->Attribs[0]=Cls[Type].Name;
        if (CToken.Type==EOL)Error("Expected closing bracket ')'",CToken.LineStart);
        if (CToken.Str!="," && CToken.Str!=")")
            Error("Separation of parameters with comma is needed",CToken.LineStart);
        if (CToken.Str==",")NextToken;
        CreateTree((*nd_)->Right);
        (*nd_)->Right->Value="NULL_PARAMETER";
        nd_=&((*nd_)->Right);
    }
    if (PCnt!=Funcs[FuncIndex].Parameters.size()) Error("Invalid number of parameters",CToken.LineStart);

}
int ParseMember(Node*& nd, int cls)
{
    NextToken;
    NextToken;
    CreateTree(nd);
    if (CToken.Type!=IDENTIFIER) Error("Invalid member of a class/structure",CToken.LineStart);
    int Member = GetMember(cls,CToken.Str);
    if (Member==-1)Error("Invalid member of a class/structure",CToken.LineStart);
    nd->Value=CToken.Str;
    int Type = Cls[cls].Members[Member].Type;
    if (Tokens[tk+1].Str==".")
    {
        nd->Attribs[1]="CLASS";
        Type=ParseMember(nd->Right, Type);
    }
    else
        nd->Attribs[1]="MEMBER";
    return Type;
}
string GetNumValType(string Tkn)
{
    if (Tkn.find(".",0)!=string::npos)
        return "DOUBLE";
    return "INTEGER";
}
int ParseFactor(Node*& nd)
{
    ErrOutOfTokens;
    int Type=-1;

    if (CToken.Str=="&")
    {
        NextToken;
        CreateTree(nd);
        Type=ParseFactor(nd->Left);
        if (nd->Left->Attribs[1]!="VARIABLE" && nd->Left->Attribs[1]!="CLASS")
            Error("Operator & are only for variables and class members",CToken.LineStart);
        nd->Value="&";
        Type=AddPtrClass(Type,1);
        nd->Attribs[0]=Cls[Type].Name;
        return Type;
    }
    int UnId=GetUnOpr(CToken.Str);
    if (UnId!=-1)
    {
        bool Ptr=false;
        if (CToken.Str=="*")Ptr=true;
        NextToken;
        CreateTree(nd);
        Type=ParseFactor(nd->Left);
        if (Ptr && Cls[Type].Name.substr(0,3)!="PTR")
            Error("Operator * are only for pointers",CToken.LineStart);
        UnId = CheckUnOprType(UnId,Type);
        if (UnId==-1)
            Error("This unary operator does not support such type of factor",CToken.LineStart);
        nd->Value=UnOprs[UnId].Name;
        nd->Attribs[1]="UNARY_OPERATOR";
        Type=UnOprs[UnId].RtCls;
        nd->Attribs[0]=Cls[Type].Name;
        return Type;
    }

    // check for bracketed expression
    if (CToken.Str=="(")
    {
        NextToken;
        Type=ParseExpression(nd,MaxOprPd);
        if (CToken.Str!=")") Error("Expected closing bracket ')'",CToken.LineStart);
        NextToken;
        return Type;
    }

    // If not bracketed expression, it is either an identifier or a constant
    CreateTree(nd);
    nd->Value = CToken.Str;

    if (CToken.Type==IDENTIFIER && !CheckVar(CToken.Str) && !CheckFunc(CToken.Str)
        && CToken.Str!="TRUE" && CToken.Str!="FALSE")
        Error("Undeclared variable name used in expression",CToken.LineStart);

    if (CheckFunc(CToken.Str))
    {
        nd->Attribs[0]=Cls[GetType(CToken.Str)].Name;
        nd->Attribs[1]="FUNCTION";
        ParseFunction(nd->Left,GetFuncIndex(CToken.Str));
        if (Tokens[tk+1].Str==".")
        {
            nd->Attribs[1]="FUNCTION_CLASS";
            nd->Attribs[0]=Cls[ParseMember(nd->Right,GetType(nd->Value))].Name;
        }
    }
    else if (CToken.Type==IDENTIFIER)
    {
        if (CToken.Str=="TRUE" || CToken.Str=="FALSE")
        {
            nd->Attribs[1]="VALUE";
            nd->Attribs[0]="BOOLEAN";
        }
        else if (Tokens[tk+1].Str==".")
        {
            nd->Attribs[1]="CLASS";
            nd->Attribs[0]=Cls[ParseMember(nd->Right,GetType(CToken.Str))].Name;
        }
        else
        {
            nd->Attribs[0]=Cls[GetType(CToken.Str)].Name;
            nd->Attribs[1]="VARIABLE";
        }
    }
    else
    {
        if (CToken.Type==STRING) nd->Attribs[0]="STRING";
        else if (CToken.Type==NUMBER) nd->Attribs[0]=GetNumValType(CToken.Str);
        else
            Error("Invalid factor type used in an expression",CToken.LineStart);
        nd->Attribs[1]="VALUE";
    }
    NextToken;

    return GetClass(nd->Attribs[0]);
}

int ParseExpression(Node*& nd, int Pd)
{
    ErrOutOfTokens;
    int LType=-1;
    if (Pd==1) LType=ParseFactor(nd);
    else LType=ParseExpression(nd, Pd-1);
    while (IsOpr(CToken.Str) && GetPd(CToken.Str)==Pd)
    {

        int OprCls = GetOprCls(CToken.Str,LEFT, LType);
        if (!CheckTypes(OprCls,LType))
            Error("This operator doesn't support such left argument",CToken.LineStart);
        string Opr=CToken.Str;
        // Everything is parsed and tested according as
        // LType Opr RType where LType is another expression
        // i.e. 2+3-4-8 = ((2+3)-4)-8 so result of final operator is the result of the expression

        Node tmp;
        tmp = *nd;
        delete nd;
        CreateTree(nd);
        CreateTree(nd->Left);
        *(nd->Left)=tmp;
        nd->Value=CToken.Str;
        nd->Attribs[1]="OPERATOR";
        NextToken;

        int RType;
        if (Pd==1) RType=ParseFactor(nd->Right);
        else RType=ParseExpression(nd->Right, Pd-1);
        OprCls = GetOprCls(Opr,RIGHT,RType,LType);
        if (!CheckTypes(OprCls,RType))
            Error("This operator doesn't support such right argument",CToken.LineStart);

        LType=GetRetOpr(Opr,LType,RType);
        nd->Attribs[0]=Cls[LType].Name;
    }
    return LType;
}

void DeclareVar(Node* nd)
{
    nd->Value = "DECLARE";
    nd->Attribs[1] = CToken.Str;
    NextToken;
    ErrOutOfTokens;
    int ptrs=0;
    while (CToken.Str=="*")
    {
        ptrs++;
        NextToken;
    }
    nd->Attribs[0] = CToken.Str;

    if (CToken.Type!=IDENTIFIER || IsReserved(CToken.Str))
        Error("Invalid variable name used in declaration",CToken.LineStart);

    if (ptrs>0) nd->Attribs[1]=Cls[AddPtrClass(GetClass(nd->Attribs[1]),ptrs)].Name;

    VarInfo var;
    var.Name = CToken.Str;
    var.Type=GetClass(nd->Attribs[1]);
    Vars.push_back(var);

    if (CurrentFunction!=-1)
    {
        Funcs[CurrentFunction].LocalSize+=GetSize(var.Type);
        nd->Attribs[2]=ToStr(Funcs[CurrentFunction].LocalSize);
    }
    else
        nd->Attribs[2]="";

    NextToken;
}

void Assign(Node* nd)
{
    nd->Value = "ASSIGN";
    CreateTree(nd->Left);
    nd->Left->Value = CToken.Str;
    if (!CheckVar(nd->Left->Value))
        Error("Undeclared variable name used in assignment",CToken.LineStart);
    int myType;
    if (Tokens[tk+1].Str==".")
    {
        nd->Left->Attribs[1]="CLASS";
        myType = ParseMember(nd->Left->Right,GetType(nd->Left->Value));
    }
    else
        myType = GetType(nd->Left->Value);
    NextToken;
    NextToken;
    int Type=ParseExpression(nd->Right, MaxOprPd);
    if (!CheckTypes(Type,myType))
        Error("Different type of expression expected",CToken.LineStart);

    nd->Attribs[0]=Cls[myType].Name;
}

void DefnFunc(Node* nd)
{
    nd->Value = "FUNCTION";
    CreateTree(nd->Left);
    nd->Left->Attribs[0]=CToken.Str; //Type
    NextToken;
    nd->Left->Value=CToken.Str; //Name

    for (unsigned i=0;i<Funcs.size();i++)
    {
        if (Funcs[i].Name==CToken.Str)
            CurrentFunction=i;
    }
    Funcs[CurrentFunction].LocalSize=0;
    while (CToken.Str!=")")NextToken;
    NextToken;

    while (CToken.Type==EOL)
    {
        ln++;
        NextToken;
    }
    NextToken;

    CreateTree(nd->Right);
    nd = nd->Right;
    while (CToken.Str!="}")
    {
        if (CToken.Type!=EOL){
            if (ln==lines-1)ParseStatement(nd);
            else
            {
                nd->Value="STATEMENTS";
                CreateTree(nd->Left);
                CreateTree(nd->Right);
                ParseStatement(nd->Left);
                nd = nd->Right;
            }

            if (CToken.Type!=EOL && CToken.Str!="}") Error("End Of Line Expected!",CToken.LineStart);
        }
        NextToken;
        ln++;
    }
    CurrentFunction=-1;
    NextToken;
}
void Return(Node* nd)
{
    nd->Value="RETURN";
    if (CurrentFunction==-1) Error("Where in HELL do you want to return from ?!",CToken.LineStart);
    NextToken;
    int Type=ParseExpression(nd->Right,MaxOprPd);
    nd->Attribs[0]=Cls[Funcs[CurrentFunction].Type].Name;
    if (!CheckTypes(Type,Funcs[CurrentFunction].Type))
        Error("Return type and function type mismatched",CToken.LineStart);
}
void DefnCls()
{
    while (CToken.Str!="}")NextToken;
    NextToken;
}
void Write(Node* nd)
{
    NextToken;
    nd->Value="WRITE";
    if (CheckVar(CToken.Str))
    {
        nd->Attribs[1]=Cls[GetType(CToken.Str)].Name;
        nd->Attribs[0]=CToken.Str;
        NextToken;
    }
    else
        Error("INVALID WRITE STATEMENT", CToken.LineStart);
}
void ParseStatement(Node* nd)
{
    if (IsClass(CToken.Str))
    {
        if (Tokens[tk+2].Str=="(")
            DefnFunc(nd);
        else
            DeclareVar(nd);
    }
    else if (CToken.Type==IDENTIFIER && (Tokens[tk+1].Str=="=" || Tokens[tk+1].Str=="."))
        Assign(nd);
    else if (CToken.Str=="STRUCTURE")
        DefnCls();
    else if (CToken.Str=="WRITE")
        Write(nd);
    else if (CToken.Str=="RETURN")
        Return(nd);
    else
        Error("INVALID STATEMENT", CToken.LineStart);
}
void DeclCls()
{
    if (CToken.Str=="STRUCTURE")
    {
        ClassInfo cls;

        NextToken;
        if (CToken.Type!=IDENTIFIER || IsReserved(CToken.Str))
            Error("Invalid structure/class name used",CToken.LineStart);
        cls.Name=CToken.Str;
        cls.Size=0;
        NextToken;
        while (CToken.Type==EOL)
        {
            ln++;
            if (ln>=lines)
                Error("Expected structure/class definition starting with '{'",CToken.LineStart);
            NextToken;
        }
        if (CToken.Str!="{")Error("Expected structure/class definition starting with '{'",CToken.LineStart);
        NextToken;
        while (CToken.Str!="}")
        {
            if (CToken.Type!=EOL)
            {
                VarInfo member;
                if (!IsClass(CToken.Str))Error("Invalid Member Type!",CToken.LineStart);
                member.Type=GetClass(CToken.Str);
                NextToken;
                if (CToken.Type!=IDENTIFIER || IsReserved(CToken.Str))
                    Error("Invalid member name used",CToken.LineStart);
                for (unsigned i=0;i<cls.Members.size();i++)
                    if (cls.Members[i].Name==CToken.Str)
                        Error("Two members with same name used",CToken.LineStart);
                member.Name=CToken.Str;
                cls.Members.push_back(member);
                cls.Size += GetSize(member.Type);
                NextToken;

                if (CToken.Type!=EOL && CToken.Str!="}") Error("End Of Line Expected!",CToken.LineStart);
            }
            NextToken;
            if (ln>=lines)
                Error("Expected end of structure/class definition with '}'",CToken.LineStart);
            ln++;
        }
        NextToken;
        Cls.push_back(cls);
    }
    else
    {
        while (CToken.Type!=EOL)
            NextToken;
    }
}
void DeclFunc()
{
    if (IsClass(CToken.Str)&&Tokens[tk+2].Str=="(")
    {
        FuncInfo func;
        func.Type=GetClass(CToken.Str);
        NextToken;
        if (CToken.Type!=IDENTIFIER || IsReserved(CToken.Str))
            Error("Invalid function name used",CToken.LineStart);
        func.Name=CToken.Str;
        NextToken;
        NextToken;
        func.ParamSize=0;
        while (CToken.Str!=")")
        {
            VarInfo parm;
            if (!IsClass(CToken.Str))Error("Invalid Parameter Type!",CToken.LineStart);
            parm.Type=GetClass(CToken.Str);
            NextToken;
            if (CToken.Type!=IDENTIFIER || IsReserved(CToken.Str))
                Error("Invalid parameter name used",CToken.LineStart);
            for (unsigned i=0;i<func.Parameters.size();i++)
                if (func.Parameters[i].Name==CToken.Str)
                    Error("Two parameters with same name used",CToken.LineStart);
            parm.Name=CToken.Str;
            func.Parameters.push_back(parm);
            func.ParamSize+=GetSize(parm.Type);

            NextToken;
            if (CToken.Type==EOL)
                Error("End of line while expecting ')'",CToken.LineStart);
            if (CToken.Str!="," && CToken.Str!=")")
                Error("Separation of parameters with comma is needed",CToken.LineStart);
            if (CToken.Str==",") NextToken;
        }
        NextToken;
        while (CToken.Type==EOL)
        {
            ln++;
            if (ln>=lines)
                Error("Expected function definition starting with '{'",CToken.LineStart);
            NextToken;
        }
        if (CToken.Str!="{")
            Error("Expected function definition starting with '{'",CToken.LineStart);
        while (CToken.Str!="}")
        {
            NextToken;
            if (CToken.Type==EOL)
            {
                ln++;
                if (ln>=lines)
                    Error("Expected end of function definition with '}'",CToken.LineStart);
                NextToken;
            }
        }
        NextToken;
        Funcs.push_back(func);
    }
    else
    {
        while (CToken.Type!=EOL)
            NextToken;
    }
}
void ParseProgram()
{
    BuildClass();
    tk=0;
    for (ln=0;ln<lines;ln++)
    {
        if (CToken.Type!=EOL)
            DeclCls();
        if (CToken.Type!=EOL) Error("End Of Line Expected!",CToken.LineStart);
        NextToken;
    }
    tk=0;
    for (ln=0;ln<lines;ln++)
    {
        if (CToken.Type!=EOL)
            DeclFunc();
        if (CToken.Type!=EOL) Error("End Of Line Expected!",CToken.LineStart);
        NextToken;
    }
    tk=0;
    CreateTree(ParseTree);
    Node* nd = ParseTree;
    for (ln=0;ln<lines;ln++)
    {
        if (CToken.Type!=EOL){
            if (ln==lines-1)ParseStatement(nd);
            else
            {
                nd->Value="STATEMENTS";
                CreateTree(nd->Left);
                CreateTree(nd->Right);
                ParseStatement(nd->Left);
                nd = nd->Right;
            }

            if (CToken.Type!=EOL) Error("End Of Line Expected!",CToken.LineStart);
        }
        NextToken;
    }
}
