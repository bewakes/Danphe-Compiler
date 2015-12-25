#include "stdinc.h"
#include "parserMisc.h"
#include "codegen.h"
#include <map>

//*****************Data Base***************************
map<string, string> Asms;
string GetAsmName(string Name)
{
    return Asms[Name];
}
void AddAsmName(string Name, string AsmName)
{
    Asms[Name]=AsmName;
}

string Data="";
string Bss="";
string Code="";
string Final="";
string FuncCode="";

string TmpFinal="";

string* CurrentCode=&Code;
string* CurrentFinal=&Final;
void AddData(string st)
{
    Data+="\n"+st;
}
void AddBss(string st)
{
    Bss+="\n"+st;
}
void AddCode(string st)
{
    *CurrentCode+="\n"+st;
}
void AddEnd(string st)
{
    *CurrentFinal+="\n"+st;
}

map<string, string> Tmps;
unsigned long tmp=0;
string GetTmp(string ForValue, int Type)
{
    map<string,string>::iterator it;
    for(it=Tmps.begin();it!=Tmps.end();it++)
        if (it->first==ForValue) return it->second;
    char buffer[30];
    sprintf(buffer, "tmp%lu",tmp);
    tmp++;
    Tmps[ForValue]=buffer;
    if (Type==QWORD)
        AddData((string)buffer+" dq "+ForValue);
    else if (Type==BYTE)
        AddData((string)buffer+" db "+ForValue);
    else if (Type==DWORD)
        AddData((string)buffer+" dd "+ForValue);
    return buffer;
}
string GetNewTmp()
{
    char buffer[30];
    sprintf(buffer, "tmp%lu",tmp);
    tmp++;
    return buffer;
}
//******************************************************

// The '&' operator
void GetAddress(Node* nd)
{
    string varName=GetAsmName(nd->Left->Value);
    if (nd->Left->Attribs[1]=="CLASS")
    {
        ClassMember(nd->Left->Right,GetType(nd->Left->Value),varName);
        AddCode("lea eax, ["+varName+"]");
        varName="eax";
    }
    AddCode("push "+varName);

}

//***************************StrCleaning***********************

void CalcClsStrParams(int cls, int InOffset, int &cnt)
{
    int offset=0;
    for (unsigned i=0;i<Cls[cls].Members.size();i++)
    {
        string varName = "esp+"+ToStr(InOffset+offset);
        if (Cls[cls].Members[i].Type==GetClass("STRING"))
        {
            cnt+=1;
            AddCode("push dword["+varName+"]");
        }
        else if (CheckIfClass(Cls[cls].Members[i].Type))
            CalcClsStrParams(Cls[cls].Members[i].Type, InOffset+offset, cnt);

        offset+=Cls[Cls[cls].Members[i].Type].Size;
    }
}
void CalcStrParams(Node*nd, int offset, int &cnt)
{
    if (nd->Value=="NULL_PARAMETER")return;
    CalcStrParams(nd->Right,offset+GetSize(GetClass(nd->Attribs[0])),cnt);
    if (nd->Attribs[0]=="STRING")
    {
        cnt+=1;
        AddCode("push dword[esp+"+ToStr(offset)+"]");
    }
    else if (CheckIfClass(GetClass(nd->Attribs[0])))
    {
        CalcClsStrParams(GetClass(nd->Attribs[0]),offset, cnt);
    }
}

void CleanStrParams(int cnt)
{
    for (int i=0;i<cnt;i++)
    {
        AddCode("call free");
        AddCode("add esp, 4");
    }
}
void CleanStack(Node*nd, int n)
{
    AddCode("mov ebx, eax");
    int cnt=0;
    CalcStrParams(nd,0,cnt);
    CleanStrParams(cnt);
    AddCode("add esp,"+ToStr(n));
}
void PopClsStr(string Name, int cls)
{
    int offset=0;
    for (unsigned i=0;i<Cls[cls].Members.size();i++)
    {
        string varName = Name+"+"+ToStr(offset);
        if (Cls[cls].Members[i].Type==GetClass("STRING"))
        {
            AddCode("push dword["+varName+"]");
            AddCode("call free");
            AddCode("add esp, 4");
            AddCode("pop dword["+varName+"]");
        }
        else if (Cls[cls].Members[i].Type==GetClass("DOUBLE"))
        {
            AddCode("pop dword["+varName+"]");
            AddCode("pop dword["+varName+"+4]");
        }
        else if (CheckIfClass(Cls[cls].Members[i].Type))
            PopClsStr(varName,Cls[cls].Members[i].Type);
        else
            AddCode("pop dword["+varName+"]");
        offset+=Cls[Cls[cls].Members[i].Type].Size;
    }
}
//*************************************************************
//*******************Cls Operations***********************

void PopCls(string Name, int cls)
{
    int offset=0;
    for (unsigned i=0;i<Cls[cls].Members.size();i++)
    {
        string varName = Name+"+"+ToStr(offset);
        if (Cls[cls].Members[i].Type==GetClass("DOUBLE"))
        {
            AddCode("pop dword["+varName+"]");
            AddCode("pop dword["+varName+"+4]");
        }
        else if (CheckIfClass(Cls[cls].Members[i].Type))
            PopCls(varName,Cls[cls].Members[i].Type);
        else
            AddCode("pop dword["+varName+"]");
        offset+=Cls[Cls[cls].Members[i].Type].Size;
    }
}
void FreeCls(string Name, int cls)
{
    int offset=0;
    for (unsigned i=0;i<Cls[cls].Members.size();i++)
    {
        string varName = Name+"+"+ToStr(offset);
        if (Cls[cls].Members[i].Type==GetClass("STRING"))
        {
            AddCode("push dword["+varName+"]");
            AddCode("call free");
            AddCode("add esp, 4");
        }
        else if (CheckIfClass(Cls[cls].Members[i].Type))
            FreeCls(varName,Cls[cls].Members[i].Type);
        offset+=Cls[Cls[cls].Members[i].Type].Size;
    }
}
//*******************************************************
void PassParameters(Node*nd)
{
    if (nd->Value=="NULL_PARAMETER")return;
    PassParameters(nd->Right);
    CodeExpr(nd->Left,nd->Attribs[0]);
}
void CodeExpr(Node*nd,string Type)
{
    if (nd->Value=="&")
        GetAddress(nd);
    else if (Type=="STRING")
    {
        AddCode("push dword 1");
        StrLen(nd);
        AddCode("call calloc");
        AddCode("add esp, 8");
        AddCode("mov dword[tmpStr],eax");
        CodeStr(nd);
        AddCode("push dword[tmpStr]");
    }
    else if (Type=="INTEGER" || Type.substr(0,3)=="PTR")
        CodeInt(nd);
    else if (Type=="DOUBLE")
        CodeDbl(nd);
    else if (Type=="BOOLEAN")
        CodeBln(nd);
    else
        CodeCls(nd);
}

void CodeAssign(Node* nd)
{
    string varName=GetAsmName(nd->Left->Value);
    if (nd->Left->Attribs[1]=="CLASS")
        ClassMember(nd->Left->Right,GetType(nd->Left->Value),varName);

    CodeExpr(nd->Right,nd->Attribs[0]);

    if (CheckIfClass(GetClass(nd->Attribs[0])))
    {
        PopClsStr(varName,GetClass(nd->Attribs[0]));
        return;
    }
    if (nd->Attribs[0]=="STRING")
    {
        AddCode("push dword["+varName+"]");
        AddCode("call free");
        AddCode("add esp, 4");
    }
    else if (nd->Attribs[0]=="DOUBLE")
        AddCode("pop dword["+varName+"+4]");
    AddCode("pop dword["+varName+"]");

}
void CodeDeclCls(string Name, int cls)
{
    int offset=0;
    for (unsigned i=0;i<Cls[cls].Members.size();i++)
    {
        string varName = Name+"+"+ToStr(offset);
        if (Cls[cls].Members[i].Type==GetClass("STRING"))
        {
            AddCode("push dword 1");
            AddCode("push dword 1");
            AddCode("call calloc");
            AddCode("add esp,8");
            AddCode("mov dword["+varName+"],eax");
            AddEnd("push dword["+varName+"]");
            AddEnd("call free");
            AddCode("add esp, 4");
        }
        else if (CheckIfClass(Cls[cls].Members[i].Type))
            CodeDeclCls(varName,Cls[cls].Members[i].Type);
        offset+=Cls[Cls[cls].Members[i].Type].Size;
    }
}
void CodeDeclare(Node* nd)
{
    string varName;
    if (nd->Attribs[2]=="")
    {
        varName = nd->Attribs[0]+"ASM";
        AddBss(varName+"  resb  "+ToStr(GetSize(GetClass(nd->Attribs[1]))));
    }
    else
        varName = "ebp-"+nd->Attribs[2];
    if (nd->Attribs[1]=="STRING")
    {
        AddCode("push dword 1");
        AddCode("push dword 1");
        AddCode("call calloc");
        AddCode("add esp, 8");
        AddCode("mov dword["+varName+"], eax");
        AddEnd("push dword["+varName+"]");
        AddEnd("call free");
        AddEnd("add esp, 4");
    }
    else if (CheckIfClass(GetClass(nd->Attribs[1])))
        CodeDeclCls(varName,GetClass(nd->Attribs[1]));
    else if (nd->Attribs[1]=="BOOLEAN" || nd->Attribs[1].substr(0,3)=="PTR")
        AddCode("mov dword["+varName+"],0");

    AddAsmName(nd->Attribs[0],varName);
}
void DeclareParameters(string Function)
{
    int id = GetFuncIndex(Function);
    unsigned sz=0;
    for (unsigned i=0;i<Funcs[id].Parameters.size();i++)
    {
        AddAsmName(Funcs[id].Parameters[i].Name,"ebp+8+"+ToStr(sz));
        sz+=GetSize(Funcs[id].Parameters[i].Type);
    }
}
void CodeWrite(Node* nd)
{
    string varName = GetAsmName(nd->Attribs[0]);
    if (nd->Attribs[1]=="STRING")
    {
        AddCode("push dword["+varName+"]");
        AddCode("call printf");
        AddCode("add esp, 4");
    }
    else if (nd->Attribs[1]=="INTEGER" || nd->Attribs[1]=="BOOLEAN")
    {
        AddCode("push dword["+varName+"]");
        AddCode("push istr");
        AddCode("call printf");
        AddCode("add esp, 8");
    }
    else if (nd->Attribs[1]=="DOUBLE")
    {
        AddCode("push dword["+varName+"+4]");
        AddCode("push dword["+varName+"]");
        AddCode("push dstr");
        AddCode("call printf");
        AddCode("add esp, 8");
    }
}

void CodeGen(Node* nd)
{
    if (nd->Value=="STATEMENTS")
    {
        CodeGen(nd->Left);
        CodeGen(nd->Right);
        return;
    }
    else if (nd->Value=="DECLARE")
        CodeDeclare(nd);
    else if (nd->Value=="ASSIGN")
        CodeAssign(nd);
    else if (nd->Value=="WRITE")
        CodeWrite(nd);
    else if (nd->Value=="FUNCTION")
    {
        FuncCode+="\n";
        TmpFinal = "";
        CurrentCode = &FuncCode;
        CurrentFinal = &TmpFinal;

        CurrentFunction = GetFuncIndex(nd->Left->Value);
        DeclareParameters(nd->Left->Value);

        AddCode(nd->Left->Value+"ASM:\nenter "+ToStr(GetLocalSize(nd->Left->Value))+",0\n");
        CodeGen(nd->Right);
        FuncCode += TmpFinal;
    /* TO REMOVE LATER----------------------------------- */
     //   FuncCode += "\nmov eax,0\n";
     //   FuncCode += "leave\nret";
    /* TILL HERE----------------------------------------- */
        CurrentCode = &Code;
        CurrentFinal = &Final;
    }
    else if (nd->Value=="RETURN")
    {
        CodeExpr(nd->Right,nd->Attribs[0]);
        if (nd->Attribs[0]=="DOUBLE")
        {
            AddCode("pop dword[tmpDbl1+4]");
            AddCode("pop dword[tmpDbl1]");
            AddCode("fld qword[tmpDbl1]");
        }
        else if (CheckIfClass(GetClass(nd->Attribs[0])))
        {
            AddCode("push dword 1");
            AddCode("push dword "+ToStr(GetSize(GetClass(nd->Attribs[0]))));
            AddCode("call calloc");
            AddCode("add esp,8");
            AddCode("mov ebx, eax");
            PopCls("ebx",GetClass(nd->Attribs[0]));
            AddCode("mov eax, ebx");
        }
        else
            AddCode("pop eax");

        AddCode("leave");
        AddCode("ret");
        CurrentFunction=-1;
    }
    AddCode("");
}
